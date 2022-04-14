#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
void main()
{
	struct sockaddr_in serv_addr;
	int sockfd,val=100;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("sockfd cli : %d\n",sockfd);
	if(sockfd ==-1){
		perror("ERROR SOCKET\n");
		return;
	}
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("172.28.34.67");
	serv_addr.sin_port = htons(50000);
	val = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));//sockfdが参照しているソケットをserv_addrで指定されたアドレスに接続する。
	printf("sockclifd : %d  ",sockfd);
	if(val ==-1){
		perror("ERROR CONNECT\n");
		if(close(sockfd)){
				perror("ERROR CLOSE cli\n");
				return;
			}
		return;
	}
	while(1){ //書き込みを繰り返す。
		int input_len = 0;
		char buff[100];
		scanf("%s",&buff); //クライアントに入力させる
		printf("%s\n",buff);
		input_len = strlen(buff);
		printf("len : %d\n",input_len);
		val = write(sockfd, &buff, input_len); //ソケットに入力を書き込む
		
		printf("write : %d\n", val);
		if(val<0){
			perror("ERROR WRITE\n");
		}
		if(strncmp("shutdown", buff, 8)==0){ //"shutdown"と入力されたらソケットを閉じる
			
			if(close(sockfd)){
				perror("ERROR CLOSE cli\n");
				return;
			}
			break;
		}
	}
}
