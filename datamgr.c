#include "datamgr.h"
#include <stdio.h>
#include <stdlib.h>
#include "lib/dplist.h"
#include <inttypes.h>
#include <time.h>
#include <assert.h>
#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>

#define INVALID_ERROR "Invalid"
#define MEMORY_ERROR "Memory null"
#define NO_ERROR "Nothing"

dplist_t* list = NULL;
/* void *element_copy(void* element);
void element_free(void** element);
int element_compare(void*x, void*y);
 */
typedef struct sensor {
    uint16_t sensor_id;
    uint16_t room_id;
    double running_avg;
    time_t last_modified;
    double temperatures[RUN_AVG_LENGTH];
}sensor_t;

void freed(void** element) {
    free(*element);
    *element = NULL;
}

int compare(void *x, void*y) {
    //This section was not my idea
    sensor_t * sensor = (sensor_t *)x;
        if(*(uint16_t *)y==sensor->sensor_id)
        {return 0;}
        else
                return -1;
    
}

void *copy(void* element) {
    sensor_t* copy = malloc(sizeof(sensor_t));
    assert(copy!=NULL);
    sensor_t* sensor = (sensor_t*) element;
    copy->room_id = sensor->room_id;
    copy->running_avg = sensor->running_avg;
    copy->sensor_id = sensor->sensor_id;
    copy->last_modified = sensor->last_modified;
    for (int i = 0; i<RUN_AVG_LENGTH; i++) {
        copy->temperatures[i] = sensor->temperatures[i];
    }
    return (void*) copy;
}

void datamgr_parse_sensor_files(FILE *fp_sensor_map, sbuffer_t **sbuffer) {
    list = dpl_create(copy,freed,compare); //list for all sensors in the map file
    uint16_t room_initial = 0; //for the first part of the file
    uint16_t sensor_initial = 0; //for the second part of the file
    //sensor_t *sensor;
    sensor_data_t *data;
    int sensor_index = 0;
    double temp_initial = 0;
    time_t time_initial = 0;
    double average = 0;
    char *send_buf;

    data = malloc(sizeof(sensor_data_t));
    ERROR_HANDLER(data==NULL, MEMORY_ERROR);
    //fscanf reads formatted input from a stream, one word at a time
    //emphasis on formatted
    //fscanf return int value, number of arguments filled
    while(fscanf(fp_sensor_map, "%hd %hd", &room_initial, &sensor_initial)>0) {
        sensor_t *sensor = NULL;
        sensor = malloc(sizeof(sensor_t));
        sensor->room_id = room_initial;
        sensor->sensor_id = sensor_initial;
        //initialize the temperatures with 0
        for (int i = 0; i<RUN_AVG_LENGTH; i++) {
            sensor->temperatures[i] = 0;
        }
        int where = dpl_size(list);
        list = dpl_insert_at_index(list, sensor, where, false); //making sure you insert at the end of the list
    }
    
    while((*sbuffer)->head != NULL) { //keep checking until sbuffer is empty
        int rc = thread_read(sbuffer, data, data_manager);
        temp_initial = data->value;
        time_initial = data->ts;
        if (rc != SBUFFER_SUCCESS) continue;
        
        sensor_t*sensor = NULL;
        sensor_index = dpl_get_index_of_element(list,(void*) &sensor_initial);
        //log file from data manager
        if (sensor_index == -1) {
            ASPRINTF_ERROR(asprintf(&send_buf, "Received sensor data with invalid sensor node ID %d", data->id));
            write_fifo(send_buf);
            free(send_buf);
            continue; //go to next sensor in list even if one is invalid
        }
        sensor = (sensor_t*) dpl_get_element_at_index(list, sensor_index);

        for(int i=RUN_AVG_LENGTH-1; i>0; i--)
        {
            sensor->temperatures[i]=sensor->temperatures[i-1];
            average += sensor->temperatures[i];
        }
        sensor->temperatures[0]=temp_initial;
        average +=temp_initial;
        sensor->running_avg=average/RUN_AVG_LENGTH;
        sensor->last_modified = time_initial; 

        if (sensor->temperatures[RUN_AVG_LENGTH-1] != 0) { //because we start filling the array from the back end
            if (sensor->running_avg < SET_MIN_TEMP) {
                //break;
                //log info cold sensor from data manager
                ASPRINTF_ERROR(asprintf(&send_buf, "The sensor node with %hu reports it's too cold (running avg temperature = %lf)", sensor->sensor_id,sensor->running_avg));
                write_fifo(send_buf);
                free(send_buf);
            } else if (sensor->running_avg > SET_MAX_TEMP) {
                //break;
                //log info hot sensor from data manager
                ASPRINTF_ERROR(asprintf(&send_buf, "The sensor node with %hu reports it's too hot (running avg temperature = %lf", sensor->sensor_id, sensor->running_avg));
                write_fifo(send_buf);
                free(send_buf);
            }
        }

        
        
    }
    

}

void datamgr_free() {
    dpl_free(&list, true);
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id) {
    sensor_t *sensor = NULL;
    for (int i = 0; i<dpl_size(list); i++) {
        sensor = dpl_get_element_at_index(list, i);
        if (sensor->sensor_id == sensor_id){
            return sensor->room_id;
        }
    }
    ERROR_HANDLER(1, INVALID_ERROR);
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
    sensor_t *sensor = NULL;
    for (int i = 0; i<dpl_size(list); i++) {
        sensor = dpl_get_element_at_index(list, i);
        if (sensor->sensor_id == sensor_id) return sensor->running_avg;
    }
    ERROR_HANDLER(1, INVALID_ERROR);
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
    sensor_t *sensor = NULL;
    for (int i = 0; i<dpl_size(list); i++) {
        sensor = dpl_get_element_at_index(list, i);
        if (sensor->sensor_id == sensor_id) return sensor->last_modified;
    }
    ERROR_HANDLER(1, INVALID_ERROR);
}

int datamgr_get_total_sensors(){
    return dpl_size(list);
}

