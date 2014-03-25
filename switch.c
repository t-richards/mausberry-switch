/*
 * switch.c
 * Raspberry Pi GPIO for use with Mausberry switches.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <poll.h>
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

int GPIOInterrupt(int pin)
{
	char path[BUFSZ] = {0};
	const char when_to_return[] = "both";
	int fd;

	snprintf(path, BUFSZ, "/sys/class/gpio/gpio%d/edge", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio edge\n");
		return -1;
	}

	if (-1 == write(fd, when_to_return, 4)) {
		fprintf(stderr, "Failed to configure gpio as interrupt source\n");
		return -1;
	}

	close(fd);
	return 0;
}

int GPIOWait(int pin)
{
	int value = -1;
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
	struct pollfd pfds[1];
	pfds[0].fd = fd;
	pfds[0].events = POLLPRI | POLLERR;

	for (;;) {

		int rc = poll(pfds, 1, -1);
		if(rc < 0) {
			int errsv = errno;
			perror("An error occurred while waiting for the switch");
			if(errsv != EAGAIN && errsv != EINTR && errsv != EINVAL)
				return -1;
		}

		if(pfds[0].revents & POLLPRI) {
			lseek(fd, 0, SEEK_SET);
			//read the value
			if (-1 == read(fd, value_str, BUFSZ)) {
				fprintf(stderr, "Failed to read value!\n");
				return -1;
			}
			value = atoi(value_str);
			if (value == LOW) {
				close(fd);
				return value;
			}
		}
	}
}

int GPIOWrite(int pin, int value)
{
	const char s_values_str[] = "01";

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
	pid_t pid, sid;

	//fork parent process
	pid = fork();
	if(pid < 0) {
		perror("An error occurred while attempting to fork()");
		exit(EXIT_FAILURE);
	}

	if(pid > 0) {
		exit(EXIT_SUCCESS);
	}

	//change file mode mask
	umask(0);

	//create unique session id
	sid = setsid();
	if(sid < 0) {
		perror("An error occurred while creating the session");
		exit(EXIT_FAILURE);
	}

	//change working directory
	if((chdir("/")) < 0) {
		perror("An error occurred while changing the working directory");
		exit(EXIT_FAILURE);
	}

	//close standard file descriptors
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// Enable GPIO pins
	if (-1 == GPIOExport(POUT) || -1 == GPIOExport(PIN))
		exit(EXIT_FAILURE);

	// Set GPIO directions
	if (-1 == GPIODirection(POUT, OUT) || -1 == GPIODirection(PIN, IN))
		exit(EXIT_FAILURE);

	// Initialize switch state
	if (-1 == GPIOWrite(POUT, HIGH))
		exit(EXIT_FAILURE);

	if (-1 == GPIOInterrupt(POUT))
		exit(EXIT_FAILURE);

	// Wait for switch state to change
	int result = GPIOWait(POUT);
	printf("Received a %d from gpiowait!\n", result);

	// Disable GPIO pins
	if (-1 == GPIOUnexport(POUT) || -1 == GPIOUnexport(PIN))
		exit(EXIT_FAILURE);

	// Shutdown
	system("echo 'shutting down!'");

	return 0;
}
