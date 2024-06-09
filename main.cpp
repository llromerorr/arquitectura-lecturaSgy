#include "libs/headers.h"
#include <thread> // Para usar this_thread::sleep_for
#include <chrono> // Para usar chrono::seconds
#include <fstream>

using namespace std;

void DisplayMainHeader(char *dataPointer);
void ShowTraceDetails(char *dataPointer, BinaryHeader &traceDetails);
void DisplayBinaryHeader(char *dataPointer, BinaryHeader &traceDetails);
void DisplayTraceHeader(char *traceInfo, int traceNumber);

fstream file("out.txt", ios::out | ios::app);

int main()
{
    // variable para archivo de salida

    int fileID = open("data/file.sgy", O_RDONLY);
    if (fileID < 0)
    {
        perror("Error opening file");
        return 1;
    }

    struct stat fileInfo;
    fstat(fileID, &fileInfo);
    cout << "File ID: " << fileID << endl;
    cout << "File size: " << fileInfo.st_size / 1024 << "KBytes" << endl;
    file << "File ID: " << fileID << endl;
    file << "File size: " << fileInfo.st_size / 1024 << "KBytes" << endl;
    this_thread::sleep_for(chrono::seconds(1));

    char *dataPointer = (char *)mmap(NULL, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fileID, 0);
    if (dataPointer == MAP_FAILED)
    {
        perror("Error mapping file");
        return 1;
    }

    cout << "\n--- Main Header ---\n";
    file << "\n--- Main Header ---\n";
    DisplayMainHeader(dataPointer);
    this_thread::sleep_for(chrono::seconds(1));

    BinaryHeader traceDetails;
    DisplayBinaryHeader(dataPointer, traceDetails);
    this_thread::sleep_for(chrono::seconds(1));

    cout << "\n--- Trace Details ---\n";
    file << "\n--- Trace Details ---\n";
    ShowTraceDetails(dataPointer, traceDetails);
}

void DisplayMainHeader(char *dataPointer)
{
    char mainHeader[3200];
    memcpy(mainHeader, dataPointer, 3200);
    convertASCII(mainHeader, 3200);
    cout << "Data from the main header of 3200 bytes:\n";
    cout << mainHeader;
    file << "Data from the main header of 3200 bytes:\n";
    file << mainHeader;
}

void DisplayBinaryHeader(char *dataPointer, BinaryHeader &traceDetails)
{
    char binaryHeaderInfo[400];
    memcpy(binaryHeaderInfo, dataPointer + 3200, 400);
    traceDetails.store(binaryHeaderInfo);
    cout << "\n--- Binary Header ---\n";
    cout << "Data from the binary header of 400 bytes with information to work with the traces:\n";
    cout << traceDetails;
    file << "\n--- Binary Header ---\n";
    file << "Data from the binary header of 400 bytes with information to work with the traces:\n";
    file << traceDetails;
    
}

void ShowTraceDetails(char *dataPointer, BinaryHeader &traceDetails)
{
    int sampleCount = traceDetails.get_num_of_samples();
    int totalTraceCount = traceDetails.get_num_of_trace();
    cout << "Number of traces: " << totalTraceCount << endl;
    cout << "Number of samples:" << sampleCount << endl;
    file << "Number of traces: " << totalTraceCount << endl;
    file << "Number of samples:" << sampleCount << endl;

    int offset = 3600;
    for (size_t i = 0; i < totalTraceCount; i++)
    {
        char *traceInfo = new char[240];
        memcpy(traceInfo, dataPointer + offset, 240);
        DisplayTraceHeader(traceInfo, i + 1);
        offset = offset + sampleCount * 4 + 240;
        delete[] traceInfo;
        // this_thread::sleep_for(chrono::seconds(0.5));
    }
}

void DisplayTraceHeader(char *traceInfo, int traceNumber)
{
    TraceHeader trace;
    trace.store(traceInfo);
    file << "\n--- Trace " << traceNumber << " ---\n";
    file << trace;    

    float sampleValue;
    memcpy(&sampleValue, traceInfo + 248, 4);
    file << "Sample: " << sampleValue << endl;
}