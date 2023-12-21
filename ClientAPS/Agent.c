#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
//---------------------Net def's
#define SIZE 1024
//---------------------Monitor def's
#define MAX_EVENTS 1024 //numb of events to process at one go
#define LEN_NAME 1024 
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS*(EVENT_SIZE + LEN_NAME)) 

void get_event(int fd) {
    char buffer[BUF_LEN];
    int length, i = 0;

    length = read(fd, buffer, BUF_LEN);
    if (length < 0) {
        error("read");
    }

    while(i < length) {
        struct inotify_event *event = (struct inotify_event*) &buffer[i];
        if(event->len) {
            if(event->mask & IN_CREATE) {
                if (event->mask & IN_ISDIR) {
                    printf("The directory %s was CREATED.\n", event->name);
                } else {
                    printf("The file %s was CREATED WD %D\n", event->name, event->wd);
                    }
                }
            if(event->mask & IN_MODIFY) {
                if(event->mask & IN_ISDIR) {
                    printf("The directory %s was MODIFIED.\n", event->name);
                } else {
                    printf("The file %S was MODIFIED with WD %d\n", event->name, event->wd);
                    }
                }
            if(event->mask & IN_DELETE) {
                if(event->mask & IN_DELETE) {
                    printf("The directory %s was DELETED.\n", event->name);
                } else {
                    printf("The file %S was DELETED with WD %d\n", event->name, event->wd);
                    }
                }
        i += EVENT_SIZE + event->len;
        }
    }
}

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void send_log_auth(FILE *fp, int sockfd) {
    int n;
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp) != NULL) {
        if(send(sockfd, data, sizeof(data), 0) == -1) {
            perror("[-] ERROR in sending log auth");
            exit(1);
        }
        bzero(data, SIZE);
    }
}

int main(int argc, char *argv[]) {

    FILE *fp;

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server; //?

    char buffer[256];

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("[-] ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"[-] ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("[-] ERROR connecting");

    printf("[?] Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) 
         error("[-] ERROR writing to socket");

    bzero(buffer,256);
    n = read(sockfd, buffer, 255);
    if (n < 0) 
         error("[-] ERROR reading from socket");
    
    printf("%s\n", buffer);

    fp = fopen("/var/log/auth.log", "r");
    if (fp == NULL) {
        perror("[-] ERROR in reading file.");
        exit(1);
    }

    send_log_auth(fp, sockfd);
    printf("[+] File data sent successfully.\n");
    
    printf("[+]Closing the connection.\n");
    close(sockfd);
}