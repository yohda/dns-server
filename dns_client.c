#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>

#define BUF_SIZE 1024
typedef struct sockaddr_in SAI;
typedef struct sockaddr SA;
typedef struct _output{
	char* hostname;
	char* aliases[100];
	char* ips[100];
}output;

output* split(char* t_buf)
{
	output* out = (output*)malloc(sizeof(output));
	int i = 0;
	char *dns_info[3], *p;

	dns_info[i] = strtok(t_buf, ";");
	for(i = 1 ; i < 3 ; ++i)
		dns_info[i] = strtok(NULL, ";");

	out->hostname = dns_info[0];
	p = strtok(dns_info[1], "$"); 
	for(i = 0 ; p != NULL ; i++){
		out->aliases[i] = p;
		p = strtok(NULL, "$");
	}
	out->aliases[i] = NULL;	
	
	p = strtok(dns_info[2], "$"); 
	for(i = 0 ; p != NULL ; i++){
		out->ips[i] = p;
		p = strtok(NULL, "$");
	}
	out->ips[i] = NULL;	

	return out;
}

void print(output* t_out)
{
	int i;
	printf("- canonical name = %s\n\n", t_out->hostname);
	
	if(strcmp(t_out->aliases[0], "none\0")){
		for(i = 0 ; t_out->aliases[i] != NULL; i++)
			printf("%d. %s\n", i+1, t_out->aliases[i]);
	}
	
	for(i = 0 ; t_out->ips[i] != NULL; i++){
		printf("Name: %s\n", t_out->hostname); 	
		printf("Address: %s\n", t_out->ips[i]);
	}
}

int main(int argc, char** argv)
{
	char msg[BUF_SIZE];
	SAI server_addr;
	int sockfd, server_addr_len = sizeof(server_addr);
	output *out;

	setbuf(stdout, NULL);

	if(argc != 3)
	{
		fprintf(stderr, "Usage: %s ip proti\n", argv[0]);
		return 1;
	}

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Error ");
		return 1;
	}

	memset((void*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));

	while(1)
	{
		printf(">");

		memset(msg, 0, sizeof(msg));
		fgets(msg, BUF_SIZE-1, stdin); 
		msg[strlen(msg)-1] = '\0';

		if(!strncmp(msg, "quit\n", 5))
			break;

		sendto(sockfd, msg, sizeof(msg), 0, (SA*)&server_addr, server_addr_len);
		
		memset(msg, 0x00, sizeof(msg));
		recvfrom(sockfd, msg, sizeof(msg), 0, (SA*)&server_addr, &server_addr_len);
		
		if(!strncmp(msg, "fail\n", 5))
		{
			fprintf(stderr, "Not founded!!\n");
			continue;
		}

		print(split(msg));		
	}
	
	return 0;
}
