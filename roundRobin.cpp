/*
This program runs a Discrete Event Simulation that simulates short-term scheduling in an OS.

AUTHORS: Lotanna Akukwe - lakukwe24@my.whitworth.edu
Discrete Event Simulation Project
CS Operating Systems
*/

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <limits>
#include <iostream>
#include <queue>
#include <unordered_map>
#include "../headers/event.h"
#include "../headers/job.h"

using namespace std;

int totalSimTime = 250;
int currentTime;
int count = 0;
int processor1 = 1;
int processor2 = 1;
int roundRobin = 30;
int waitingAvg = 0;
int responseAvg = 0;
float cpuAvg = 0;
int turnaroundT = 0;
int completeJob = 0;
int CPU1Time = 0;
int CPU2Time = 0;

std::vector<Event> FEL;
std::vector<Job> jobList;
vector<int> completedJobTimes;
queue<Job> readyQ1;
queue<Job> readyQ2;
queue<Job> readyQ3;
vector<queue<Job>> readyQ;
unordered_map<int, int> quantumMap = {
    {0, 40},
    {1, 60},
};
unordered_map<int, int> processorJobMap ={
    {1, 7},
    {2, 7},
};
Event imminentEvent;


//Looks at the list of jobs and seeds the FEL with their initial arrival events.
void generateArrivalEvents(std::vector<Job> jobs);

//Finds the imminent event (smallest timestamp), then deletes said event from FEL.                                                                                                  
Event findImminentEvent();

//Schedules the next arrival event and departure event while setting up the ready queues
int arrivalEvent();

//Processor accesses the queues and schedules next departure event. It also checks the io queue for blocked queues
int departureEvent();


int main(int argc, char* param) {
    //Sets up the first Future event
    FEL.push_back(Event(1, 1, 0));
    //FEL.push_back(Event(1, 10, 2));
    //sets up the ready queues
    readyQ.push_back(readyQ1);
    readyQ.push_back(readyQ2);
    readyQ.push_back(readyQ3);
    //Sets up the job list
    Job j1;
    j1.jobID = 0;
    j1.arrivalTime = 40;
    j1.burstTimes = {
        std::make_pair(std::string("cpu"), 15),
        std::make_pair(std::string("io"), 20),
        std::make_pair(std::string("cpu"), 40),
        //std::make_pair(std::string("io"), 200),
    };
    j1.priority = 0;

    Job j2;
    j2.jobID = 1;
    j2.arrivalTime = 90;
    j2.burstTimes = {
        std::make_pair(std::string("cpu"), 10),
        std::make_pair(std::string("io"), 30),
        std::make_pair(std::string("cpu"), 30)
    };
    j2.priority = 2;
    
    Job j3;
    j3.jobID = 2;
    j3.arrivalTime = 10;
    j3.burstTimes = {
        std::make_pair(std::string("cpu"), 10),
        std::make_pair(std::string("io"), 20),
        std::make_pair(std::string("cpu"), 10),
        std::make_pair(std::string("io"), 10),
        std::make_pair(std::string("cpu"), 30)
    };
    j3.priority = 1;

    Job j4;
    j4.jobID = 3;
    j4.arrivalTime = 20;
    j4.burstTimes = {
        std::make_pair(std::string("cpu"), 30)
    };
    j4.priority = 0;

    Job j5;
    j5.jobID = 4;
    j5.arrivalTime = 60;
    j5.burstTimes = {
        std::make_pair(std::string("cpu"), 30),
        std::make_pair(std::string("io"), 30),
        std::make_pair(std::string("cpu"), 13)
    };
    j5.priority = 2;

    jobList.push_back(j1);
    jobList.push_back(j2);
    jobList.push_back(j3);
    jobList.push_back(j4);
    jobList.push_back(j5);


    // generate arrival events
    //generateArrivalEvents(jobList);

    //Main loop where we copy the imminent event, set the current time, and execute the arrival and departure of the event
    while (currentTime < totalSimTime) {
        imminentEvent = findImminentEvent(); //Retrieves the event with the least event time
        currentTime = imminentEvent.timestamp; //Sets the current time as the current event's time
        //cout << "FEL vector size: " << FEL.size() << endl;
        //TODO: process imminent event here
        //FEL.erase(FEL.begin());
        switch (imminentEvent.type){
            case 1:
                arrivalEvent();
                continue;
            case 2:
                departureEvent();
                break;
            case 3:
                break;
            case 4:
                break;
        }
    }

    return 0;
}


int arrivalEvent(){
    //cout << "FEL Size: " << FEL.size() << endl; 
    //Creates new arrival event for next job in line.
    //if(imminentEvent.jobID+1 < jobList.size()){
        FEL.push_back(Event(1, currentTime + jobList[imminentEvent.jobID + 1].arrivalTime, jobList[imminentEvent.jobID + 1].jobID));
        //jobList[imminentEvent.jobID +1].arrivalTime += currentTime;
    //}else{
      //  cout << "No more jobs to schedule" << endl;
    //}
    
    //cout << "FEL arrival time: " << FEL[imminentEvent.jobID + 1].timestamp << " ID: " << FEL[imminentEvent.jobID + 1].jobID << endl;
    //cout << "FEL Size: " << FEL.size() << endl; 

    //If processor 1 is idle, then create a departure event for the this job
    //if a processor is available, then that processor would then execute the job
    //How long this takes is accounted for by the time between the job's arrival and that same job's departure
    //This is represented by the timestamp of the job's departure timestamp.
    if (processor1 == 1) {
        //"run" the task: if job is of priority 1 or 2, this means it must be from either round robin queues.
        if (jobList[imminentEvent.jobID].priority == 0 || jobList[imminentEvent.jobID].priority == 1) {
            if(jobList[imminentEvent.jobID].firstScheduleT == 0){
                jobList[imminentEvent.jobID].firstScheduleT = currentTime;
            }
            //Ternary statement: if the burst time is smaller than the time quantum, roundRobin will be the burst time. Otherwise, roundRobin = time quantum.
            //roundRobin = (jobList[imminentEvent.jobID].burstTimes[0].second < quantumMap[jobList[imminentEvent.jobID].priority]) ? jobList[imminentEvent.jobID].burstTimes[0].second : quantumMap[jobList[imminentEvent.jobID].priority];
            CPU1Time+=roundRobin;
            //make new departure event at that time stamp.
            FEL.push_back(Event(2, currentTime + roundRobin, jobList[imminentEvent.jobID].jobID));
            //Identify which processor did the job
            processorJobMap[1] = imminentEvent.jobID;
            //edit the burstTime value to accurately reflect remaining burstTime left.
            jobList[imminentEvent.jobID].burstTimes[0].second -= roundRobin;
            //After the edit, if the burstTime ends up 0, then just remove it.
            if (jobList[imminentEvent.jobID].burstTimes[0].second == 0 && !jobList[imminentEvent.jobID].burstTimes.empty()) {
                jobList[imminentEvent.jobID].burstTimes.erase(jobList[imminentEvent.jobID].burstTimes.begin());
            }
            processor1 = 0;
        }
        // } else {
        //     //this means the job is from the FCFS queue, thus will finish its CPU burst to the end.
        //     FEL.push_back(Event(2, currentTime + jobList[imminentEvent.jobID].burstTimes[0].second, jobList[imminentEvent.jobID].jobID));
        //     CPU1Time+=jobList[imminentEvent.jobID].burstTimes[0].second;
        //     jobList[imminentEvent.jobID].burstTimes[0].second -= roundRobin;
        //     if (jobList[imminentEvent.jobID].firstScheduleT == 0)
        //     {
        //         jobList[imminentEvent.jobID].firstScheduleT = currentTime;
        //     }
        //     // After the edit, if the burstTime ends up 0, then just remove it.
        //     if (jobList[imminentEvent.jobID].burstTimes[0].second == 0)
        //     {
        //         jobList[imminentEvent.jobID].burstTimes.erase(jobList[imminentEvent.jobID].burstTimes.begin());
        //     }
        //     processorJobMap[1] = imminentEvent.jobID;
        //     processor1 = 0;
        // }
        
    //If processor 1 isn't idle, then check if processor 2 is idle. If so, do the same as above.
    } else if (processor2 == 1) {
        //"run" the task: if job is of priority 1 or 2, this means it must be from either round robin queues.
        if (jobList[imminentEvent.jobID].priority == 0 || jobList[imminentEvent.jobID].priority == 1) {
            if(jobList[imminentEvent.jobID].firstScheduleT == 0){
                jobList[imminentEvent.jobID].firstScheduleT = currentTime;
            }
            //if the burst time is smaller than the time quantum, roundRobin will be the burst time. Otherwise, roundRobin = time quantum.
            roundRobin = (jobList[imminentEvent.jobID].burstTimes[0].second < quantumMap[jobList[imminentEvent.jobID].priority]) ? jobList[imminentEvent.jobID].burstTimes[0].second : quantumMap[jobList[imminentEvent.jobID].priority];
            CPU2Time += roundRobin;
            //make new departure event at that time stamp.
            FEL.push_back(Event(2, currentTime + roundRobin, jobList[imminentEvent.jobID].jobID));
            //Identify which processor did the job
            processorJobMap[2] = imminentEvent.jobID;
            //edit the burstTime value to accurately reflect remaining burstTime left.
            jobList[imminentEvent.jobID].burstTimes[0].second -= roundRobin;
            //After the edit, if the burstTime ends up 0, then just remove it.
            if (jobList[imminentEvent.jobID].burstTimes[0].second == 0) {
                jobList[imminentEvent.jobID].burstTimes.erase(jobList[imminentEvent.jobID].burstTimes.begin());
            }
            processor2 = 0;
        } 
        // else {
        //     //this means the job is from the FCFS queue, thus will finish its CPU burst to the end.
        //     FEL.push_back(Event(2, currentTime + jobList[imminentEvent.jobID].burstTimes[0].second, jobList[imminentEvent.jobID].jobID));
        //     CPU2Time+=jobList[imminentEvent.jobID].burstTimes[0].second;
        //     jobList[imminentEvent.jobID].burstTimes[0].second -= roundRobin;
        //     if (jobList[imminentEvent.jobID].firstScheduleT == 0)
        //     {
        //         jobList[imminentEvent.jobID].firstScheduleT = currentTime;
        //     }
        //     // After the edit, if the burstTime ends up 0, then just remove it.
        //     if (jobList[imminentEvent.jobID].burstTimes[0].second == 0)
        //     {
        //         jobList[imminentEvent.jobID].burstTimes.erase(jobList[imminentEvent.jobID].burstTimes.begin());
        //     }
        //     processorJobMap[2] = imminentEvent.jobID;
        //     processor2 = 0;
        // }

    //This else block should only execute when both processors are not idle (!= 1)
    } 
    else {
        readyQ[0].push(jobList[imminentEvent.jobID]);
    }
    /*
    for (auto Event: FEL){
        cout << "Event Type: " << Event.type << "  Event Time: " << Event.timestamp << "  Event ID: " << Event.jobID << endl;
    }
    */
    //count++;
    //return FEL[count].type;
}


int departureEvent(){
    //Checks if queue is empty and which processor performed the task
    if(processor1 == 1){
        for(int i = 0; i < readyQ.size(); i++){
            if(!readyQ[i].empty()){
                if(readyQ[i].front().burstTimes[0].first == "cpu"){
                    if(i == 2){
                        FEL.push_back(Event(2, currentTime + jobList[readyQ[i].front().jobID].burstTimes[0].second, jobList[readyQ[i].front().jobID].jobID));
                        CPU1Time+=jobList[readyQ[i].front().jobID].burstTimes[0].second;
                        jobList[readyQ[i].front().jobID].burstTimes[0].second -= roundRobin;
                        if(jobList[readyQ[i].front().jobID].firstScheduleT == 0){
                            jobList[readyQ[i].front().jobID].firstScheduleT = currentTime;
                        }
                        //After the edit, if the burstTime ends up 0, then just remove it.
                        if (jobList[readyQ[i].front().jobID].burstTimes[0].second == 0) {
                            jobList[readyQ[i].front().jobID].burstTimes.erase(jobList[readyQ[i].front().jobID].burstTimes.begin());
                        }
                        readyQ[i].pop(); //removes blocked event from the queue
                        processor1 = 0; //sets the processor as busy
                        processorJobMap[1] = readyQ[i].front().jobID; //sets the first processor is occupied with the queued job
                        return 0;
                    } else{
                        // if the burst time is smaller than the time quantum, roundRobin will be the burst time. Otherwise, roundRobin = time quantum.
                        //roundRobin = (jobList[readyQ[i].front().jobID].burstTimes[0].second < quantumMap[jobList[readyQ[i].front().jobID].priority]) ? jobList[readyQ[i].front().jobID].burstTimes[0].second : quantumMap[jobList[readyQ[i].front().jobID].priority];
                        CPU1Time += roundRobin;
                        // make new departure event at that time stamp.
                        FEL.push_back(Event(2, currentTime + roundRobin, jobList[readyQ[i].front().jobID].jobID));
                        // edit the burstTime value to accurately reflect remaining burstTime left.
                        jobList[readyQ[i].front().jobID].burstTimes[0].second -= roundRobin;
                        // After the edit, if the burstTime ends up 0, then just remove it.
                        if (jobList[readyQ[i].front().jobID].burstTimes[0].second == 0)
                        {
                            jobList[readyQ[i].front().jobID].burstTimes.erase(jobList[readyQ[i].front().jobID].burstTimes.begin());
                        }
                        readyQ[i].pop(); //removes blocked event from the queue
                        processor1 = 0; //sets the processor as busy
                        processorJobMap[1] = readyQ[i].front().jobID; //sets the first processor is occupied with the queued job
                        return 0;
                    }
                    
                }
            }
        }        
    } else if (processor2 == 1){
       for(int i = 0; i < readyQ.size(); i++){
            if(!readyQ[i].empty()){
                if(readyQ[i].front().burstTimes[0].first == "cpu"){
                    if(i == 2){
                        FEL.push_back(Event(2, currentTime + jobList[readyQ[i].front().jobID].burstTimes[0].second, jobList[readyQ[i].front().jobID].jobID));
                        CPU2Time+=jobList[readyQ[i].front().jobID].burstTimes[0].second;
                        jobList[readyQ[2].front().jobID].burstTimes[0].second -= roundRobin;
                        if(jobList[readyQ[2].front().jobID].firstScheduleT == 0){
                            jobList[readyQ[2].front().jobID].firstScheduleT = currentTime;
                        }
                        //After the edit, if the burstTime ends up 0, then just remove it.
                        if (jobList[readyQ[2].front().jobID].burstTimes[0].second == 0) {
                            jobList[readyQ[2].front().jobID].burstTimes.erase(jobList[readyQ[2].front().jobID].burstTimes.begin());
                        }
                        readyQ[i].pop(); //removes blocked event from the queue
                        processor1 = 0; //sets the processor as busy
                        processorJobMap[2] = readyQ[i].front().jobID; //sets the first processor is occupied with the queued job
                        return 0;
                    }else{
                        // if the burst time is smaller than the time quantum, roundRobin will be the burst time. Otherwise, roundRobin = time quantum.
                        //roundRobin = (jobList[readyQ[i].front().jobID].burstTimes[0].second < quantumMap[jobList[readyQ[i].front().jobID].priority]) ? jobList[readyQ[i].front().jobID].burstTimes[0].second : quantumMap[jobList[readyQ[i].front().jobID].priority];
                        CPU1Time += roundRobin;
                        // make new departure event at that time stamp.
                        FEL.push_back(Event(2, currentTime + roundRobin, jobList[readyQ[i].front().jobID].jobID));
                        // edit the burstTime value to accurately reflect remaining burstTime left.
                        jobList[readyQ[i].front().jobID].burstTimes[0].second -= roundRobin;
                        // After the edit, if the burstTime ends up 0, then just remove it.
                        if (jobList[readyQ[i].front().jobID].burstTimes[0].second == 0)
                        {
                            jobList[readyQ[i].front().jobID].burstTimes.erase(jobList[readyQ[i].front().jobID].burstTimes.begin());
                        }
                        readyQ[i].pop(); //removes blocked event from the queue
                        processor2 = 0; //sets the processor as busy
                        processorJobMap[2] = readyQ[i].front().jobID; //sets the first processor is occupied with the queued job
                        return 0;
                    }
                }
            }
        }
    }else{
        cout << "Queues are empty" << endl;
    }
    //     else if (!readyQ[1].empty() && jobList[readyQ[1].front().jobID].burstTimes[0].first == "cpu")
    //     {
    //         // if the burst time is smaller than the time quantum, roundRobin will be the burst time. Otherwise, roundRobin = time quantum.
    //         roundRobin = (jobList[readyQ[1].front().jobID].burstTimes[0].second < quantumMap[jobList[readyQ[1].front().jobID].priority]) ? jobList[readyQ[1].front().jobID].burstTimes[0].second : quantumMap[jobList[readyQ[1].front().jobID].priority];
    //         // make new departure event at that time stamp.
    //         FEL.push_back(Event(2, currentTime + roundRobin, jobList[readyQ[1].front().jobID].jobID));
    //         // edit the burstTime value to accurately reflect remaining burstTime left.
    //         jobList[readyQ[1].front().jobID].burstTimes[0].second -= roundRobin;
    //         // After the edit, if the burstTime ends up 0, then just remove it.
    //         if (jobList[readyQ[1].front().jobID].burstTimes[0].second == 0)
    //         {
    //             jobList[readyQ[1].front().jobID].burstTimes.erase(jobList[readyQ[1].front().jobID].burstTimes.begin());
    //         }
    //         readyQ[1].pop(); //removes blocked event from the queue
    //         processor2 = 0; //sets the processor as busy
    //         processorJobMap[2] = readyQ[1].front().jobID; //sets the second processor is occupied with the queued job
    //     }
    //     else if (!readyQ[2].empty() && jobList[readyQ[2].front().jobID].burstTimes[0].first == "cpu")
    //     {
    //         FEL.push_back(Event(2, currentTime + jobList[readyQ[2].front().jobID].burstTimes[0].second, readyQ[2].front().jobID));
    //         jobList[readyQ[2].front().jobID].burstTimes.erase(jobList[readyQ[2].front().jobID].burstTimes.begin());
    //         readyQ[2].pop(); //removes blocked event from the queue
    //         processor2 = 0; //sets the processor as busy
    //         processorJobMap[2] = readyQ[2].front().jobID; //sets the second processor is occupied with the queued job
    //     }
    //     else
    //     {
    //         cout << "Queues are empty" << endl;
    //     }
    // }

    //}
    
    string first = jobList[imminentEvent.jobID].burstTimes[0].first;
    int second = jobList[imminentEvent.jobID].burstTimes[0].second;
    int switchTime = 10;
    bool jobDone = false;
    int waitingtime = 0;
    int responsetime = 0;
    float cputime = 0;
    int oneprocessor = 391;

    //checks if the job is complete where all its burst times are 0
    if(jobList[imminentEvent.jobID].burstTimes.empty()){
        //Calculate turnaround time
        completeJob += 1;
        turnaroundT += imminentEvent.timestamp;
        //Calculate waiting time
        waitingtime = turnaroundT - jobList[imminentEvent.jobID].arrivalTime;
        waitingAvg += waitingtime;
        responsetime = jobList[imminentEvent.jobID].firstScheduleT - jobList[imminentEvent.jobID].arrivalTime;
        responseAvg += responsetime;
        cputime = CPU1Time + CPU2Time;
        cpuAvg += (cputime /currentTime)*100;
        // cout << "Calculated turnaround waiting time for job " << imminentEvent.jobID << endl;
        // cout << "Turnaround time for Job ID " << imminentEvent.jobID << " is " << imminentEvent.timestamp << endl;
        // cout << "Waiting time for Job ID " << imminentEvent.jobID << " is " << imminentEvent.timestamp - jobList[imminentEvent.jobID].arrivalTime << endl;
        // cout << "first schedule time " << imminentEvent.jobID << " is " << jobList[imminentEvent.jobID].firstScheduleT << endl;
        cout << "Average waiting time is " << waitingAvg / completeJob<< endl;
        cout << "Average turnaround time is " << turnaroundT/completeJob << endl;
        cout << "Average response time is " << responseAvg/completeJob << endl;
        //cout << "Total Sim time: " << currentTime << endl;
        cout << "Speeduptime: " << oneprocessor / currentTime << endl;
        cout << "Average CPU Utilization: " << (cpuAvg/currentTime)*100 << endl;
        //completeJob = imminentEvent.jobID;
        jobDone = true;
        
        //jobList.erase(jobList.begin() +imminentEvent.jobID); //erases this job from the joblist
        //Checks which processor was busy with the done job and releases it
        if(processorJobMap[1] == imminentEvent.jobID){
            processor1 = 1; 
        } else if(processorJobMap[2] == imminentEvent.jobID){
            processor2 = 1;
        }
    } else{
        for(const auto& burstT : jobList[imminentEvent.jobID].burstTimes){
            if (burstT.first == "cpu" && burstT.second != 0 ){
                //schedule arrival event to complete the burst time
                FEL.push_back(Event(1, currentTime + switchTime, jobList[imminentEvent.jobID].jobID));
                //cout << "Entered cpu if" << endl;
                roundRobin = (second < quantumMap[jobList[imminentEvent.jobID].priority]) ? jobList[imminentEvent.jobID].burstTimes[0].second : quantumMap[jobList[imminentEvent.jobID].priority];
                //jobList[imminentEvent.jobID].burstTimes[0].second = second - roundRobin;
                //Checks which processor was busy with the done job and releases it
                if(processorJobMap[1] == imminentEvent.jobID){
                    processor1 = 1;
                } else if(processorJobMap[2] == imminentEvent.jobID){
                    processor2 = 1;
                }
                break;
            } else if (burstT.first == "io"){
                //schedule an arrival event with switch time, current time, and the io time
                FEL.push_back(Event(1, currentTime + switchTime + burstT.second, imminentEvent.jobID));
                jobList[imminentEvent.jobID].burstTimes.erase(jobList[imminentEvent.jobID].burstTimes.begin()); //erases the current jobs io time
                //Checks which processor was busy with the done job and releases it
                if(imminentEvent.jobID == processorJobMap[1]){
                    processor1 = 1;
                } else if(imminentEvent.jobID == processorJobMap[2]){
                    processor2 = 1;
                }

                break;
            }
        }
    }

    /*
    for (auto Event: FEL){
        cout << "Departure Event Type: " << Event.type << "  Event Time: " << Event.timestamp << "  Event ID: " << Event.jobID << endl;
    }
    */
}


Event findImminentEvent() {
    //variable to hold the imminent event. Placeholder value of maximum integer (currently 2^32 - 1 for 32-bit systems).
    Event imminentEvent(1, std::numeric_limits<int>::max(), 0);
    /**
     * immEventIndex records the index of the imminent event in the FEL list while the for loop iterates.
     * Originally, the line FEL.erase(FEL.begin() + i) was used to remove the imminent event from the FEL as described in the pseudocode,
     * but it ended up removing more than it should since the for-loop aren't supposed to stop when it first encounters a timestamp smaller than 2^32 - 1.
     * immEventIndex solves this by keeping track of the index of the current imminent event, then removing it from the FEL AFTER the for-loop's done.
     * C++ shenanigans, I hate it down here.
    */
    int immEventIndex = 0;
    for (int i = 0; i < FEL.size(); i++) {
        if (FEL[i].timestamp < imminentEvent.timestamp) {
            imminentEvent = FEL[i];
            //FEL.erase(FEL.begin() + i)
            immEventIndex = i;
        }
    }
    FEL.erase(FEL.begin() + immEventIndex);
    return imminentEvent;

}
