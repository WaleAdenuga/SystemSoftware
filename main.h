#ifndef MAIN_H_
#define MAIN_H_

#define _GNU_SOURCE

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
#include <errno.h>

#ifndef TIMEOUT
    #error TIMEOUT for connection manager not specified
#endif

#define MAX 200
/* #define FIFO_NAME "logFifo"
#define FIFO_LOG "gateway.log" */

//thread and buffer variables
pthread_t threads[3]; //three threads that need to be made
sbuffer_t *sbuffer;
pthread_mutex_t fifolock; //lock and unlock for the fifo pipeline (to make it thread safe)

//Now for the methods that would be run

//Connection manager, port number given at compile time
void *start_connmgr(void *port_no);

//Start database manager
void *start_db();
//Data(file) manager
void *start_data();

//main function
int main(int argc, char *argv[]);
void start_condition(void);

//dealing with the main and log process
void run_child(void);
void run_parent(char *argv[]);
void final_message(void);

//Now we create functions for the log processes, all the FIFOs I call it
void initialize_fifo();
void write_fifo(char *send_buf);
void read_fifo();

//shut it all down
void close_gateway();

//pthread err handler
void pthread_err_handler(int err_code, char*msg, char*file_name, char line_nr);

#endif //MAIN_H_