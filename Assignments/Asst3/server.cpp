// Ryan Brooks
// ID: 1530605

// From: www.thegeekstuff.com/2011/12/c-socket-programming
// Note that port# 5000 is hard-coded into this implementation

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <time.h> 
#include <iostream>
#include <limits.h>
#include <poll.h>
#include <vector>

using namespace std;

// global variables
int TransSave = 0;
int workCnt = 1;
int startTime;
int endTime;
vector<string> clientNames;
vector<int> workCount;
int con = 0;

void Trans( int n ) {
    long i, j;

    // Use CPU cycles 
    j = 0;
    for( i = 0; i < n * 100000; i++ ) {
        j += i ^ (i+1) % (i+n);
    }
    TransSave += j;
    TransSave &= 0xff;
}

// from previous assignments I made.  converts c++ string to c style string
const char *conversion(string line) {
    return line.c_str();
}

// main function
int main(int argc, char *argv[])
{
    // quit if improper arguments set
    if (argc != 2) {
        fputs(conversion("Usage: ./server <port number>\n"), stdout);
        return 1;
    }

    // variable creation and basic setup for socket connection
    startTime = time(0);
    int listenfd = 0, connfd = 0;
    int epochTime;
    struct sockaddr_in serv_addr; 

    char nameBuff[HOST_NAME_MAX + 1024];
    char sendBuff[1025];
    char workBuff[1025];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));
    memset(workBuff, '0', sizeof(workBuff));
    memset(nameBuff, '0', sizeof(nameBuff));

    // check for correct port number
    uint16_t portNum = atoi(argv[1]);
    if (portNum < 5000 || portNum > 64000) {
        fputs(conversion("Port number must be between "), stdout);
        fputs(conversion("5000 and 64000\n"), stdout);
        return 1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portNum); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    int totalWork = 0;
    // main loop to connect and process client requests
    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        con++;
        fputs(conversion("Using port " + to_string(portNum)), stdout);
        fputs(conversion("\n"), stdout);
        int n = 0;
        // send acknowledgement of connection
        string line = "ack\n";
        const char *val = line.c_str();
        // send ack of connection
        snprintf(sendBuff, sizeof(sendBuff), val);
        write(connfd, sendBuff, strlen(sendBuff));
        sendBuff[n] = 0;
        // receive ack from client
        n = recv(connfd, sendBuff, sizeof(sendBuff) + 1, 0);
        line = sendBuff;
        if (line == "ack\n") {
            // client received acknowledgement and returned an acknowledgement
            // get name of client
            n = recv(connfd, nameBuff, sizeof(nameBuff) + 1, 0);
            string clientName = nameBuff;
            clientNames.push_back(clientName);
            // get first value
            n = recv(connfd, workBuff, sizeof(workBuff) + 1, 0);
            while (true) {
                // check for end of information
                if (atoi(workBuff) == -1) {
                    break;
                }
                int num = atoi(workBuff);
                epochTime = time(0);
                fputs(conversion(to_string(epochTime) + ": #  " + to_string(totalWork) + " (T  " + workBuff + ") from " 
                    + clientName + "\n"), stdout);
                // do work
                Trans(num);
                totalWork++;
                // send done and wait for next input
                string sendVal = "D" + to_string(totalWork);
                send(connfd, conversion(sendVal), strlen(conversion(sendVal)) + 1, 0);
                epochTime = time(0);
                fputs(conversion(to_string(epochTime) + ": #  " + to_string(totalWork) + " (Done)" + " from " 
                    + clientName + "\n"), stdout);
                workCnt++;
                memset(nameBuff, '0', sizeof(nameBuff));
                // get value
                n = recv(connfd, workBuff, sizeof(workBuff) + 1, 0);
                // check for end of work
                if (atoi(workBuff) == -1) {
                    workCount.push_back(workCnt);
                    workCnt = 1;
                    break;
                }
            }
        }
        // close connection and wait for another connection
        close(connfd);
        sleep(1);
        struct pollfd polls[1];
        polls[0].events = POLLIN;
        polls[0].fd = listenfd;
        // end program and print summary information if 60 seconds pass with no client connections
        int m = poll(polls, (unsigned long)1, 60000);
        if (m == 0) {
            int val = 0;
            fputs(conversion("\nSummary\n"), stdout);
            for (int i = 0; i < workCount.size(); i++) {
                fputs(conversion(to_string(workCount.at(i) - 1) + " transactions from "), stdout);
                fputs(conversion(clientNames.at(i) + "\n"), stdout);
            }
            for (int i = 0; i < workCount.size(); i++) {
                val += workCount.at(i) - 1;
            }
            endTime = time(0);
            double transTotal = val / (endTime - startTime - 60);
            fputs(conversion(to_string(transTotal) + " transactions/sec ("), stdout);
            fputs(conversion(to_string(val) + "/" + to_string((endTime - startTime) - 60) + ")\n"), stdout);
            return 0;
        }
     }
}
