//header for shared sensor struct
//Mike Duggan and Waleed Johnson
//

#ifndef SENSOR_H
#define SENSOR_H

//constants
#define PORT_NO 9774
#define PATH "/var/log/therm/temp_logs"
#define GROUP_NO 13

typedef struct Sensor {
	char hostName[32];//name of the host
	int numberSensors;//total number of sensors on host
	int currentSensor; //sensor number
	double data;//sensor data from sensor
	double lowvalue;//low value from sensor config
	double highvalue;//high value from sensor config
	char timestamp[32];//time stamp string
	int actionrequested; //action requested

} Sensor;

#endif
