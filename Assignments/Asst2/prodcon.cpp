/*
Assignment: CMPUT 379 assignment 2 Winter 2020
*/  

#include<iostream>
#include<vector>
#include<string>
#include<string.h>
#include<sstream>
#include<unistd.h>
#include<fstream>
#include<stdlib.h>
#include<queue>
#include<setjmp.h>
#include<pthread.h>
#include<iomanip>
#include<stdio.h>
#include<time.h>

using namespace std;


// global variable definitions
queue<string> work;

pthread_mutex_t prodMutex;
pthread_mutex_t consMutex;
pthread_mutex_t genMutex;
pthread_mutex_t printMutex;
int flag = 1;
int inc = 1;
float startTime;
int ask;
int receive;
int doing;
int complete;
int slept;
int threadCount;
ofstream file;
vector<pthread_t> threadIds;
vector<int> totalWork;


// DO NOT MODIFY THIS FILE

// Trans simulates processing a transaction
// It does not simulate this by using sleep -- that would free up the CPU.
// Thus, Trans does a silly computation to use up CPU cycles. Note that the 
// computation has to produce some result. If it doesn't, then a smart 
// compiler will notice this and delete (optimize) the code!

// The loop's computation (to waste time) is used to modify TransSave.
// TransSave is added to the wait time in Sleep -- a few billionths of a
// second. By doing this, something "real" comes from the computation, and
// the compiler is fooled.

int TransSave = 0;

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

/*
this function prints out the summary information at the end of the program
*/
void outputSum() {
    float endTime = clock();
    file << "Summary:" << endl;
    file << "   " << "Work        " << doing << endl;
    file << "   " << "Ask         " << ask << endl;
    file << "   " << "Receive     " << receive << endl;
    file << "   " << "Complete    " << complete << endl;
    file << "   " << "Sleep       " << slept << endl;
    for (int i = 0; i < threadCount; i++) {
        file << "   " << "Thread  " << i + 1 << "   " << totalWork.at(i) << endl;
    }
    file << "transactions per second: " << doing / ((endTime - startTime) / CLOCKS_PER_SEC) << endl;  
}

// this function converts from a c++ string to a c style string
const char *conversion(string line) {
    return line.c_str();
}

// the consumer function
void * consumers(void *arg) {
    // get the consumer id
    int id;
    for (int i = 0; i < threadCount; i++) {
        if (threadIds.at(i) = pthread_self()) {
            id = i + inc;
            inc++;
            break;
        }
    }
    while (true) {
        // ask for work
        pthread_mutex_lock(&printMutex);
        float eTime = clock();
        file << fixed << setprecision(3) << (eTime - startTime) / CLOCKS_PER_SEC << " ";
        file << "ID= " << id << "       " << "Ask" << endl;
        ask++;
        pthread_mutex_unlock(&printMutex);
        pthread_mutex_lock(&consMutex);
        // block producer if there is too much work
        if (work.size() >= 2 * threadCount) {
            pthread_mutex_lock(&prodMutex);
        }
        // check if consumer should block or if consumer should exit
        if (work.size() == 0) {
            pthread_mutex_unlock(&consMutex);
            while (true) {
                if (work.size() != 0) {                    
                    break;
                } else if (flag == 0 && work.size() == 0) {
                    pthread_mutex_unlock(&consMutex);
                    return 0;                      
                } else {
                    file << " " << endl;
                    // sleep if consumer needs to wait for more work
                    sleep(2);
                }
            }
            pthread_mutex_lock(&consMutex);
        }
        // get work from queue and execute it
        pthread_mutex_lock(&genMutex);
        string val = work.front();
        char *arr = &val[0];
        int n = arr[1] - '0';
        pthread_mutex_lock(&printMutex);
        eTime = clock();
        file << fixed << setprecision(3) << (eTime - startTime) / CLOCKS_PER_SEC << " ";
        file << "ID= " << id << "       " << "Receive" << "    " << n << endl;
        receive++;
        pthread_mutex_unlock(&printMutex);
        work.pop();
        pthread_mutex_unlock(&genMutex);
        if (work.size() < 2 * threadCount) {
            pthread_mutex_unlock(&prodMutex);
        }
        Trans(n);
        // finish work and print information
        pthread_mutex_unlock(&consMutex);
        pthread_mutex_lock(&printMutex);
        eTime = clock();
        file << fixed << setprecision(3) << (eTime - startTime) / CLOCKS_PER_SEC << " ";
        file << "ID= " << id << "       " << "Complete" << "   " << n << endl;
        complete++;
        totalWork.at(id - 1)++;
        pthread_mutex_unlock(&printMutex);
    }
    return 0;
}

// main function
int main(int argc, char* argv[]) {
    startTime = clock();
    int fileNo;
    // get arguments and do some error handling to make sure the correct amount of arguments are passed
    threadCount = atoi(argv[1]);
    if (argc == 3) {
        fileNo = atoi(argv[2]);
    } else if (argc == 2) {
        fileNo = 0;
    } else if (argc == 1 || argc > 3) {
        cout << "Too many or too few arguments.  Try again" << endl;
        exit(1);
    }
    // check for the output file and if it doesn't exist then create it in the same directory
    string fileName;
    if (fileNo > 0) {
        fileName = "prodcon." + to_string(fileNo) + ".log";
    } else {
        fileName = "prodcon.log";
    }
    file.open(fileName);
    if (!file.good()) {
        system(conversion("touch " + fileName));
        file.open(fileName);
    }
    file.clear();
    // create the threads
    int err;
    pthread_t threads[threadCount];
    int n;
    for (int i = 0; i < threadCount; i++) {
        err = pthread_create(&threads[i], NULL, consumers, (void *)&n);
    }
    // create thread ids for later use as well as total work for each thread for later use
    for (int i = 0; i < threadCount; i++) {
        threadIds.push_back(threads[i]);
        totalWork.push_back(0);
    }
    // producer loop
    while (true) {
        // block consumers if no data in queue
        if (work.size() == 0) {
            pthread_mutex_lock(&consMutex);
        }
        // read ing information
        float eTime;
        pthread_mutex_lock(&prodMutex);
        string val;
        cin >> val;
        if (cin.eof()) {
            // end of file reached
            flag = 0;
            pthread_mutex_lock(&printMutex);
            eTime = clock();
            file << fixed << setprecision(3) << (eTime - startTime) / CLOCKS_PER_SEC << " ";
            file << "ID= " << "0" << "       " << "end" << endl;
            pthread_mutex_unlock(&printMutex); 
            pthread_mutex_unlock(&prodMutex);  
            break;
        }
        if (work.size() >= 2 * threadCount) {
            pthread_mutex_unlock(&prodMutex);
            while (true) {
                if (work.size() < 2 * threadCount) {
                    break;
                } else {
                    sleep(2);
                }
            }
            pthread_mutex_lock(&prodMutex);
        }
        // sleep if asked
        if (val[0] == 'S') {
            char *arr = &val[0];
            int n = arr[1] - '0';
            pthread_mutex_lock(&printMutex);
            eTime = clock();
            file << fixed << setprecision(3) << (eTime - startTime) / CLOCKS_PER_SEC << " ";
            file << "ID= " << "0" << "       " << "Sleep" << "      " << n << endl;
            slept++;
            pthread_mutex_unlock(&printMutex);
            Sleep(n);
        } else {
            // otherwise add information to queue
            pthread_mutex_lock(&genMutex);
            pthread_mutex_lock(&printMutex);
            eTime = clock();
            file << fixed << setprecision(3) << (eTime - startTime) / CLOCKS_PER_SEC << " ";
            file << "ID= " << "0" << "       " << "Work" << "       " << val[1] << endl;
            doing++;
            pthread_mutex_unlock(&printMutex);
            work.push(val);
            pthread_mutex_unlock(&genMutex);
            // unblock consumers if needed
            if (work.size() >= 0) {
                pthread_mutex_unlock(&consMutex);       
            }
        }
        pthread_mutex_unlock(&prodMutex);
    }
    pthread_join(threads[0], NULL);
    outputSum();
    file.close();
    return 0;
}