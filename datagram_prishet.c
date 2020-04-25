
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define MAXBUFLEN 100
char PORT[10];
char path_index[100];

int cfg_reader();

void *get_in_addr(struct sockaddr *sa);
int main(int argc, char *argv[])
{

    if (cfg_reader()==-1)
    {
        printf("eror cfg_reader"); 
    }
    int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	if (argc != 3) {
		fprintf(stderr,"usage: talker hostname message\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}

	if ((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	freeaddrinfo(servinfo);

	printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
	close(sockfd);

	return 0;
}



int cfg_reader()
{
    char port[256]; int flag_port=0;
    char path[256]; int flag_path=0;
    char line[256];
    char str[100];
    char standart[]="# coments\nPORT 8080\nPATH www/index.html";
    int linenum=0;
    
    FILE * fp;
    if((fp=fopen("config.txt", "r+"))==NULL)
    {   
        printf ("Cannot open file,create new file \n");
        fp=fopen("config.txt", "a+t");
        fputs(standart, fp); 
        fclose(fp);
        /////рекурсивный вызов самой себя 
        fp=fopen("config.txt", "r+");
    }
    while(fgets(line, 256, fp) != NULL)
    {
        char buff1[256], buff2[256];

        linenum++;
        if(line[0] == '#') continue;

        if(sscanf(line, "%s %s", buff1, buff2) != 2)
        {
                fprintf(stderr, "Syntax error, line %d\n", linenum);
                continue;
        }

        
        if (strcmp(buff1,"PORT")==0)
        {   
            flag_port=1;
            strcpy(port,buff2);
        }
        if (strcmp(buff1,"PATH")==0)
        {   
            flag_path=1;
            strcpy(path,buff2);
        }
    }
    fclose(fp);
    if (flag_port==0|| flag_path==0)
    {
        printf("BAD config file,create new file,old file will be renamed ");
        long int ttime = time (NULL);
        strcpy(str,ctime (&ttime));

        for(int i = 0; str[i] != '\0'; i++)
        {
        if(str[i]== ' '|| str[i]== '\n')
        {
            str[i]='_';
        }
        }
        strcat(str,"config.txt");
        printf("Получили: %s\n",str);
        if (-1 == rename ("config.txt",str))
        printf ("Ошибка переименования файла, удалите configs.txt \n");
        else 
        {
            printf ("Выполнено переименование\n");
            cfg_reader(); return -1;
        }  
        fp=fopen("config.txt", "a+t");
        fputs(standart, fp);
        fclose(fp);
        /////рекурсивный вызов самой себя 
        
    }
    strcpy(PORT,port); // переписываю в глобальные переменные
    strcpy(path_index,path);
    
    printf("\tlen_port-%d\n\tlen_path-%d\n",strlen(port),strlen(path));
    printf("\tport-%s\n\tpath-%s\n",port,path);
}


void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}