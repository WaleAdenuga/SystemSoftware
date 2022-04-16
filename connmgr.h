#ifndef __CONMGR_H__
#define __CONMGR_H__

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <assert.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include "lib/dplist.h"

#ifndef TIMEOUT
#error TIMEOUT not specified (in seconds)
#endif

typedef struct pollfd polldescr; //list and events you wish to poll for are stored

typedef struct {
    polldescr file_descriptor; //an array used to identify an opened file (or socket) at the kernel level 
    time_t time;
    sensor_id_t sensor_id;
    tcpsock_t *socket_id;
}pollinfo; //information received every time you poll right
//pollinfo is essentially a list consisting of an array, socket you're reading from, sensor you're reading from and the last time you read something

//provide a port number and the connection manager listens on that port
//when the sensor node connects, it writes data to a sensor file which should 
//have the same format as the previous sensor data files
//The connection manager should be using dplist to store all the info on the 
//active sensor nodes
void connmgr_listen(int port_number);

//Clean the connmgr and free all used memory, no new connections are possible when this is called
void connmgr_free();

#endif //__CONMGR_H__   