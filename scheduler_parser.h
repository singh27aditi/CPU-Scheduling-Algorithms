#ifndef SCHEDULER_PARSER_H
#define SCHEDULER_PARSER_H

#include <bits/stdc++.h>

using namespace std;

/** Header for parsing and storing global scheduling variables **/

// Operation modes and global variables
string operationMode; // "trace" or "stats"
int lastMoment;       // Last simulation time
int processCount;     // Number of processes

// Scheduling algorithms and process data
vector<pair<char, int>> schedulingAlgorithms; // Algorithm ID and quantum time (if applicable)
vector<tuple<string, int, int>> processList;  // For algorithms 1-7: process name, arrival time, service time
                                             // For algorithm 8: process name, arrival time, priority
vector<vector<char>> executionTimeline;      // Execution timeline for all processes
unordered_map<string, int> processIndexMap;  // Map from process name to its index

// Results for each process
vector<int> completionTime;
vector<int> turnaroundTime;
vector<float> normalizedTurnaround;

/*
 * Parse the scheduling algorithms from the input string.
 * Each algorithm is represented by a number (and optionally a quantum value).
 * For example: "2-4" means Round Robin with quantum=4.
 */

void parseAlgorithms(const string& algorithmData) {
    stringstream algorithmStream(algorithmData);  

    while (algorithmStream.good()) {  
        string algorithmChunk;
        if (!getline(algorithmStream, algorithmChunk, ',')) break;  // Extract substring until ','

        stringstream chunkStream(algorithmChunk);  

        string idPart, quantumPart;
        if (!getline(chunkStream, idPart, '-')) continue;  // Extract algorithm ID part
        char algorithmId = idPart[0];  

        int quantum = -1;  // Default value
        if (getline(chunkStream, quantumPart, '-') && !quantumPart.empty()) {
            quantum = stoi(quantumPart);
        }

        schedulingAlgorithms.push_back({algorithmId, quantum});  
    }
}


void parseProcesses()
{
    string processChunk, processName;
    int processArrivalTime, processServiceTime;
    for(int i = 0; i < processCount; i++)
    {
        cin >> processChunk;

        stringstream stream(processChunk);
        string temp;
        getline(stream, temp, ',');
        processName = temp;
        getline(stream, temp, ',');
        processArrivalTime = stoi(temp);
        getline(stream, temp, ',');
        processServiceTime = stoi(temp);

        processList.push_back(make_tuple(processName, processArrivalTime, processServiceTime) );
        processIndexMap[processName] = i;
    }
}

/**
 * Parse the entire input based on the specified format.
 */
void parseInputData() {
    string algorithmData;

    // Read operation mode, algorithm data, last simulation time, and process count
    cin >> operationMode;
    cin >> algorithmData;
    cin >> lastMoment;
    cin >> processCount;

    // Parse algorithms and processes
    parseAlgorithms(algorithmData);

    parseProcesses();

    // Initialize result vectors and timeline matrix
    completionTime.resize(processCount);
    turnaroundTime.resize(processCount);
    normalizedTurnaround.resize(processCount);

    executionTimeline.resize(lastMoment, vector<char>(processCount, ' '));
}

#endif // SCHEDULER_PARSER_H
