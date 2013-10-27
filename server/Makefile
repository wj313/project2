all: thermd


thermd: server.c ../shared/sensor.h
	gcc server.c ../shared/sensor.h -o thermd -Wall

clean:
	rm thermd