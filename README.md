# Multi-Level Feedback Queue (MLFQ) and Round Robin Scheduler Simulation

## Project Overview
This project simulates short-term scheduling in an Operating System (OS) using a Discrete Event Simulation approach. The implemented algorithms include:
- **Multi-Level Feedback Queue (MLFQ):** A scheduling algorithm that prioritizes jobs based on their dynamic behavior and CPU burst times.
- **Round Robin (RR):** A time-slicing scheduling algorithm ensuring fair CPU time distribution among processes.

The simulation tracks key metrics such as:
- Average waiting time
- Average turnaround time
- Average response time
- CPU utilization

## Features
- Implements both MLFQ and Round Robin scheduling algorithms.
- Simulates real-world OS behavior with multiple processors.
- Calculates performance metrics for job scheduling.
- Handles CPU bursts and I/O wait times effectively.

## Key Algorithms and Data Structures
### Algorithms
- **Multi-Level Feedback Queue (MLFQ):**
  - Dynamically adjusts process priorities based on their execution history.
  - Uses preemption to ensure high-priority jobs get quick access to the CPU.
- **Round Robin (RR):**
  - Allocates fixed time slices (quantum) for each job.
  - Ensures fair CPU distribution among processes.

### Data Structures
- **Future Event List (FEL):** A vector used to manage events sorted by timestamp.
- **Ready Queues:** A vector of queues categorized by priority levels.
- **Job List:** A vector storing all jobs with attributes such as arrival time, burst times, and priority.
- **Hash Maps:**
  - `quantumMap`: Stores quantum values for different priority levels.
  - `processorJobMap`: Tracks jobs assigned to processors.

## Prerequisites
To build and run the project, you need:
- **C++ Compiler** (e.g., `g++`)
- Standard Template Library (STL)

## Usage
Run the compiled program with the following command:
```bash
./simulator
```
The program initializes with predefined job configurations and simulates scheduling events over a fixed simulation time.

### Key Components
- **Job:** Represents a process with attributes such as job ID, arrival time, priority, and burst times.
- **Event:** Represents scheduling events (e.g., arrivals, departures) with a timestamp.
- **Future Event List (FEL):** Manages events to be processed in chronological order.
- **Ready Queues:** Maintains jobs waiting for CPU access, classified by priority levels.

## Configuration
Modify the following parameters in the code to customize the simulation:
- `totalSimTime`: Total duration of the simulation.
- `quantumMap`: Time quantum values for different priority levels in Round Robin scheduling.
- `jobList`: List of predefined jobs with attributes such as priority and burst times.

## Metrics and Outputs
The simulation outputs:
- **Average Waiting Time:** The average time processes wait in the ready queue.
- **Average Turnaround Time:** The total time from job arrival to completion.
- **Average Response Time:** Time taken from job arrival to its first CPU access.
- **CPU Utilization:** Percentage of time CPUs are busy.

Example Output:
```
Average waiting time: 12 ms
Average turnaround time: 25 ms
Average response time: 10 ms
CPU utilization: 85%
```

## Code Structure
- `main.cpp`: Contains the simulation logic.
- `event.h`: Defines the Event class.
- `job.h`: Defines the Job class.

## Authors
- **Lotanna Akukwe** 

## Acknowledgments
Special thanks to the Dr. Qian Mao, teaching the CS Operating Systems course at Whitworth University, for the opportunity to develop this simulation.
