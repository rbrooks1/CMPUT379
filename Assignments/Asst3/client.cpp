/*
Ryan Brooks
ID: 1530605
*/

// From: www.thegeekstuff.com/2011/12/c-socket-programming
// Note that port# 5000 is hard-coded into this implementation

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>  
#include <iostream>
#include <limits.h>

using namespace std;

int TransSave = 0;

// Sleep simulates pauses between transactions
// Parameter given expresses wait time in hundreds of a nanosecond.

#include <stdio.h>
#include <time.h>

void Sleep( int n ) {
    struct timespec sleep;

    // Make sure pass a valid nanosecond time to nanosleep
    if( n <= 0 || n >= 100 ) {
        n = 1;
    }

    // Sleep for less than one second
    sleep.tv_sec  = 0;
        sleep.tv_nsec = n * 10000000 + TransSave;
    if( nanosleep( &sleep, NULL ) < 0 ) {
        perror ("NanoSleep" );
    }
}

// Taken from previous assignments I made.  This function converts from
// from a c++ string to a c style char array
const char *conversion(string line) {
    return line.c_str();
}

// main function
int main(int argc, char *argv[])
{
    // variable creation and basic setup for socket connections
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    char valBuff[1024];
    struct sockaddr_in serv_addr;
    int epochTime;

    // get the hostname of the machine
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, sizeof(hostname));
    string host = hostname;
    int pid = getpid();

    string filename = host + "." + to_string(pid);

    if(argc != 3)
    {
        printf("\n Usage: %s <port number> <ip of server> \n",argv[0]);
        return 1;
    } 

    memset(recvBuff, '0', sizeof(recvBuff));
    memset(valBuff, '0', sizeof(valBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    // check for correct port number
    uint16_t portNum = atoi(argv[1]);
    if (portNum < 5000 || portNum > 64000) {
        fputs(conversion("Port number must be between "), stdout);
        fputs(conversion("5000 and 64000\n"), stdout);
        return 1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNum); 

    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    // main loop where the work is sent to the server for processing
    int transactions = 0;
        while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
        {
            fputs(conversion("Using port " + to_string(portNum) + "\n"), stdout);
            fputs(conversion("Using server address "), stdout);
            fputs(argv[2], stdout);
            fputs(conversion("\n"), stdout);
            fputs(conversion("Host " + filename + "\n"), stdout);
            recvBuff[n] = 0;
            string line = recvBuff;
            string val = "ack\n";
            // receive ack from server
            if (line == "ack\n") {
                // send ack back to server
                send(sockfd, conversion(val), strlen(conversion(val)) + 1, 0);
            }
            // send the filename
            send(sockfd, conversion(filename), strlen(conversion(filename)) + 1, 0);
            TransSave += rand() % 38;
            // send value for server to work on
            while (true) {
                string info;
                cin >> info;
                if (cin.eof()) {
                    string end = "-1\n";
                    send(sockfd, conversion(end), strlen(conversion(end)) + 1, 0);
                    break;
                }
                const char *newInfo = conversion(info);
                if (newInfo[0] == 'S') {
                    string use;
                    for (int i = 1; i < sizeof(newInfo); i++) {
                        use += newInfo[i];  
                    }
                    fputs(conversion("Sleep " + use + " units " + "\n"), stdout);
                    fputs(conversion(" units \n"), stdout);
                    Sleep(stoi(use));
                } else if (newInfo[0] == 'T') {
                    transactions++;
                    string sendVal = "";
                    for (int i = 1; i < sizeof(newInfo); i++) {
                        sendVal += newInfo[i];
                    }
                    string newline = "\n";
                    sendVal.append(newline);
                    TransSave += rand() % 38;
                    epochTime = time(0);
                    fputs(conversion(to_string(epochTime) + ": Send (T" + sendVal + ") " + "\n"), stdout);
                    fputs(conversion(") \n"), stdout);
                    // send value
                    send(sockfd, conversion(sendVal), strlen(conversion(sendVal)) + 1, 0);
                    // wait for the server to finish processing information
                    while (true) {
                        n = recv(sockfd, recvBuff, sizeof(recvBuff), 0);
                        string trNum;
                        for (int i = 1; i < sizeof(recvBuff); i++) {
                            trNum += recvBuff[i];
                        }
                        epochTime = time(0);
                        fputs(conversion(to_string(epochTime) + ": Recv (D " + trNum + ") " + "\n"), stdout);
                        fputs(conversion(") \n"), stdout);
                        if (recvBuff[0] == 'D') {
                            break;
                        }
                    }
                }
            }
    fputs(conversion("Sent " + to_string(transactions) + " transactions\n"), stdout);
    break;
    }
    return 0;
}
