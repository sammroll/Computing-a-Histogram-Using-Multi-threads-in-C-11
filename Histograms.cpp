#include<iostream>
#include<thread>
#include<fstream>
#include<atomic>
#include<vector>

using namespace std;

void fileToMemoryTransfer(const char* fileName, char** data, size_t& numOfBytes);
void GlobalHistogram(const char* fileName);
void LocalHistogram(const char* fileName);

int main(int argc, char* argv[]) {
	// Check if the user has provided a file name
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " <filename>" << endl;
		return 1;
	}
	string filename = argv[1];

	GlobalHistogram(filename.c_str());
	LocalHistogram(filename.c_str());

	return 0;
}

void fileToMemoryTransfer(const char* fileName, char** data, size_t& numOfBytes) {
	streampos begin, end;
	ifstream inFile(fileName, ios::in | ios::binary | ios::ate);
	if (!inFile) {
		cerr << "Cannot open " << fileName << endl;
		inFile.close();
		exit(1);

	}

	size_t size = inFile.tellg();
	char* buffer = new char[size];
	inFile.seekg(0, ios::beg);
	inFile.read(buffer, size);
	inFile.close();
	*data = buffer;
	numOfBytes = size;
}


void GlobalHistogram(const char* fileName) {
	atomic<long long int> global_Histogram[256];
	int numthreads = thread::hardware_concurrency();
	string Userfile;
	char* data = nullptr;
	size_t numOfBytes;
	vector<thread> workers;

	fileToMemoryTransfer(fileName, &data, numOfBytes);

	size_t workSize = numOfBytes / numthreads;
	size_t start = 0;
	size_t end = workSize;

	// Loop through all the threads
	for (int t = 0; t < numthreads; t++) {
		// Leaves the remaining number of bytes for the last thread
		if (t == numthreads - 1) {
			end = numOfBytes;
		}

		// Creates each thread and assigns each of them a portion of the data to increment
		//  the global histogram if a certain byte value is found
		workers.push_back(thread([&](size_t start, size_t end) {
			for (size_t i = start; i < end; i++) {
				unsigned char value = static_cast<unsigned char>(data[i]);
				global_Histogram[value]++;
			}
			}, start, end));

		// Move the start and end to the next portion of the data
		start = end;
		end = start + workSize;
	}

	// Wait for all threads to finish
	for (auto& t : workers) {
		t.join();
	}

	cout << "Run with one global histogram\n";
	for (int i = 0; i < 256; i++) {
		cout << i << ": " << global_Histogram[i] << endl;
	}

	// Deallocate memory
	delete[] data;
}

void LocalHistogram(const char* fileName) {
	int numThreads = thread::hardware_concurrency();

	//allocate overall histogram on the stack
	long long int overallHistogram[256] = { 0 };

	//allocate local histogram on the heap
	long long int* localHistogram = new long long int[numThreads * 256]();

	char* data = nullptr;
	size_t numOfBytes;
	vector<thread> workers;

	fileToMemoryTransfer(fileName, &data, numOfBytes);

	size_t workSize = numOfBytes / numThreads;
	size_t start = 0;
	size_t end = workSize;

	// Loop through all the threads
	for (int t = 0; t < numThreads; t++) {

		// Leaves the remaining number of bytes for the last thread
		if (t == numThreads - 1) {
			end = numOfBytes;
		}

		// Creates each thread and assigns each of them a portion of the data to increment
		//  each thread's local histogram if a certain byte value is found
		workers.push_back(thread([&, t](size_t start, size_t end) {
			for (size_t i = start; i < end; i++) {
				unsigned char value = static_cast<unsigned char> (data[i]);
				localHistogram[t * 256 + value]++;
			}
			}, start, end));
		start = end;
		end = start + workSize;
	}

	// Wait for all threads to finish
	for (auto& t : workers) {
		t.join();
	}

	// Add the local histograms to the overall histogram
	for (int t = 0; t < numThreads; t++) {
		for (int j = 0; j < 256; j++) {
			overallHistogram[j] += localHistogram[t * 256 + j];
		}
	}

	cout << "Run with local histograms\n";
	for (int i = 0; i < 256; i++) {
		cout << i << ": " << overallHistogram[i] << endl;
	}

	// Deallocate memory
	delete[] localHistogram;
	delete[] data;
}
