/*
 * switch.c
 * Raspberry Pi GPIO for use with Mausberry switches.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24
#define POUT 23

#define BUFSZ 64

int GPIOExport(int pin)
{
	char buffer[BUFSZ] = {0};
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return -1;
	}

	bytes_written = snprintf(buffer, BUFSZ, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return 0;
}

int GPIOUnexport(int pin)
{
	char buffer[BUFSZ] = {0};
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return -1;
	}

	bytes_written = snprintf(buffer, BUFSZ, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return 0;
}

int GPIODirection(int pin, int dir)
{
	const char s_directions_str[]  = "in\0out";

	char path[BUFSZ] = {0};
	int fd;

	snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return -1;
	}

	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		return -1;
	}

	close(fd);
	return 0;
}

int GPIOWait(int pin)
{
	char path[BUFSZ] = {0};
	char value_str[BUFSZ] = {0};
	int fd;

	//open gpio file descriptor for select
	snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for reading!\n");
		return -1;
	}

	//wait for kernel to notify us of changes
	int done = 0;
	fd_set active_fd_set, read_fd_set;
	FD_ZERO(&active_fd_set);
	FD_SET(pinfd, &active_fd_set);
	struct timeval tv = {0, 500};

	while(!done) {

		read_fd_set = active_fd_set;
		if(select(FD_SETSIZE, &read_fd_set, NULL, NULL, &tv) < 0) {
			perror("An error occurred while waiting for the switch to be flipped");
		}

		int i;
		for(i=0; i < FD_SETSIZE; i++) {
			if(FD_ISSET(i, &read_fd_set)) {
				if(i == //... to be continued
			}
		}
	}

	//read the value
	if (-1 == read(fd, value_str, BUFSZ)) {
		fprintf(stderr, "Failed to read value!\n");
		return -1;
	}

	close(fd);

	return atoi(value_str);
}

int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";

	char path[BUFSZ] = {0};
	int fd;

	snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		return -1;
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write value!\n");
		return -1;
	}

	close(fd);
	return 0;
}

int main(int argc, char *argv[])
{
	// Enable GPIO pins
	if (-1 == GPIOExport(POUT) || -1 == GPIOExport(PIN))
		return 1;

	// Set GPIO directions
	if (-1 == GPIODirection(POUT, OUT) || -1 == GPIODirection(PIN, IN))
		return 2;

	// Initialize switch state
	if (-1 == GPIOWrite(POUT, HIGH))
		return 3;

	// Wait for switch state to change
	GPIO_Wait(PIN);


	// Disable GPIO pins
	if (-1 == GPIOUnexport(POUT) || -1 == GPIOUnexport(PIN))
		return 4;

	// Shutdown
	system("echo 'shutting down!'");

	return 0;
}
