#include<string>
#include<vector>
#include<sstream>
#include<iostream>
#include<unordered_map>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<sys/times.h>

using namespace std;

struct rusage usage;
struct tms;

struct jobs {
    int jobNum;
    string jobName;
    double CPUTime;
    double userTime;
    bool active;
    double waitTime;
    bool completed;
    int id;
};

vector<jobs> processes;
int cnt = 0;
int status;

const char *conversion(string line) {
   // line = "./" + line;
    return line.c_str();
}

int main(int argc, char **argv) {
    while (true) {
        vector<string> cmd;
        string value;
        string intVal;
        cout << "Shell379: ";
        while (true) {
            getline(cin, value);
            if (!value.empty()) {
                break;
            }
        }
        istringstream iss(value);
        while (iss >> intVal) {
            cmd.push_back(intVal);
        }
        // execlp("/home/cmput274/Desktop/yes1/yes2/yes3/a.out", "./a.out", NULL);
        if (cmd.front() == "exit") {
            while (wait(0) > 0){}
            break;
        } else if (cmd.front() == "jobs") {
            int activity = 0;
            cout << "running processes:" << endl;
            for (int i = 0; i < processes.size(); i++) {
                if (processes.at(i).active == true) {
                    activity++;
                }
            }
            cout << "processes =      " << activity << " active" << endl;
            cout << "completed processes:" << endl;
            int cpu = 0.0, user = 0.0;
            for (int i = 0; i < processes.size(); i++) {
                if (processes.at(i).completed == true) {
                    cpu += processes.at(i).CPUTime;
                    user += processes.at(i).userTime;
                }
            }
            cout << "user time =       " << user << " seconds" << endl;
            cout << " sys time =       " << cpu << " seconds" << endl;
        } else if (cmd.front() == "kill") {
            string val = cmd.at(1);
            cout << val << endl;
            int killed = stoi(val);
            for (int i = 0; i < processes.size(); i++) {
                if (killed == processes.at(i).jobNum) {
                    processes.erase(processes.begin() + i);
                    break;
                }
            }
        } else if (cmd.front() == "suspend") {
            int val = stoi(cmd.at(1));
            processes.at(val - 1).active = false;
            // 19 == SIGSTOP
            cout << processes.at(val - 1).id << endl;
            kill(processes.at(val - 1).id, 19);
        } else if (cmd.front() == "wait") {
            int val = stoi(cmd.at(1));
            waitpid(processes.at(val - 1).id, &status, NULL);
        } else if (cmd.front() == "sleep") {

        } else if (cmd.front() == "resume") {
            int val = stoi(cmd.at(1));
            // 18 == SIGCONT
            processes.at(val - 1).active = true;
            int pid = processes.at(val - 1).id;
            // 19 == SIGSTOP
            kill(pid, 18);
        } else {
            jobs process;
            process.jobNum = ++cnt;
            process.jobName = cmd.at(0);
            process.CPUTime = 0.0;
            process.userTime = 0.0;
            process.active = true;
            process.waitTime = 0.0;
            process.completed = false;
            int val = fork();
            if (val == 0) {
                //cout << getpid() << endl;
                execvp(conversion(cmd.at(0)), argv);
            }
            process.id = val;
            cout << process.id << endl;
            //cout << process.id << endl;
            processes.push_back(process);
            //times(tms);
            //cout << tms.tms_utime << endl;
        }
    }
    // switch (cmd.front()) {
    //     case "exit" : // wait until all processes end then exit program
    //                   return 0;
    //     case "jobs" : // return all jobs in the map
    //                   break;
    //     case "kill" : int job = cmd.at(1).stoi();
    //                   // remove Job from the map
    //                   break;
    //     case "wait" : int Job = cmd.at(1).stoi();
    //                   // wait for the Job to finish then resume
    //                   break;
    //     case "suspend" : int Job = cmd.at(1).stoi();
    //                      // suspend Job until resume is called on it
    //                      break; 
    //     case "sleep" : int time = cmd.at(1).stoi();
    //                    // wait until time has passsed to resume processes
    //                    break;
    //     case "resume" : int Job = cmd.at(1).stoi();
    //                     // resume Job
    //                     break;
    // }
    return 0;
}
