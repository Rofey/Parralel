#include <iostream>       
#include <thread>         
#include <chrono> 
#include <mutex>
#include <windows.h>

using namespace std;
using namespace chrono;

std::mutex mtx, mtx2, mtx3;
std::condition_variable cv;
int threadCount = 0;									// Keeping track of all the threads that are successfully completed.
int totalThreadsCompleted = 0;							// Keeping a track of all the Threads that have been Initialized. 
int TOTAL_THREADS = 10;									// Change this value to the number of threads you want in this program.

int ack(int m, int n)
{
	//cout << "m: " << m << "n: " << n << endl;
	if (m == 0) return n + 1;
	if (n == 0) return ack(m - 1, 1);
	return ack(m - 1, ack(m, n - 1));
}

void function(int i) {
	totalThreadsCompleted++;
	mtx.lock();											// Same mutex will be applied on same resources.
	threadCount++;										// Same mtx.lock on threadCount --.
	mtx.unlock();

	auto start = high_resolution_clock::now();
	int temp = ack(3, 8);
	mtx3.lock();
	cout << i << " " << temp << endl;
	mtx3.unlock();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	mtx.lock();
	threadCount--;
	mtx.unlock();
	cv.notify_one();
}

int main() {

	std::cout << "Spawning and detaching 3 threads...\n";
	auto start2 = high_resolution_clock::now();

	for (int i = 0; i < TOTAL_THREADS; i++)
	{
		std::thread threadVar(function, i);	
		threadVar.detach();
			
		//mtx.lock();								Already have a lock with unique lock.
		//threadCount++;							Commenting this out because if main makes the thread, starts working and might suddenly crash,
		//mtx.unlock();								it will still keep the thread count increment, right?
	}
	
	//cout << thread::hardware_concurrency() << endl;					Will return the number of threads
	
	std::unique_lock<mutex> ul(mtx2);
	cv.wait(ul, [] { return (threadCount == 0 && totalThreadsCompleted == TOTAL_THREADS) ? true : false; });
	// cv.wait will keep the thread locked, when it returns false,
	// Only unlocks when true. It keeps checking until it reaches a true condition


	auto stop2 = high_resolution_clock::now();
	auto completeDuration = duration_cast<microseconds>(stop2 - start2);
	cout << "\nComplete Duration: " << completeDuration.count();

	std::cout << "\nDone spawning threads.\n";

	return 0;
}