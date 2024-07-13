//http server from scratch in c 
//server--> 1)createSocketToListen2)bind3)wait4)createSocketToAccept5)acceptConnectionRequest6)acceptRequest7)sendResponse8)close
#include <stdio.h>
#include <string.h>
#include <winsock2.h>   //for socket
#pragma comment(lib, "ws2_32.lib")

//http handles the client server data transfer
void reqHandling(SOCKET gate){

    //1024 is the standard size
    char buffer[1000]= {0};

    //recieve buffer at gate
    recv(gate, buffer, 1000, 0);

    if(strstr(buffer, "GET /")){
        const char *thisIsAResponse= "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>hi</h1></body></html>";
        send(gate, thisIsAResponse, strlen(thisIsAResponse), 0);
    }
    
    if(strstr(buffer, "POST /")){
        if(strstr(buffer, "\r\n\r\n")){
            char *bodyFound= strstr(buffer, "\r\n\r\n");
            printf(bodyFound);
            //output is apparently also printing two extra lines
        }
        printf("hello");
        const char *thisIsAResponse= "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>body received</h1></body></html>";
        printf("HELLO");
        send(gate, thisIsAResponse, strlen(thisIsAResponse), 0);
        printf("END");
    }

    int closeGate= closesocket(gate);
    if (closeGate== -1){
        printf("closesocket faled for gate= %d\n", WSAGetLastError() );
        WSACleanup();
    }
}

int _cdecl main(){

    WSADATA wsaData= {0};
    int forError= 0;

    //init  winsock doc: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup
    forError= WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (forError!= 0) {
        printf("you done failed at startup dude? %d\n", forError);
        return 1;
    }

    //making server socket--> socket(domain, type, protocol)
    SOCKET door= INVALID_SOCKET;
    door= socket (AF_INET, SOCK_STREAM, 0);
    if (door== INVALID_SOCKET) {
        printf("door did not open :( %d\n", WSAGetLastError() );
        closesocket(door);
        WSACleanup();
        return 1;
    }

    int port= 3000;
    struct sockaddr_in thisIsBS;
    thisIsBS.sin_family = AF_INET;
    //htonl(host to network long): 32-bit unsigned integer from host byte order to network byte order
    thisIsBS.sin_addr.s_addr = inet_addr("127.0.0.1");
    //htons(host to network short): 16-bit unsigned integer from host byte order to network byte order
    thisIsBS.sin_port = htons(port);

    //associate local address with socket: int bind( [in] SOCKET s,const sockaddr *addr, [in] int namelen);
    int errForBind= bind (door, (struct sockaddr *)&thisIsBS, sizeof(thisIsBS) );
    //SOCKET_ERROR is defined as -1, so use either -1 or SOCKET_ERROR
    if (errForBind== -1){
        printf("can't bind %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    //listen for conn reqs, SOMAXCONN could be used to take maximum no of reqs possible as backlog
    int wait= listen(door, SOMAXCONN);
    if (wait== -1){
        printf("err listening %d\n", WSAGetLastError());
        closesocket(door);
        WSACleanup();
        return 1;
    }

    //socket returned by accept() that'd handle the actual connection
    while(1){
        SOCKET gate= INVALID_SOCKET;
        gate= accept (door, NULL, NULL);
        if (gate== INVALID_SOCKET) {
            printf("connection failed= %d\n", WSAGetLastError() );
            closesocket(door);
            WSACleanup();
            return 1;
        }
        
        //data transfer: hypertext transfer protocol, handled by the socket returned by accept()
        reqHandling(gate);
    }

    
    //closing socket and error handling; SOCKET_ERROR is defined as -1, so use either -1 or SOCKET_ERROR
    int closeDoor= closesocket(door);
    if (closeDoor== -1){
        printf("closesocket failed for door, listening socket= %d\n", WSAGetLastError() );
        WSACleanup();
        return 1;
    }

    //cleanup required for _cdecl
    WSACleanup();
    return 0;
}
