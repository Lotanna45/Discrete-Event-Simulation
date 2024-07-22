#ifndef JOB_H
#define JOB_H

#include <vector>
#include <utility> //std::pair
#include <string>

/**
 * Models a job. Has an jobID, its arrival time, and a preset list of burst times for simulation purposes.
 * IRL we don't know the burst times of each job, and while we can generate and assign vectors of random 
 * burst times, we currently have it to validate correctness.
*/
class Job {
    public:
    int jobID;
    int arrivalTime;
    //Priority can be in range [0, 2]. These are used to index queues. Smaller = higher priority.
    int priority;
    int firstScheduleT;
    /**
     * burstTimes is a vector of pairs, where the pairs' first value describe what type of burst it is,
     * and the second value denotes the burst time in miliseconds.
    */
    std::vector<std::pair<std::string, int>> burstTimes;

    Job() {
        this->jobID = -1;
        this->arrivalTime = -1;
        this->burstTimes = {};
        this->priority = -1;
        this->firstScheduleT = 0;
    }
};

#endif