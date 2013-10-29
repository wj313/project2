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

#define LOG "/var/log/therm/temp_logs/"

//function definitions
void errorPrint(char * message);

void logMessage(Sensor sensors[], int numberSensors) {
	char filename[512];
    char timestamp[32];
    memcpy(timestamp, sensors[0].timestamp,32);
    char *year, *month;
    year = strtok(timestamp, " ");
    month = strtok(NULL, " ");

    sprintf(filename, "%sg%d_%s_%s_%s",LOG,GROUP_NO,year,month,sensors[0].hostName);
    char data[128];
    int size = sprintf(data, "%s %f", sensors[0].timestamp, sensors[0].data);
    int i;
    for (i = 1; i < numberSensors; i++)
        size += sprintf(data+size, "%f", sensors[i].data);
    printf("Data taken in: %s\n", data);
}

int main(int argc, char * argv[]) {
    struct sockaddr_in server;
    int s, option; //create variable for socket, socket options
    
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
    
    if (listen(s, 2) < 0) {
        errorPrint("error - listening on socket");
    }

    while(1) {
  		struct sockaddr client;
        memset(&client, 0, sizeof(client));
        int client_socket = 0;
        socklen_t client_socket_length = sizeof(client);
        
        client_socket = accept(s, &client, &client_socket_length);
        if (client_socket < 0) {
            errorPrint("error - accepting client connection");
        }

        Sensor sensor;
        memset(&sensor, 0, sizeof sensor);
        if(read(client_socket, (void *) &sensor, sizeof sensor) < sizeof sensor) {
        	errorPrint("error - receiving sensor information");
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

        if(sensor.actionrequested == 0) {
        	logMessage(sensors, numberSensors);
        }

     }

	return 0;
}

void errorPrint(char * message) {
  //print error and errno and exit with error
  perror(message);
  exit(EXIT_FAILURE);
}
