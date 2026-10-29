#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

int main(int argc, char*argv[])
{
	// initialize syslog using the LOG_USER
	openlog("writer", LOG_PID, LOG_USER);
	
	// validate arguments, expecting program name, filename and string
	if (argc !=3)
	{
		syslog(LOG_ERR, "Error: Invalid number of arguments. Expected 2, got %d", argc-1);
		fprintf(stderr, "Usage: %s <filename> <string>\n",argv[0]);
		closelog();
		exit(1);
	}

	const char *filename = argv[1];
	const char *writer_string = argv[2];

	// open the file using system call open
	// O_WRONLY: open the writing file
	// O_CREAT: create the file if it does not exit
	// O_TRUNC: overwrite if the file alread exists
	// 0644: Permissions
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		syslog(LOG_ERR, "Error opening file %s for writing", filename);
		perror("Error opening file");
		closelog();
		exit(1);
	}

	// log the write operation using LOG_DEBUG level
	syslog(LOG_DEBUG, "Writing %s to %s", writer_string, filename);
	
	// write the string contents to the file using system call write
	ssize_t bytes_written = write(fd, writer_string, strlen(writer_string));
	if (bytes_written == -1)
	{
		syslog(LOG_ERR, "Error writing string to file %s", filename);
		perror("Error writing to file");
		close(fd);
		closelog();
		exit(1);
	}

	// close the file descriptor
	if (close(fd) == -1)
	{
		syslog(LOG_ERR, "Error closing the file %s", filename);
		perror("Error closing the file");
		closelog();
		exit(1);
	}

	// clean up syslog connection
	closelog();
	return 0;

}
