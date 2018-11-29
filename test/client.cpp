#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#include <string>
#include <thread>

#define SERVER_PORT 8168

struct COMM_PAKT{
  unsigned char HEADER[4];
  unsigned char CMD;
  unsigned char res[3];

  struct PAYLOAD{
    unsigned char CLIENT_TYPE;
    unsigned char STATE;
    unsigned char LISTEN_ADDR[30];
  }payload;
};

enum ClientType { CREDENTIAL = 0, TRAYICON };
enum CMDType { CLI_REGISTER = 0, CLI_INQUIRE, CLI_EXIT,SVR_NOTIFY, ACK};
enum DevState { OUTOFRANGE=0, WITHIN, UNKNOWN };

int main(int argc, char **argv)
{
    sleep(2);
    
    char szAddr[] = "";
    COMM_PAKT packet = {0};
    packet.HEADER[0]= 0xFF;
    packet.HEADER[1]= 0xE0;
    packet.HEADER[2]= 0xFF;
    packet.HEADER[3]= 0xE0;

    packet.CMD = CMDType::CLI_REGISTER;

    
    strcpy(szAddr,std::string("F0:99:B6:92:84:E9").c_str());
    memcpy(&packet.payload.LISTEN_ADDR,szAddr, strlen(szAddr));

    struct sockaddr_in addr = {0};
    int n, sockfd,num=1;
    srandom(getpid());
    /* Create socket and connect to server */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if( -1== connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)))
    {
        printf("child {%d} connect failed {%d} \n", getpid(), errno);
        return -1;
    }
    else
    {
        printf("child {%d} connected \n", getpid());
    }
    
    
    while(1)
    {
        int sl = (random() % 10 ) +  1;
        num++;
        sleep(sl);

        write( sockfd, (void*)&packet, sizeof(COMM_PAKT));

        COMM_PAKT recv_buffer = {0};
        read( sockfd, (void*)&recv_buffer, sizeof(COMM_PAKT));
        if( recv_buffer.CMD == CMDType::ACK)
        {
            printf("ACK Recv OK \n");
            break;
        }
    }
 
    return 0;
}