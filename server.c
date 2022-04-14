#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#define MAX_EVENTS 10

int main(){
	int k;
	int i, sockfd, val, flag,clientfd, readval,writeval;
	int epollfd;
	int conn_sock;	//プロセス間通信で使うファイルディスクリプタ
	struct epoll_event ev;
	struct epoll_event events[MAX_EVENTS];
	struct sockaddr_in serv_addr, client_addr;
	char buff[100];
	epollfd=epoll_create(MAX_EVENTS);
	if(epollfd==-1){
		perror("ERROR EPOLL_CREATE\n");
		return 0;
	}
	//ソケットセットアップ
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("sockfd server : %d\n",sockfd);
	if(sockfd == -1){
		perror("ERROR SOCKET\n");
		return 0;
	}
	memset(&serv_addr,0,sizeof(struct sockaddr_in));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(50000); 

	val = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)); //バインド
	if(val == -1){
		perror("ERROR BIND\n");
		return 0;
	}
	val = listen(sockfd, 5); //ソケットを接続待ちとする
	if(val == -1){
		perror("ERROR LISTEN\n");
		return 0;
	}
	

	memset(&ev, 0, sizeof ev); //epoll_eventの初期化
	ev.events = EPOLLIN; //入力待ち
	ev.data.fd = sockfd;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev)==-1) //epollインスタンスに登録
	{
		perror("ERROR EPOLL_CTL\n");
	}
	while(1){
	
		for(k=0;k<MAX_EVENTS;k++){
			printf("wait1 : event.data.fd[%d] : %d \n",k,events[k].data.fd);
		}
		//ディスクリプタをカーネルで管理
		val=epoll_wait(epollfd, events,MAX_EVENTS,-1);//要求されたI/Oに対して準備ができているファイルディスクリプタの数を返す。
		for(k=0;k<MAX_EVENTS;k++){
			printf("wait2 : event.data.fd[%d]:%d \n",k,events[k].data.fd);
		}
		printf("wait : %d  epollfd\n",val,epollfd);
		if(val==-1){
			perror("ERROR EPOLL_WAIT");
		}
		for(i=0;i<val;i++){
			printf("events[i].data.fd %d  , sockfd %d\n",events[i].data.fd, sockfd);
			if(events[i].data.fd==sockfd){ //目的のファイルディスクリプタ（fd）があったら専用操作を行う。
				socklen_t client_addr_len = sizeof client_addr;
				//1引数リッスン状態のソケット　２引数　接続中エントリのアドレスを受け取るバッファへのポインタを指定　３引数addrパラメータ	が指す+構造体の長さを含む整数へのポインタを任意で指定する。
				//外部からの接続を許可する
				
				clientfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len); //接続待ちのソケットを接続済みのソケットとして新規で作成する。
				printf("cliant socket : %d\n", clientfd);
				if(clientfd<0){
					perror("ERROR ACCEPT\n");
				}
				struct epoll_event access;
				memset(&access, 0, sizeof access);
				access.events = EPOLLIN | EPOLLET;
				access.data.fd = clientfd;
				if(epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &access)==-1){ //epollの制御下に入る
					perror("ERROR EPOLL_CTL\n");
				}
				printf("cliant socket : %d  is Connected!!\n", clientfd);
			}
			else if(events[i].events & EPOLLIN){
				char buff[100]="";
				printf("buff  : %s\n",buff);
				int readfd = events[i].data.fd;
				printf("readfd : %d\n",readfd);
				readval = read(readfd, buff, sizeof buff); //read
				printf("1 events  %d\n",events[0].data.fd);
				if(readval==0){
					printf("ClientSocket is disconnected\n");
					if(epoll_ctl(epollfd, EPOLL_CTL_DEL, readfd, &ev)==-1){ //epollからソケットを消す
						perror("ERROR EPOLL_CTL\n");
					}
					if(close(readfd)==-1){
						perror("ERROR CLOSE\n");
					}
				}else if(readval>0){
					printf("%s\n",buff);

				}else{
					perror("ERROR READ");
				}
			}
		}
	}
}

//fprintf(stderr, "a\n");
