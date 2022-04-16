#include "sensor_db.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sqlite3.h>
#include <sys/types.h>
#include <sys/stat.h>

/*the first parameter of the callback function is data provided in the 4th argument of
sqlite3_exec(); it is often not used. 
The second parameter is the number of columns in the result. 
The third parameter is an array of strings representing fields in the row. 
The last parameter is array of strings representing column names. */
typedef int (*callback_t)(void *, int, char **, char **);

/**
 * Make a connection to the database server
 * Create (open) a database with name DB_NAME having 1 table named TABLE_NAME  
 */
DBCONN *init_connection(char clear_up_flag) {
    sqlite3 *db;
    char *err_msg = 0;
    char *send_buf;
    char sql[300];

    int rc = sqlite3_open(TO_STRING(DB_NAME), &db); //open database with name supplied by the user

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        //log event for connection failed
        ASPRINTF_ERROR(asprintf(&send_buf, "Unable to connect to SQL server\n"));
        write_fifo(send_buf);
        free(send_buf);

        return NULL; //return NULL if an error occurs
    } else {
        //log event for connection passed
        ASPRINTF_ERROR(asprintf(&send_buf, "Connection to SQL server established\n"));
        write_fifo(send_buf);
        free(send_buf);
    }
    

    //We delete the table if it already exists and create a new one with new fields
    //that is of course after connecting to db_name.db, if the clear up flag is 1
    if (clear_up_flag == 1) { //clear the existing data if clear flag is set to 1
        snprintf(sql,300, "DROP TABLE IF EXISTS %s;" //the table name and database name are inserted into the preprocessor, 300 is the maximum size for chars
                    "CREATE TABLE %s(id AUTOINCREMENT, sensor_id INT, sensor_value DECIMAL(4,2), timestamp TIMESTAMP;",
                    TO_STRING(DB_NAME),TO_STRING(TABLE_NAME)); //basically add the strings /SQL instructions to the char sql, that one is then sent at run time
        rc = sqlite3_exec(db,sql,0,0,&err_msg); //start sqlite3 database
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR; %s\n", err_msg);
            sqlite3_free(err_msg);
            disconnect(db); //disconnect from the database server if there is an error
            return NULL;
        }
        //what if clear up flag is not 1? Do you still need to create new table?
    } else {
        snprintf(sql, 300, "CREATE TABLE IF NOT EXISTS %s(id AUTOINCREMENET, sensor_id INT, sensor_value DECIMAL(4,2), timestamp TIMESTAMP;",
        TO_STRING(TABLE_NAME));
        rc = sqlite3_exec(db,sql,0,0,&err_msg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL ERROR; %s\n", err_msg);
            sqlite3_free(err_msg);
            disconnect(db);
            return NULL;
        }
    }
    

    //log event about creation of new table, sends in either case of the clear up flag
    ASPRINTF_ERROR(asprintf(&send_buf, "New table %s created\n", TO_STRING(TABLE_NAME)));
    write_fifo(send_buf);
    free(send_buf);

    return db;
}

/**
 * Disconnect from the database server
 */
void disconnect(DBCONN *conn) {
    char *send_buf;
    //log event for database disconnect
    ASPRINTF_ERROR(asprintf(&send_buf, "Connection to SQL server lost\n"));
    write_fifo(send_buf);
    free(send_buf);

    sqlite3_close(conn);
}

/**
 * Write an INSERT query to insert a single sensor measurement
 * \return zero for success, and non-zero if an error occurs
 */
int insert_sensor(DBCONN *conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    char sql[200];
    snprintf(sql,200, "INSERT INTO %s VALUES (%d %f %ld)",TO_STRING(TABLE_NAME),id,value,ts);
    char *err_msg;
    int rc = sqlite3_exec(conn,sql,0,0,&err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL ERROR: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1; //non-zero if error occurs
    }
    return 0;
}

//Insert sensor measurements from shared data structure
int insert_sensor_from_buffer(DBCONN *conn, sbuffer_t **buffer) {
    sensor_data_t *data;
    data = malloc(sizeof(sensor_data_t));
    while ((*buffer)->head != NULL) { //check that buffer structure is not empty
        int rc = thread_read(buffer, data, database); //read with database as manager
        if (rc == SBUFFER_SUCCESS) {
            insert_sensor(conn, data->id,data->value, data->ts);
        }
    }
    free(data);
    return 0;
}

//Insert all sensor measurements available in file
int insert_sensor_from_file (DBCONN *conn, FILE *sensor_data) {
    uint16_t sensor_id;
    double temperature;
    time_t time;

    //read the id, temp and time values in the file
    //then add it to the database
    while (fread(&sensor_id,sizeof(sensor_id),1,sensor_data) > 0) {
        fread(&temperature,sizeof(temperature),1,sensor_data);
        fread(&time, sizeof(time), 1 , sensor_data);
        int rc = insert_sensor(conn,sensor_id,temperature,time);
        //rc returns 0 if you're able to connect to the database and -1 if you
        //can't establish a database connection
        if (rc != 0) return -1; //return -1 if an error occurs
    }
    return 0;

}

int find_sensor_all (DBCONN *conn, callback_t f) {
    char sql[200];
    snprintf(sql,200, "SELECT * FROM %s",TO_STRING(TABLE_NAME));
    char *err_msg;
    int rc = sqlite3_exec(conn,sql,f,0,&err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error reading files %s", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    return 0;
}

int find_sensor_by_value(DBCONN *conn, sensor_value_t value, callback_t f) {
    char sql[100];
    snprintf(sql,100, "SELECT * FROM %s WHERE sensor_value = %f", TO_STRING(TABLE_NAME),value);
    char *err_msg;
    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error finding sensor with value %f, message %s", value, err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    return 0;
}

int find_sensor_exceed_value (DBCONN *conn, sensor_value_t value, callback_t f) {
    char sql[100];
    snprintf(sql, 100, "SELECT * FROM %s WHERE sensor_value > %f", TO_STRING(TABLE_NAME), value);
    char *err_msg;
    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error finding sensor with value greater than %f, with error message being %s", value, err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    return 0;

}

int find_sensor_by_timestamp (DBCONN *conn, sensor_ts_t ts, callback_t f) {
    char sql[100];
    snprintf(sql, 100, "SELECT * FROM %s WHERE timestamp = %ld", TO_STRING(TABLE_NAME), ts);
    char *err_msg;
    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error reading timestamp with value %ld with message %s", ts, err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    return 0;
}

int find_sensor_after_timestamp(DBCONN *conn, sensor_ts_t ts, callback_t f) {
    char sql[100];
    snprintf(sql, 100, "SELECT * FROM %s WHERE timestamp > %ld", TO_STRING(TABLE_NAME), ts);
    char *err_msg;
    int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error reading timestamp greater than value %ld with error message %s", ts, err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    return 0;
}

