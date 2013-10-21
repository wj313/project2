//header for shared sensor struct
//Mike Duggan and Waleed Johnson
//

#ifndef SENSOR_H
#define SENSOR_H

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
