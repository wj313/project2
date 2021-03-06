/* server.c
 *
 * contains implementatian of server for project2
 *
 * - Mike Duggan and Waleed Johnson
 */

#include "../shared/sensor.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define LOG "/var/log/therm/temp_logs/"

//function definitions
void errorPrint(char * message);
void logMessage(Sensor sensors[], int numberSensors); //takes sensor data and adds to log file

void * clientThread(void * arg) {
    int client_socket = *(int *) arg;
    Sensor sensor;
    memset(&sensor, 0, sizeof sensor);
    if(read(client_socket, (void *) &sensor, sizeof sensor) < sizeof sensor) {
        errorPrint("error - receiving sensor information");
	printf("error - receiving sensor information\n");
    }

    Sensor * sensors;
    int numberSensors = sensor.numberSensors;
    sensors = malloc(numberSensors * sizeof(Sensor));
    sensors[0] = sensor;

    if (numberSensors > 1)
    {
        int i;
        for(i = 1; i < numberSensors; i++)
            if(read(client_socket, (void *) &sensors[i], sizeof(Sensor)) < sizeof(Sensor)) {
                errorPrint("error - receiving sensor information");
            }
    }
    if(sensors[0].actionrequested == 0) {
        logMessage(sensors, numberSensors);
    }
    close(client_socket);
    pthread_exit(NULL);
}


int main(int argc, char * argv[]) {
    struct sockaddr_in server;

    int s, option, *newsock; //create variable for socket, socket options
    
    //specify server address info and set port
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_NO);
    server.sin_addr.s_addr = INADDR_ANY;
    
    //setup socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        errorPrint("error - creating socket");
    }
    option = 1;
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &option, sizeof option) < 0) {
        errorPrint("error - setting option");
    }
    
    if(bind(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
        errorPrint("error - binding socket");
    }
    
    if (listen(s, 10) < 0) {
        errorPrint("error - listening on socket");
    }

    while(1) {
	pthread_t child;
  	struct sockaddr client;
        memset(&client, 0, sizeof(client));
        int client_socket = 0;
        socklen_t client_socket_length = sizeof(client);
        client_socket = accept(s, &client, &client_socket_length);
        if (client_socket < 0) {
            errorPrint("error - accepting client connection");
        }
	newsock = malloc(1);
	*newsock = client_socket;
        pthread_create(&child,NULL, clientThread, (void*)newsock);
     }

	return 0;
}

void errorPrint(char * message) {
  //print error and errno and exit with error
  perror(message);
  exit(EXIT_FAILURE);
}

void logMessage(Sensor sensors[], int numberSensors) {
    char filename[512];
    char timestamp[32];
    memcpy(timestamp, sensors[0].timestamp,32);
    char *year, *month;
    year = strtok(timestamp, " ");
    month = strtok(NULL, " ");

    sprintf(filename, "%sg%d_%s_%s_%s",LOG,GROUP_NO,year,month,sensors[0].hostName);
    char data[128];
    sprintf(data, "%s %.3f", sensors[0].timestamp, sensors[0].data);
    int i;
    for (i = 1; i < numberSensors; i++)
        sprintf(data, "%s %.3f", data, sensors[i].data);
    //printf("File path: %s\n", filename);
    //printf("Get Data: %s\n", data);
    FILE * logfile = fopen(filename, "a");
    if(logfile == NULL)
       errorPrint("error - cannot open logfile");
    fprintf(logfile, "%s\n",data);
    fclose(logfile);
}
