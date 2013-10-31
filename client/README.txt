Waleed Johnson
Mike Duggan
Group 13
Project 2
10/31/13

README for the Client

List of files:

	The client folder includes a Makefile and the client.c file
	The client.c in compiled into a binary called therm

Summary:

	The program starts by sleeping for 7 seconds to account for any
	errors that may occur from the code to read sensors not being 
	thread safe. The program then opens up the error log and determines
	if there were a sufficient amount of command line arguments. After 
	that the program reads the config file and determines the appropriate
	size to allocate for an array of type Sensor. It then reads the 
	/dev/gotemp file and fills in the appropriate struct and reads the 
	/dev/gotemp2 file and fills in the appropriate struct if necessary.
	Following that the program sends each struct to the server.	