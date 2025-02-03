
#include <bits/stdc++.h>
#include "scheduler_parser.h"

using namespace std;

/** Global Constants **/
const string TRACE_MODE = "trace";
const string STATS_MODE = "statistics";
const string SCHEDULING_ALGORITHMS[9] = {"", "FCFS", "RR", "SPN", "SRT", "HRRN", "FB-1", "FB-2i", "AGING"};

// Comparator functions for sorting
bool compareByServiceTime(const tuple<string, int, int>& a, const tuple<string, int, int>& b) {
    return get<2>(a) < get<2>(b);
}

bool compareByArrivalTime(const tuple<string, int, int>& a, const tuple<string, int, int>& b) {
    return get<1>(a) < get<1>(b);
}

bool compareByResponseRatio(const tuple<string, double, int>& a, const tuple<string, double, int>& b) {
    return get<1>(a) > get<1>(b);
}

bool compareByPriority(const tuple<int, int, int>& a, const tuple<int, int, int>& b) {
    if (get<0>(a) == get<0>(b)) {
        return get<2>(a) > get<2>(b);
    }
    return get<0>(a) > get<0>(b);
}

// Function to clear the timeline matrix
void resetTimeline() {
    for (int i = 0; i < lastMoment; ++i) {
        for (int j = 0; j < processCount; ++j) {
            executionTimeline[i][j] = ' ';
        }
    }
}

// Utility functions to access tuple components
string extractProcessName(const tuple<string, int, int>& process) {
    return get<0>(process);
}

int extractArrivalTime(const tuple<string, int, int>& process) {
    return get<1>(process);
}

int extractServiceTime(const tuple<string, int, int>& process) {
    return get<2>(process);
}

int getPriorityLevel(tuple<string, int, int> &process)
{
    return get<2>(process);
}

// Calculate the response ratio
double computeResponseRatio(int waitingTime, int serviceTime) {
    return (waitingTime + serviceTime) * 1.0 / serviceTime;
}

// Fill in wait time on the timeline
void markWaitingTimes() {
    for (int i = 0; i < processCount; ++i) {
        int arrivalTime = extractArrivalTime(processList[i]);
        for (int t = arrivalTime; t < completionTime[i]; ++t) {
            if (executionTimeline[t][i] != '*') {
                executionTimeline[t][i] = '.';
            }
        }
    }
}

// First-Come-First-Serve (FCFS) scheduling algorithm
void runFCFS() {
    int currentTime = extractArrivalTime(processList[0]);
    for (int i = 0; i < processCount; ++i) {
        int arrival = extractArrivalTime(processList[i]);
        int service = extractServiceTime(processList[i]);

        if(currentTime < arrival) currentTime = arrival;

        completionTime[i] = currentTime + service;
        turnaroundTime[i] = completionTime[i] - arrival;
        normalizedTurnaround[i] = (turnaroundTime[i] * 1.0) / service;

        for (int t = currentTime; t < completionTime[i]; ++t) {
            executionTimeline[t][i] = '*';
        }
        for (int t = arrival; t < currentTime; ++t) {
            executionTimeline[t][i] = '.';
        }

        currentTime += service;
    }
}

// Round Robin (RR) scheduling algorithm
void runRoundRobin(int quantum) {
    queue<pair<int, int>> readyQueue; // Index, Remaining Service Time
    int currentIndex = 0;

    // Add the first process if it arrives at time 0
    if (currentIndex < processCount && extractArrivalTime(processList[currentIndex]) == 0) {
        readyQueue.push({currentIndex, extractServiceTime(processList[currentIndex])});
        ++currentIndex;
    }

    int currentQuantum = quantum; // Initialize the quantum for the first process

    for (int time = 0; time < lastMoment; ++time) {
        // Add new arrivals to the ready queue before process execution to ensure correct order of preemption
        while (currentIndex < processCount && extractArrivalTime(processList[currentIndex]) == time + 1) {
            readyQueue.push({currentIndex, extractServiceTime(processList[currentIndex])});
            ++currentIndex;
        }

        if (!readyQueue.empty()) {
            // Process at the front of the queue
            int processIndex = readyQueue.front().first;
            int &remainingService = readyQueue.front().second;

            // Execute the current process
            executionTimeline[time][processIndex] = '*';
            --remainingService;
            --currentQuantum;

            if (currentQuantum == 0 && remainingService == 0) { 
                // Process completes
                completionTime[processIndex] = time + 1;
                turnaroundTime[processIndex] = completionTime[processIndex] - extractArrivalTime(processList[processIndex]);
                normalizedTurnaround[processIndex] = (turnaroundTime[processIndex] * 1.0) / extractServiceTime(processList[processIndex]);
                readyQueue.pop();
                currentQuantum = quantum; // Reset quantum
            } else if (currentQuantum == 0 && remainingService != 0) {
                // Quantum expires, preempt the process
                readyQueue.push({processIndex, remainingService});
                readyQueue.pop();
                currentQuantum = quantum; // Reset quantum
            } else if(currentQuantum != 0 && remainingService == 0){
                // Process completes
                completionTime[processIndex] = time + 1;
                turnaroundTime[processIndex] = completionTime[processIndex] - extractArrivalTime(processList[processIndex]);
                normalizedTurnaround[processIndex] = (turnaroundTime[processIndex] * 1.0) / extractServiceTime(processList[processIndex]);
                readyQueue.pop();
                currentQuantum = quantum; // Reset quantum
            }
        }

        // Check for new arrivals after process execution
        while (currentIndex < processCount && extractArrivalTime(processList[currentIndex]) == time + 1) {
            readyQueue.push({currentIndex, extractServiceTime(processList[currentIndex])});
            ++currentIndex;
        }
    }

    markWaitingTimes(); // Calculate waiting times for all processes
}

// Shortest Process Next (SPN) scheduling algorithm
void runSPN() { // Non Preemptive
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> readyQueue; // Pair of Service Time and Index
    int currentIndex = 0;

    for (int time = 0; time < lastMoment; ++time) {
        while (currentIndex < processCount && extractArrivalTime(processList[currentIndex]) <= time) {
            readyQueue.push({extractServiceTime(processList[currentIndex]), currentIndex});
            ++currentIndex;
        }

        if (!readyQueue.empty()) {
            int processIndex = readyQueue.top().second;
            readyQueue.pop();

            for (int t = time; t < time + extractServiceTime(processList[processIndex]); ++t) {
                executionTimeline[t][processIndex] = '*';
            }

            completionTime[processIndex] = time + extractServiceTime(processList[processIndex]);
            turnaroundTime[processIndex] = completionTime[processIndex] - extractArrivalTime(processList[processIndex]);
            normalizedTurnaround[processIndex] = (turnaroundTime[processIndex] * 1.0) / extractServiceTime(processList[processIndex]);

            time = completionTime[processIndex] - 1;
        }
    }
    markWaitingTimes();
}

// Shortest Remaining Time (SRT) scheduling algorithm
void runSRT() {
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> readyQueue; // Pair of Remaining Service Time and Index
    int currentIndex = 0;

    for (int time = 0; time < lastMoment; ++time) {
        while (currentIndex < processCount && extractArrivalTime(processList[currentIndex]) == time) {
            readyQueue.push({extractServiceTime(processList[currentIndex]), currentIndex});
            ++currentIndex;
        }

        if (!readyQueue.empty()) {
            int processIndex = readyQueue.top().second;
            int remainingTime = readyQueue.top().first;
            readyQueue.pop();

            executionTimeline[time][processIndex] = '*';

            if (remainingTime > 1) {
                readyQueue.push({remainingTime - 1, processIndex});
            } else {
                completionTime[processIndex] = time + 1;
                turnaroundTime[processIndex] = completionTime[processIndex] - extractArrivalTime(processList[processIndex]);
                normalizedTurnaround[processIndex] = (turnaroundTime[processIndex] * 1.0) / extractServiceTime(processList[processIndex]);
            }
        }
    }
    markWaitingTimes();
}

void runHRRN() {
    vector<tuple<string, double, int>> readyQueue; // Vector of tuple <process_name, process_response_ratio, time_in_service> for processes that are in the ready queue
    int currentIndex = 0;

    for (int time = 0; time < lastMoment; ++time) {
        while (currentIndex < processCount && extractArrivalTime(processList[currentIndex]) <= time) {
            readyQueue.push_back(make_tuple(extractProcessName(processList[currentIndex]), 1.0, 0)); // Initialize response ratio and service time tracker
            currentIndex++;
        }

        for (auto& process : readyQueue) {
            string name = get<0>(process);
            int idx = processIndexMap[name];
            int waitTime = time - extractArrivalTime(processList[idx]);
            get<1>(process) = computeResponseRatio(waitTime, extractServiceTime(processList[idx]));
        }
        //Sort from Higher to Lower Response Ratio
        sort(readyQueue.begin(), readyQueue.end(), compareByResponseRatio);

        if (!readyQueue.empty()) {
            int idx = processIndexMap[get<0>(readyQueue[0])];
            for (int t = time; t < time + extractServiceTime(processList[idx]) && t < lastMoment; ++t) {
                executionTimeline[t][idx] = '*';
                get<2>(readyQueue[0])++;
            }
            completionTime[idx] = time + extractServiceTime(processList[idx]);
            turnaroundTime[idx] = completionTime[idx] - extractArrivalTime(processList[idx]);
            normalizedTurnaround[idx] = (turnaroundTime[idx] * 1.0) / extractServiceTime(processList[idx]);

            readyQueue.erase(readyQueue.begin());
            time = completionTime[idx] - 1;
        }
    }
    markWaitingTimes();
}

// Feedback Queue 1 (FB-Q1) scheduling algorithm
void runFeedbackQ1() {
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> readyQueue; //Pair of Priority Level and Process Index
    vector<int> remainingTime(processCount);
    for (int i = 0; i < processCount; ++i) {
        remainingTime[i] = extractServiceTime(processList[i]);
    }

    int currentIndex = 0;

    if (extractArrivalTime(processList[0])==0) {
        readyQueue.push({0,currentIndex});
        currentIndex++;
    }

    for (int time = 0; time < lastMoment; ++time) {
        if (!readyQueue.empty()) {
            int priorityLevel = readyQueue.top().first;
            int processIndex =readyQueue.top().second;
            readyQueue.pop();

            while (currentIndex < processCount && extractArrivalTime(processList[currentIndex]) == time + 1) {
                readyQueue.push({0, currentIndex});
                currentIndex++;
            }

            executionTimeline[time][processIndex] = '*';
            remainingTime[processIndex]--;

            if (remainingTime[processIndex] == 0) {
                completionTime[processIndex] = time + 1;
                turnaroundTime[processIndex] = completionTime[processIndex] - extractArrivalTime(processList[processIndex]);
                normalizedTurnaround[processIndex] = (turnaroundTime[processIndex] * 1.0) / extractServiceTime(processList[processIndex]);
            } else {
                if (readyQueue.size()>=1)
                    readyQueue.push({priorityLevel+1,processIndex});
                else
                    readyQueue.push({priorityLevel,processIndex});
            }
        }
        while (currentIndex < processCount && extractArrivalTime(processList[currentIndex]) == time + 1) {
            readyQueue.push({0, currentIndex});
            remainingTime[currentIndex] = extractServiceTime(processList[currentIndex]);
            currentIndex++;
        }
    }
    markWaitingTimes();
}

// Aging scheduling algorithm
void runAging(int quantum)
{
    // Ready queue: priority level, process index, and total waiting time.
    vector<tuple<int, int, int>> v;
    unordered_map<int, int> remainingTime; // Map to track remaining service time.
    int currentIndex = 0, currentProcess = -1;

    // Main simulation loop.
    for (int time = 0; time < lastMoment; time++) {
        // Add new arrivals to the ready queue.
        while (currentIndex < processCount && extractArrivalTime(processList[currentIndex]) <= time) {
            v.push_back({getPriorityLevel(processList[currentIndex]), currentIndex, 0});
            remainingTime[currentIndex] = extractServiceTime(processList[currentIndex]);
            currentIndex++;
        }

        // Skip if no processes are ready.
        if (v.empty()) {
            continue;
        }

        // Sort the queue by priority.
        sort(v.begin(), v.end(), compareByPriority);

        // Select the process with the highest priority.
        currentProcess = get<1>(v[0]);

        // Adjust priorities for all processes in the queue.
        for (int i = 0; i < v.size(); i++) {
            if(i != 0) {
                // Increment priority and waiting time for waiting processes.
                get<0>(v[i])++;
                get<2>(v[i])++;
            }
        }

        // Execute the selected process for its quantum or until finished.
        int currentQuantum = quantum;
        while (currentQuantum-- && remainingTime[currentProcess] > 0 && time < lastMoment) {
            executionTimeline[time][currentProcess] = '*';
            remainingTime[currentProcess]--;
            time++;
        }
        time--; // Adjust for the outer loop increment.

        // If the process is finished, remove it from the queue.
        if (remainingTime[currentProcess] == 0) {
            completionTime[currentProcess] = time + 1;
            turnaroundTime[currentProcess] = completionTime[currentProcess] - extractArrivalTime(processList[currentProcess]);
            normalizedTurnaround[currentProcess] = (turnaroundTime[currentProcess] * 1.0) / extractServiceTime(processList[currentProcess]);
            v.erase(v.begin());
        }
    }

    // Fill in waiting times for the timeline.
    markWaitingTimes();
}

void runFeedbackQ2i()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> readyQueue; //pair of priority level and process index
    unordered_map<int,int> remainingTime; //map from process index to the remaining service time
    int currentIndex = 0;
    if(extractArrivalTime(processList[0]) == 0) {
        readyQueue.push({0,currentIndex});
        remainingTime[currentIndex] = extractServiceTime(processList[currentIndex]);
        currentIndex++;
    }
    for(int time = 0; time < lastMoment; time++) {
        if(!readyQueue.empty()) {
            int priorityLevel = readyQueue.top().first;
            int processIndex =readyQueue.top().second;
            int arrivalTime = extractArrivalTime(processList[processIndex]);
            int serviceTime = extractServiceTime(processList[processIndex]);
            readyQueue.pop();
            while(currentIndex < processCount && extractArrivalTime(processList[currentIndex]) <= time + 1) {
                    readyQueue.push({0,currentIndex});
                    remainingTime[currentIndex] = extractServiceTime(processList[currentIndex]);
                    currentIndex++;
            }

            int quantum = pow(2, priorityLevel);
            int temp = time;
            while(quantum && remainingTime[processIndex]){
                quantum--;
                remainingTime[processIndex]--;
                executionTimeline[temp++][processIndex]='*';
            }

            if(remainingTime[processIndex] == 0) {
                completionTime[processIndex] = temp;
                turnaroundTime[processIndex] = (completionTime[processIndex] - arrivalTime);
                normalizedTurnaround[processIndex] = (turnaroundTime[processIndex] * 1.0 / serviceTime);
            }else{
                if(readyQueue.size() >= 1)
                    readyQueue.push({priorityLevel + 1, processIndex});
                else
                    readyQueue.push({priorityLevel, processIndex});
            }
            time = temp - 1;
        }
        while(currentIndex < processCount && extractArrivalTime(processList[currentIndex]) <= time + 1) {
                readyQueue.push({0, currentIndex});
                remainingTime[currentIndex] = extractServiceTime(processList[currentIndex]);
                currentIndex++;
        }
    }
    markWaitingTimes();
}

// Print functions
void printAlgorithm(int index) {
    cout << SCHEDULING_ALGORITHMS[schedulingAlgorithms[index].first - '0'];
    if (schedulingAlgorithms[index].second > 0) {
        cout << "-" << schedulingAlgorithms[index].second;
    }
    cout << endl;
}

void printProcesses() {
    cout << "Process    ";
    for (int i = 0; i < processCount; ++i) {
        cout << "|  " << extractProcessName(processList[i]) << "  ";
    }
    cout << "|" << endl;
}

void printArrivalTime()
{
    cout << "Arrival    ";
    for (int i = 0; i < processCount; i++)
        printf("|%3d  ",extractArrivalTime(processList[i]));
    cout<<"|\n";
}
void printServiceTime()
{
    cout << "Service    |";
    for (int i = 0; i < processCount; i++)
        printf("%3d  |",extractServiceTime(processList[i]));
    cout << " Mean|\n";
}
void printFinishTime()
{
    cout << "Finish     ";
    for (int i = 0; i < processCount; i++)
        printf("|%3d  ",completionTime[i]);
    cout << "|-----|\n";
}
void printTurnAroundTime()
{
    cout << "Turnaround |";
    int sum = 0;
    for (int i = 0; i < processCount; i++)
    {
        printf("%3d  |",turnaroundTime[i]);
        sum += turnaroundTime[i];
    }
    if((1.0 * sum / turnaroundTime.size())>=10)
        printf("%2.2f|\n",(1.0 * sum / turnaroundTime.size()));
    else
        printf(" %2.2f|\n",(1.0 * sum / turnaroundTime.size()));
}

void printNormTurn()
{
    cout << "NormTurn   |";
    float sum = 0;
    for (int i = 0; i < processCount; i++)
    {
        if( normalizedTurnaround[i]>=10 )
            printf("%2.2f|",normalizedTurnaround[i]);
        else
            printf(" %2.2f|",normalizedTurnaround[i]);
        sum += normalizedTurnaround[i];
    }

    if( (1.0 * sum / normalizedTurnaround.size()) >=10 )
        printf("%2.2f|\n",(1.0 * sum / normalizedTurnaround.size()));
    else
        printf(" %2.2f|\n",(1.0 * sum / normalizedTurnaround.size()));
}

void printStats(int index) {
    printAlgorithm(index);
    printProcesses();
    printArrivalTime();
    printServiceTime();
    printFinishTime();
    printTurnAroundTime();
    printNormTurn();
}

void printTimeline(int index) {
    for (int i = 0; i <= lastMoment; i++)
        cout << i % 10<<" ";
    cout <<"\n";
    cout << "------------------------------------------------\n";
    for (int i = 0; i < processCount; i++)
    {
        cout << extractProcessName(processList[i]) << "     |";
        for (int j = 0; j < lastMoment; j++)
        {
            cout << executionTimeline[j][i]<<"|";
        }
        cout << " \n";
    }
    cout << "------------------------------------------------\n";
}

void executeSchedulingAlgorithm(char algorithmId, int quantum, const string& mode) {
    switch (algorithmId)
    {
    case '1':
        if(mode == TRACE_MODE)cout<<"FCFS  ";
        runFCFS();
        break;
    case '2':
        if(mode == TRACE_MODE) cout << "RR-" << quantum << "  ";
        runRoundRobin(quantum);
        break;
    case '3':
        if(mode == TRACE_MODE) cout << "SPN   ";
        runSPN();
        break;
    case '4':
        if(mode == TRACE_MODE) cout << "SRT   ";
        runSRT();
        break;
    case '5':
        if(mode == TRACE_MODE) cout << "HRRN  ";
        runHRRN();
        break;
    case '6':
        if(mode == TRACE_MODE) cout << "FB-1  ";
        runFeedbackQ1();
        break;
    case '7':
        if(mode == TRACE_MODE) cout << "FB-2i ";
        runFeedbackQ2i();
        break;
    case '8':
        if(mode == TRACE_MODE) cout << "Aging ";
        runAging(quantum);
        break;
    default:
        cerr << "Invalid algorithm ID!" << endl;
        break;
    }
}

int main() {
    parseInputData();
    for (int i = 0; i < schedulingAlgorithms.size(); ++i) {
        resetTimeline();
        executeSchedulingAlgorithm(schedulingAlgorithms[i].first, schedulingAlgorithms[i].second, operationMode);
    
        if (operationMode == TRACE_MODE) {
            printTimeline(i);
        } else if (operationMode == STATS_MODE) {
            printStats(i);
        }

        cout << endl;
    }
    return 0;
}