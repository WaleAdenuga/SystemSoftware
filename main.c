#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "datamgr.h"
#include "lib/dplist.h"

int main(void)
{
        FILE * fp= fopen("room_sensor.map","r");
        FILE * fp2=  fopen("sensor_data","rb");
        datamgr_parse_sensor_files(fp,fp2);
        fclose(fp);
        fclose(fp2);
        datamgr_free();
}
