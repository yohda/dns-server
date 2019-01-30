#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<sys/stat.h>
#include<fcntl.h>
#include"hash.h"

#define BUF_SIZE 1024
#define PORT 7563

typedef struct sockaddr_in SAI;
typedef struct sockaddr SA;
typedef struct hostent HS;

HS* gethost(char* t_s)
{
	SAI addr;
	if(t_s[0] < 'A'){
		memset((void*)&addr, 0x00, sizeof(addr));
		addr.sin_addr.s_addr = inet_addr(t_s);

		return gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);
	}else{
		return gethostbyname(t_s);
	}
}

void serialize(char* buf, HS* t_h, FILE* fp, node* table, slot *a)
{
	int i;
	char s[BUF_SIZE];
	data info;

	memset(s, 0x00, sizeof(s));
	strcpy(s, buf); strcat(s, "=");
	memset(buf, 0x00, sizeof(buf));
	
	t_h->h_name != NULL ?
		strcpy(buf, t_h->h_name) : strcpy(buf, "none");
	strcat(buf, ";");

	if(t_h->h_aliases[0] != NULL){
		for(i = 0 ; t_h->h_aliases[i] ; ++i){
			strcat(buf, t_h->h_aliases[i]);
			strcat(buf, "$");
		}
		buf[strlen(buf)-1] = ';';
	}else{
		strcat(buf, "none;");
	}

	if(t_h->h_addr_list[0] != NULL){
		for(i = 0 ; t_h->h_addr_list[i] ; ++i){
			strcat(buf, inet_ntoa(*(struct in_addr*)t_h->h_addr_list[i]));
			strcat(buf, "$");
		}
		buf[strlen(buf)-1] = '\0';
	}else{
		strcat(buf, "none;");
	}
	
	strcat(s, buf); strcat(s, ";0\n");
	write(fileno(fp), s, strlen(s));
	strcpy(info.key, strtok(s, "="));
	strcpy(info.record, strtok(NULL, "="));
	info.hit = 0;
	hash_insert(info, a);
	rb_insert(info, table);
}

node* gethostbytable(char* t_s, node* table, slot a[]){ 
	slot *t = hash_search(t_s, a);
	node *n = (node*)malloc(sizeof(node)); 
	if(!t)
		 return rb_search(t_s, table);
	else{
		strcpy(n->val.key, t->key);
		strcpy(n->val.record, t->record);
		n->val.hit = t->hit;
	}

	return n;
}

void init_struct_by_file(FILE* fp, node* table, slot* cache){
	char buf[BUF_SIZE];
	data info;
	memset(buf, 0, sizeof(buf));
					
	while(fgets(buf, sizeof(buf), fp)){
		buf[strlen(buf)-1] = 0;
		strcpy(info.key, strtok(buf, "="));
		strcpy(info.record, strtok(NULL, "="));
		rb_insert(info, table);
		hash_insert(info, cache);
	}
}

void update_hit_cnt(char* _s, node* _table, slot cache[]){
	node* n = rb_search(_s, _table);
	slot* t = hash_search(_s, cache);

	(n->val.hit)++;
	(t->hit)++;
}

int main(int argc, char** argv)
{
	char buf[BUF_SIZE];
	SAI server_addr, client_addr;
	int sockfd, fd, client_addr_len = sizeof(server_addr);
	HS* host;
	node *dns_table, *n;
	FILE* fp;
	slot cache[HASH_SIZE];

	if((fp = fopen("local.dns.txt", "a+")) == NULL)
	{
		perror("Error ");
		return 1;
	}

	init_tree(&dns_table);	
	init_hash(cache);
	init_struct_by_file(fp, dns_table, cache);
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Error ");
		return 1;
	}

	memset((void*)&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
		
	if(bind(sockfd, (SA*)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("Error ");
		return 1;
	}

	while(1)
	{
		recvfrom(sockfd, buf, sizeof(buf), 0, (SA*)&client_addr, &client_addr_len);

		n = gethostbytable(buf, dns_table, cache);
		if(!n){
			host = gethost(buf);
			
			if(!host){
				sendto(sockfd, "fail\n", 5, 0, (SA*)&client_addr, client_addr_len);  
				continue;
			}
			serialize(buf, host, fp, dns_table, cache);
		}else{
			update_hit_cnt(buf, dns_table, cache);	
			memset(buf, 0, sizeof(buf));
			strcpy(buf, n->val.record);
			buf[strlen(buf)-1] = 0;
		}	
			
		sendto(sockfd, buf, sizeof(buf), 0, (SA*)&client_addr, client_addr_len);
	}
	
	fclose(fp);
	close(sockfd);
	
	return 0;
}
