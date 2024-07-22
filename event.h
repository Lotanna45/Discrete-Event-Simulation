#ifndef EVENT_H_
#define EVENT_H_
/**
 * Models an event.
 * Has a type, a timestamp (when the event happens), and an associated jobID.
 * An event can be of types:
 *     1: is an arrival event - meaning the associated job has arrived and is ready.
 *     2: is a departure event - meaning the job is either done and exiting OR timed out.
 *     3: is a CPU event - the job is going to start using a CPU.
 *     4: is an IO event - the job is going to start using an I/O peripheral device.
 */
class Event {
    public:
        int type;
        int timestamp;
        int jobID;

        //Constructor
        Event(int type, int timestamp, int jobID) {
            this->type = type;
            this->timestamp = timestamp;
            this-> jobID = jobID;
        }

        //Default constructor
        Event() {
            this->type = 1;
            this->timestamp = 0;
            this->jobID = 0;
        }

        Event(const Event& copycat){
            type = copycat.type;
            timestamp = copycat.timestamp;
            jobID = copycat.jobID;
        }

        //print event information. Normally I don't need it because the debugger exists, but today the debugger doesn't work, so here it is.
        // void toString(){
        //     printf("Event of type %d for job %d at %dms\n", type, jobID, timestamp);
        // }
};

#endif