#define _GNU_SOURCE

#include "connmgr.h"
dplist_t *list_connections = NULL;

void element_free(void **element) {
    free(*element);
    *element = NULL;
}

void *element_copy (void *element) { //make a copy of the information you receive when you poll
    pollinfo *copy = malloc(sizeof(pollinfo)); //create space on the heap the size of a pollinfo
    copy->file_descriptor = ((pollinfo*) element)->file_descriptor;
    copy->sensor_id = ((pollinfo*)element) ->sensor_id;
    copy->socket_id = ((pollinfo*)element)->socket_id;
    copy->time = ((pollinfo*)element)->time;
    return ((void*) copy);

}

int element_compare (void *a, void *b) { //comparing two poll records received
//The sensor node closes the tcp connection when it's done so there's
//the possibility of having the same socket id

    return 0; //Because I can't think of anything at the moment
    //and turns out I don't even need it in the first place

}

void connmgr_free() {
    //free the list with the connections made
    dpl_free(&list_connections, true);
}

void connmgr_listen(int port_number) {
    //write all processed data to a file
    FILE *file = fopen("sensor_data_recv", "w"); //declare you want to write to the newly opened file

    //create a list that contain connections and pointers to previous and next connections
    list_connections = dpl_create(element_copy,element_free, element_compare);
    tcpsock_t *server, *client;
    pollinfo server_poll;
    polldescr fd_0; //fd is file descriptor and consists of fd, events and revents
    bool check_end = false; //you're not done reading all connections
    int number_connections = 0;
    int result;

    //Check if you can get a connection with the port number provided, i.e start a server
    //creates a new socket in passive listening mode, waiting for a connection
    if (tcp_passive_open(&server, port_number) != TCP_NO_ERROR) {
        printf("The server was not successfully created\n");
    }
    if (tcp_get_sd(server,&(fd_0.fd)) != TCP_NO_ERROR) {
        printf("The socket is not bound and has no descriptor\n");
    }

    //initialize the server's starting position
    //normally you would just initialize the poll description
    //but since you created a poll info that contains the fd, socket id and last event in server
    //you have to initialize that instead
    server_poll.file_descriptor = fd_0;
    server_poll.file_descriptor.events = POLLIN; //there is data to read from the descriptor
    server_poll.socket_id = server;
    server_poll.time = time(NULL); //initialize last event recorded with no of seconds since January 1, 1970
    server_poll.sensor_id = 0; //initialize sensor read from with 0

    //A data structure needs to contain all sockets, so we use the dplist
    list_connections = dpl_insert_at_index(list_connections, ((void*)&server_poll),0,false);

    while (!check_end) { //endless loop stopped after timeout and no sensor connects for a period
        int connection_size = dpl_size(list_connections);
        //Now we loop through every element in the list and decide what to do with them
        //based on their event flags, timeout conditions, and 
        for (int i = 0; i<connection_size; i++) {
            //get the current pollinfo we want to work with
            pollinfo *now = ((pollinfo*)dpl_get_element_at_index(list_connections, i));
            //we call poll to check if a file descriptor is open
            //poll returns the number of elements whose revents have been set to a nonzero value (indicating event or error)
            //poll() blocks until one of the events or revennts of the file descriptors is set
            if (poll(&(now->file_descriptor),1,0) > 0) {
                //check for output parameter revents being set
                //That means there is data to be read
                if (now->file_descriptor.revents == POLLIN) {
                    //Two possibilities
                    //1. It is the first connection that has data to be sent. That means we create a new connection, wait for it and then do something with the data received
                    //2. It is a different connection that wants to send data. We just do something with the data then
                    if (i == 0) {
                        //wait for connection puts socket in a waiting mode
                        //returns when incoming connection request is received
                        //client is the newly created socket identifying remote system that initiated the connection
                        tcp_wait_for_connection(now->socket_id,&client);
                        printf("Incoming client connection");
                        if (tcp_get_sd(client,&(fd_0.fd)) != TCP_NO_ERROR) printf("The listening socet is unreadable\n");
                        pollinfo addition;
                        addition.socket_id = client; //newly created socket to newly created array
                        addition.file_descriptor = fd_0;
                        addition.file_descriptor.events = POLLIN | POLLRDHUP;
                        //insert the new connection at the end of the list
                        list_connections = dpl_insert_at_index(list_connections,((void*)&addition),dpl_size(list_connections),false);
                        printf("A new sensor has been added to the list");
                    } else {
                        //Now let's use the data sent on the port
                        int bytes;
                        sensor_data_t data;
                        //read sensor data
                        bytes = sizeof(data.id);
                        result = tcp_receive(client, ((void*)&data.id), &bytes);
                        now->sensor_id = data.id;
                        //read sensor temperature
                        bytes = sizeof(data.value);
                        result = tcp_receive(client, ((void*)&data.value), &bytes);
                        //read sensor timestamp
                        bytes = sizeof(data.ts);
                        result = tcp_receive(client, ((void*)&data.ts), &bytes);
                        now->time = data.ts;
                        //Print data received as a form of debug
                        printf("The sensor with ID %d gives temperature %f at timestamp %ld", data.id, data.value, data.ts);
                        //Write data received to the sensor_data_recv file
                        fprintf(file, "%d %f %ld\n", data.id, data.value, data.ts);



                    }
                } else {
                    printf("Error! It does not read revents\n");
                    break;
                }

            }
            //Check if the client has closed connection bby itself.
            if (result == TCP_CONNECTION_CLOSED) {

                printf("Client has closed connection\n");
                
                tcp_close(&client);
                list_connections = dpl_remove_at_index(list_connections, i, true);
                now->time = time(NULL);
                number_connections = dpl_size(list_connections);
                printf("Number of sensors remaining %d\n", number_connections);
                break;
            } else {
                printf("Error occured on connection to client");
            }
            if ((now->time + TIMEOUT) < time(NULL)  && i!= 0) {
                //check if there's a timeout
                //no activity monitored on socket one by one, close and remove it from the data structure
                printf("Closing server due to timeout\n");
                tcp_close(&client);
                dpl_remove_at_index(list_connections,i,true);
                now->time = time(NULL);
                number_connections = dpl_size(list_connections);
                printf("Number of sensors remaining: %d\n", number_connections);
                break;
            }

            if (number_connections == 1 && (now->time + TIMEOUT)<time(NULL)) {
                //No sensors are active anymore, now close the connection manager.
                tcp_close(&client);
                check_end = true;
                connmgr_free();
            }

        }
    }

}
//gcc sensor_node.c -o sensor_node -Wall -std=c11 -Werror -DLOOPS=5 -lm -L. -Wl, -rpath=. -ltcpsock
//gcc main.c connmgr.c -o conn_mgr_main -Wall -std=c11 -Werror -lm -L. -Wl,-rpath=. -ltcpsock -ldplist -DTIMEOUT=5