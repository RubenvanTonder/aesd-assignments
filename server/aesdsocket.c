#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 9000
#define DATA_FILE "/var/tmp/aesdsocketdata"
char *packet_buf = NULL;
size_t packet_len = 0;
char read_buf[1024];
ssize_t bytes_received;

int server_socket = -1;
int client_socket = -1;
int file_fd = -1;
volatile sig_atomic_t g_caught_signal = 0;

void signal_handler(int signo)
{
	if (signo == SIGINT || signo == SIGTERM)
	{
		g_caught_signal = 1;
		syslog(LOG_INFO, "Caught signal, exiting");

		// Close sockets and files to unblock system calls if necessary
		if (client_socket != -1)
		{
			close(client_socket);
		}
		if (server_socket != -1)
		{
			close(server_socket);
		}
	}
	// Ignore SIGPIPE to prevent crashes when sending to a closed client socket
	signal(SIGPIPE, SIG_IGN);
}


int main(int argc, char *argv[]) {
    // Open syslog connection
    openlog("aesdsocket", LOG_PID | LOG_CONS, LOG_USER);

    // Reigster signal handlers for SIGINT and SIGTERM
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    if (sigaction(SIGINT, &sa, NULL) !=0 || sigaction(SIGTERM, &sa, NULL) != 0)
    {
	syslog(LOG_ERR, "Error registering signal handler: %s", strerror(errno));
	closelog();
	return -1;
    }

    // TODO: Step 1: Create server socket using socket()
    // Handle failure by returning -1 and logging an error. 
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
	syslog(LOG_ERR, "Error creating socket: %s", strerror(errno));
	closelog();
	return -1;
    }

    // TODO: Step 2: Set socket options (SO_REUSEADDR) to prevent "Address already in use" errors
    int optval = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
	syslog(LOG_ERR, "Error setting setsocketopt: %s", strerror(errno));
	close(server_socket);
	closelog();
	return -1;
    }

    // TODO: Step 3: Bind socket to port 9000 using bind()
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
	syslog(LOG_ERR,"Error binding socket: %s", strerror(errno));
	close(server_socket);
	closelog();
	return -1;
    }

    // TODO: Step 4: Listen for incoming connections using listen()
    if (listen(server_socket, 10) < 0)
    {
	syslog(LOG_ERR, "Error listening on socket: %s", strerror(errno));
	close(server_socket);
	closelog();
	return -1;
    }

    // Check for deamon mode argument
    int daemon_mode = 0;
    if (argc > 1 && strcmp(argv[1], "-d") == 0)
    {
	daemon_mode = 1;
    }

    if (daemon_mode) 
    {
	pid_t pid = fork();
	if (pid < 0)
	{
		syslog(LOG_ERR, "Error forking daemon: %s", strerror(errno));
		close(server_socket);
		closelog();
		return -1;
	}
	if (pid > 0)
	{
		// Parent process exits successfully so shell returns control
		exit(0);
	}

	if (setsid() < 0)
	{
		syslog(LOG_ERR, "Error creating new session: %s", strerror(errno));
		close(server_socket);
		closelog();
		return -1;
	}

	// Change working directory to root
	if (chdir("/") < 0)
	{
		syslog(LOG_ERR, "Error changing directory to root: %s", strerror(errno));
	}

	// Redirect standard file descritpors to /dev/null
	int dev_null = open("/dev/null", O_RDWR);
	if (dev_null < 0 )
	{
		dup2(dev_null, STDIN_FILENO);
		dup2(dev_null, STDOUT_FILENO);
		dup2(dev_null, STDERR_FILENO);
		close(dev_null);
	}
    }

    while (!g_caught_signal) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        // TODO: Step 5: Accept incoming connection using accept()
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            // Handle error or break on signal interruption
	    if (g_caught_signal)
	    {
	    	break;
	    }
	    syslog(LOG_ERR, "Error accepting connection: %s", strerror(errno));
            continue;
        }

        // TODO: Step 6: Extract client IP using inet_ntop and log to syslog:
        char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
	syslog(LOG_INFO, "Accepted connection from %s", client_ip);
        
        // TODO: Step 7: Handle receiving data, writing to file, and sending data back (detailed below)
	file_fd = open(DATA_FILE, O_CREAT | O_RDWR | O_APPEND, 0644);
	if (file_fd < 0)
	{
		syslog(LOG_ERR, "Error opening data file: %s", strerror(errno));
		close(client_socket);
		client_socket = -1;
		continue;
	}

	char buffer[1024];
	ssize_t bytes_received;
	int packet_complete = 0;

	// Read data stream from client
	while (!g_caught_signal)
	{
		bytes_received = recv(client_socket, buffer, sizeof(buffer),0);
		if (bytes_received < 0)
		{
			syslog(LOG_ERR, "Error receiving data: %s", strerror(errno));
			break;
		} else if (bytes_received == 0)
		{
			break; // client disconnected
		}

		if (write(file_fd, buffer, bytes_received) < 0)
		{
			syslog(LOG_ERR, "Error writing buffer to file: %s", strerror(errno));
			break;
		}

		// Check if packet contains newline '\n' indicating completion of a command packet
		
		for (ssize_t i =0 ;i< bytes_received;i++)
		{
			if (buffer[i] == '\n')
			{
				packet_complete = 1;
				break;
			}
		}

		if (packet_complete)
		{
			break;
		}
	}

	// Close and reopne file in read mode to send back full contents to clinet
	close(file_fd);
	file_fd = -1;

        file_fd = open(DATA_FILE, O_RDONLY);

	if (file_fd < 0)
	{
		syslog(LOG_ERR, "Error opening file for reading: %s", strerror(errno));
		return -1;
	}
	else {
            char read_buf[1024];
            ssize_t bytes_read;
            while ((bytes_read = read(file_fd, read_buf, sizeof(read_buf))) > 0) {
                ssize_t bytes_sent = 0;
                while (bytes_sent < bytes_read) {
                    ssize_t sent = send(client_socket, read_buf + bytes_sent, bytes_read - bytes_sent, 0);
                    if (sent < 0) {
                        syslog(LOG_ERR, "Error sending data to client: %s", strerror(errno));
                        break;
                    }
                    bytes_sent += sent;
                }
            }
            close(file_fd);
        }

	close(client_socket);
	client_socket = -1;
	syslog(LOG_INFO, "Closed connection from %s", client_ip);
        
    }
	
    // Cleanup before exit
    if (server_socket != -1) {
        close(server_socket);
    }
    
    // Delete the data file upon exit
    if (remove(DATA_FILE) != 0) {
        // Ignore if file doesn't exist, log otherwise if needed
    }

    closelog();
    return 0;
}
