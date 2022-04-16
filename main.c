#include "main.h"
#include <unistd.h>
#include <sys/types.h>
#include "errmacros.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <wait.h>
#include "sbuffer.h"
#include "sensor_db.h"
#include "connmgr.h"
#include "datamgr.h"
#include <string.h>
#include <sys/stat.h>
#include "config.h"
#include <fcntl.h>


#define PTHR_CREATE_SUCCESS 0
#define PTHR_JOIN_SUCCESS 0

//Two things, we make two processes, main and log process using fork()
int main(int argc, char*argv[]) {
    if (argc != 2) {
        start_condition(); //port number of TCP connection is given as a command line argument at start-up of the main process
    } 

    pid_t parent_pid, child_pid;
    parent_pid = getpid();
    child_pid = fork(); //create dummy process in parent's image
    SYSCALL_ERROR(child_pid);

    //run the child process, i.e the logging process of all that information
    if (child_pid == 0) run_child();
    else {
        printf("Parent process with pid %d has started\n", parent_pid);
        printf("The parent process created a child with pid %d\n",child_pid);
        printf("Let's get on with it with the sensor gateway\n");
        run_parent(argv);
    }
    return 0;

}

void start_condition(void) {
    printf("For the connection manager, you need to provide a port number\n");
    printf("This is necessary for communication between threads");
}

void run_child(void) {
    //This just reads from the log file, read and exit
    read_fifo();
    exit(EXIT_SUCCESS);
}

void run_parent(char *argv[]) {
    //This creates threads and begins the shared data structure
    //and also initializes the log file

    char *send_buf;

    //initialie fifo and shared structure
    initialize_fifo();
    sbuffer_init((&sbuffer));

    //Time for the creation of the three threads needed
    //first connection manager, and using port number from run time conditions, WRITE
    if (pthread_create(&threads[0], NULL, &start_connmgr, (void*)argv[1]) != PTHR_CREATE_SUCCESS) {
        printf("Main thread couldn't create thread for connection manager\n");
        exit(EXIT_FAILURE);
    }
    //database, READ
    if (pthread_create(&threads[1], NULL, &start_db, NULL) != PTHR_CREATE_SUCCESS) {
        printf("Main thread couldn't create thread for data manager\n");
        exit(EXIT_FAILURE);
    }
    //reading from file, READ
    if (pthread_create(&threads[2], NULL, &start_data, NULL) != PTHR_CREATE_SUCCESS) {
        printf("Main thread couldn't create thread for storage manager\n");
        exit(EXIT_FAILURE);
    }
    //Joining threads, according to google, that means to wait for it complete, i.e blocking current thread until another completes
    if (pthread_join(threads[0], NULL) != PTHR_JOIN_SUCCESS) {
        printf("Main thread says connedtion manager thread join failed\n");
    }
    if (pthread_join(threads[1], NULL) != PTHR_JOIN_SUCCESS) {
        printf("Main thread says data manager join failed\n");
    }
    if (pthread_join(threads[2], NULL) != PTHR_JOIN_SUCCESS) {
        printf("Main thread says storage manager thread join failed\n");
    }

    ASPRINTF_ERROR(asprintf(&send_buf, "Over\n"));
    write_fifo(send_buf);
    free(send_buf);

    sbuffer_free(&sbuffer);
    close_gateway();
}


// LOG PROCESS STARTS HERE, i.e FIFO
void initialize_fifo() { // in case the log file does not already exist, initialize it here
    int create = mkfifo(FIFO_NAME, 0666); //both processes can write to and read from this file
    //idea is from geeksforgeeks.org
    CHECK_MKFIFO(create);

    //during the process of initialization, the scheduler should not switch processes
    //so it's better to lock it while it is working.
    //actually more like initialize the lock for the write fifo
    //initializing mutex dynamically
    if (pthread_mutex_init(&fifolock, NULL) != 0) printf("Mutex initialization failed\n");
}

void read_fifo() {
    //no need to lock, it can operate freely
    FILE *file_fifo, *file_log;
    int result, result2, sequence_number;
    char *str_result;
    char recv_buf[MAX];

    file_fifo = fopen(FIFO_NAME, "r"); //read data from the fifo
    FILE_OPEN_ERROR(file_fifo);

    file_log = fopen(FIFO_LOG, "a"); //plan is to write data from the fifo into the gateway file
    //"a" appends to a file, at the end of the file, file created if it does not exist
    FILE_OPEN_ERROR(file_log);
    fprintf(file_log, "Let's get the part started\n");
    sequence_number = 1;
    
    do {
        str_result = fgets(recv_buf, MAX, file_fifo);
        if (str_result != NULL) {
            //print to log file immediately

            fprintf(file_log, "%d %ld %s", sequence_number, time(NULL), recv_buf);
            sequence_number++; //increment line number like a normal file
        }

    } while (str_result != NULL);

    result = fclose(file_fifo);
    FILE_CLOSE_ERROR(result);

    result2 = fclose(file_log);
    FILE_CLOSE_ERROR(result2);
}

void write_fifo(char *send_buf) { 
    //should be locked when writing so the scheduler does not switch processes
    FILE *file_fifo;
    int result, preresult;

    file_fifo = fopen(FIFO_NAME, "w");
    FILE_OPEN_ERROR(file_fifo);
    preresult = pthread_mutex_lock(&fifolock); 
    pthread_err_handler(preresult, "pthread_mutex_lock", __FILE__, __LINE__); //mainmutex.c

    if (fputs(send_buf, file_fifo) == EOF) {
        fprintf(stderr, "Error writing data to fifo\n");
        exit(EXIT_FAILURE);
    }

    FFLUSH_ERROR(fflush(file_fifo));
    /* printf("Message send: %s", send_buf);
    free(send_buf); */

    result = fclose(file_fifo); //close the fifo when done
    FILE_CLOSE_ERROR(result);
    pthread_mutex_unlock(&fifolock); // thread safe
    pthread_err_handler(preresult, "pthread_mutex_unlock", __FILE__, __LINE__);

}

void close_gateway() { //this is to close the main thread and all the processes
    //most of the thinking process is from wait.c
    pid_t child_pid;
    int child_exit_status;

    child_pid = wait(&child_exit_status); //block calling process until one of child processes exits or signal received
    SYSCALL_ERROR(child_pid);
    if (WIFEXITED(child_exit_status)){
        printf("Child %d terminated with exit status %d\n", child_pid, WEXITSTATUS(child_exit_status));
    } else {
        printf("Child %d terminated abnormally\n", child_pid);
    }

    printf("The hard work is all over now\n");
    printf("The application is completed\n");

    //terminate main thread
    pthread_exit(NULL);
}

//threads are nominally created, log process done, now let's tamper with the actual threads themselves
void *start_connmgr(void *port_no) {
    //start listening for incoming connections 
    int port_number = atoi(port_no); //convert to int
    connmgr_listen(port_number, &sbuffer);

    //free when finished
    connmgr_free();
    pthread_detach(threads[0]); //detach joinable thread
    return NULL;
}

void *start_data() {
    FILE *file = fopen("room_sensor.map", "r");
    FILE_OPEN_ERROR(file);

    datamgr_parse_sensor_files(file, &sbuffer);
    fclose(file); //close file when done with the file
    datamgr_free();
    pthread_detach(threads[1]); //detach the thread when finished with it
    return NULL;
}

void *start_db() {

    //Gateway closes after 3 tries, but we need to avoid hard-coded values in the first place
    for (int i = 0; i<NUMBER_TRIALS_CONNECTION; i++) {
        DBCONN *conn;
        //char *send_buf;

        //initialize database connection
        conn = init_connection(CLEAR_UP_FLAG); //1 would drop the table, anything else creates a new table if it doesn't already exist
        //now proceeding to try and read data from the data structure
        if (conn != NULL) {
            insert_sensor_from_buffer(conn, &sbuffer);
            disconnect(conn);
            break;
        }

        //if connection fails, database waits before trying again
        sleep(WAIT_CONNECTION);
        //close the sensor gateway if connection still doesn't work
        if (i == (NUMBER_TRIALS_CONNECTION-1)) {
            close_buffer(sbuffer);
        }
    }
    pthread_detach(threads[2]); //detach the thread, it is now over
    return NULL;
}

void pthread_err_handler(int err_code, char*msg, char*file_name, char line_nr){
    if (0!= err_code) {
        fprintf(stderr, "\n%s failed with error code %d in file %s at line %d\n", msg,err_code,file_name,line_nr);
    } 
}


//// gcc -Wall -Werror main.c connmgr.h connmgr.c config.h lib/dplist.h lib/dplist.c -pthread lib/tcpsock.c  lib/tcpsock.h -D TIMEOUT=5
//valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./a.out
