#include "log.h"
#include <stdalign.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <network.h>

static s32 log_sock;

#define LOG_SIZE (0x100)

alignas(32) static char log_buf[LOG_SIZE];

#define XSTR(A) STR(A)
#define STR(A) #A

bool log_init(void) {
    #ifdef LOG_IP
	char localip[16] = {0};
	char gateway[16] = {0};
	char netmask[16] = {0};

	printf("Configuring network interface\n");

	// Configure the network interface
	if (if_config(localip, netmask, gateway, true, 20) < 0) {
		printf("Network configuration failed!\n");
        return false;
	}

	log_sock = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(3000);
	server.sin_addr.s_addr = inet_addr(XSTR(LOG_IP));

	if (net_connect(log_sock, (struct sockaddr*)&server, sizeof(server)) != 0) {
		printf("Connection failure\n");
        return false;
	}
	#else
	(void)log_sock;
	(void)log_buf;
    #endif

    return true;
}
void log_term(void) {
    #ifdef LOG_IP
    net_close(log_sock);
    #endif
}

void lprintf(const char* fmt, ...) {
    #ifdef LOG_IP
    va_list args;
    va_start(args, fmt);
    s32 num_chars = vsnprintf(log_buf, LOG_SIZE, fmt, args);
    va_end(args);
    
    net_send(log_sock, log_buf, num_chars, 0);
    #else
	#ifdef PC_PORT
	va_list args;
    va_start(args, fmt);
    s32 num_chars = vprintf(fmt, args);
    va_end(args);
	#endif
	(void)fmt;
	#endif
}