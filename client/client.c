//Waleed Johnson
//Mike Duggan
//Computer Networks
//Project 2

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include "../shared/sensor.h"
#include <errno.h>
#include <fcntl.h>
#include <time.h>

//NOTE:
//Anything labeled "From newgo.c"
//was taken from the file given to us
//written by Jeff Sadowski

//All other references cited inline

///From newgo.c
struct packet {
	unsigned char measurements;
	unsigned char counter;
	int16_t measurement0;
	int16_t measurement1;
	int16_t measurement2; 
	};

/* Function to convert Celsius to Fahrenheit*/
double CtoF(double C){return (C*9.0/5.0)+32;}
///

void getTimeStamp(char *buffer);
void logMessage(char *message, int fileInt);

int main(int argc, char *argv[])
{

	//open error log
	char *errorLog = "/var/log/therm/error/group13_error_log";
	char errorLogChar = 'a';
	FILE * elPtr = fopen(errorLog, &errorLogChar); 	

	char stamp1[20];	
	getTimeStamp(stamp1);
	char finalMessage[100];

	if(argc != 2)
	{
		if( elPtr == NULL)
		{
			printf("Error opening Error Log\n");
			exit(1);
		}
			
		int elPtrInt = fileno(elPtr);	

		char *message = "Invalid number of arguments";
		sprintf(finalMessage, "%s %s\n", message, stamp1);
		logMessage(finalMessage, elPtrInt);
	}

	if( elPtr == NULL)
	{
		printf("Error opening Error Log\n");
		exit(1);
	}
		
	int elPtrInt = fileno(elPtr);	

	char configLocation [] = "/etc/t_client/client.conf";
	char mode = 'r';
	FILE * fileDescriptor = fopen(configLocation, &mode); 
	int fileInt = fileno(fileDescriptor);
	char buffer[100]; 
	int endOfFile = 0;	
	int numberOfSensors = 0;
	char delimiters [] = "\n ";	
	int bytesRead = 0;
	char hostName[32];

	////From newgo.c
	char *fileName="/dev/gotemp";
	char *fileName2="/dev/gotemp2";
	struct packet temp, temp2;
	double conversion=0.0078125;
	int fd, fd2;
	///

	//Read config file
	while(endOfFile == 0)
	{ 
		if((bytesRead = read(fileInt, buffer, 100)) < 0)
		{
			char *message = "Error reading file contents";
			sprintf(finalMessage, "%s %s\n", message, stamp1);
			logMessage(finalMessage, elPtrInt);
		}

		if (bytesRead == 0)
		{
			endOfFile = 1;
		}
	}

	numberOfSensors = atoi(strtok(buffer, delimiters));
	Sensor sensorArray[numberOfSensors];

	if(numberOfSensors == 0)
	{
		char *message = "No sensors to read";
		sprintf(finalMessage, "%s %s\n", message, stamp1);
		logMessage(finalMessage, elPtrInt);
	}	
	else
	{
		///From newgo.c
		if((fd=open(fileName,O_RDONLY))==-1)
		{
			char *message = "Could not open /dev/gotemp";
			sprintf(finalMessage, "%s %s\n", message, stamp1);
			logMessage(finalMessage, elPtrInt);
		}	
		if(read(fd,&temp,sizeof(temp))!=8)
		{
			char *message = "Could not read /dev/gotemp";
			sprintf(finalMessage, "%s %s\n", message, stamp1);
			logMessage(finalMessage, elPtrInt);
		}			
		///		

		//Initialize Sensor struct
		gethostname(hostName, 32);
		strcpy(sensorArray[0].hostName, hostName);
		sensorArray[0].numberSensors = numberOfSensors;
		sensorArray[0].currentSensor = 0;
		sensorArray[0].data = CtoF(((double)temp.measurement0)*conversion);
		sensorArray[0].lowvalue = atoi(strtok(NULL, delimiters));
		sensorArray[0].highvalue = atoi(strtok(NULL, delimiters));
		sensorArray[0].actionrequested = 0;

		strcpy(sensorArray[0].timestamp, stamp1);
		
		if(numberOfSensors == 2)
		{
			///From newgo.c
			if((fd2=open(fileName2,O_RDONLY))==-1)
			{
				char *message = "Could not open /dev/gotemp2";
				sprintf(finalMessage, "%s %s\n", message, stamp1);
				logMessage(finalMessage, elPtrInt);
			}
			if(read(fd2,&temp2,sizeof(temp))!=8)
			{
				char *message = "Could not read /dev/gotemp2";
				sprintf(finalMessage, "%s %s\n", message, stamp1);
				logMessage(finalMessage, elPtrInt);
			}
			////

			//Initialize second Sensor struct
			strcpy(sensorArray[1].hostName, hostName);
			sensorArray[1].numberSensors = numberOfSensors;
			sensorArray[1].currentSensor = 1;
			sensorArray[1].data = CtoF(((double)temp2.measurement0)*conversion);
			sensorArray[1].lowvalue = atoi(strtok(NULL, delimiters));
			sensorArray[1].highvalue = atoi(strtok(NULL, delimiters));
			sensorArray[1].actionrequested = 0;	
			getTimeStamp(stamp1);
			strcpy(sensorArray[1].timestamp, stamp1);
		}
	}

	//Send data:
	int s;
	struct sockaddr_in sin;
	if ( ( s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		char *message = "Error creating socket";
		sprintf(finalMessage, "%s %s\n", message, stamp1);
		logMessage(finalMessage, elPtrInt);
	}

	//Initialize sockaddr_in
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT_NO);
	sin.sin_addr.s_addr = inet_addr(argv[1]);

	//Connect()
	if(connect(s, (struct sockaddr *) &sin, sizeof(struct sockaddr)) < 0)
	{
		char *message = "Error connecting to socket";
		sprintf(finalMessage, "%s %s\n", message, stamp1);
		logMessage(finalMessage, elPtrInt);
	}	

	//Send data
	int z = 0;
	for( z = 0; z < numberOfSensors; z++)
	{
		if(write(s, (Sensor *) &sensorArray[z], sizeof(struct Sensor)) < 0)
		{
			char *message = "Error sending data";	
			sprintf(finalMessage, "%s %s\n", message, stamp1);
			logMessage(finalMessage, elPtrInt);
		} 	
	}

	//Close socket
	if( close(s) == -1)
	{
		char *message = "Error closing socket";
		sprintf(finalMessage, "%s %s\n", message, stamp1);
		logMessage(finalMessage, elPtrInt);
	}
	
	fclose(elPtr);

	return 0;
}

void getTimeStamp(char *buffer)
{
	//Code taken from:
	//http://www.cplusplus.com/reference/ctime/localtime/
	//http://www.gnu.org/software/libc/manual/html_node/Time-Functions-Example.html
	
	struct tm * stamp;
	time_t rawtime;
	time(&rawtime);
	stamp = localtime(&rawtime);
	strftime(buffer, 20, "%Y %m %d %H %M", stamp);
}

void logMessage(char *message, int fileInt)
{
	write(fileInt, (char *) message, strlen(message));
	fputs(message, stdout);
	fputs("\n", stdout);
	exit(1);
}
