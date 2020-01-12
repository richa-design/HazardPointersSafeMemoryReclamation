#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <string>

#include "../inc/TestThread.h"
#include "../inc/LockFreeQueue.h"
#include "../inc/LockFreeStack.h"
#include "../inc/HazardPointer.h"
#include "../inc/common.h"

using namespace std;


int main(int argc, char** argv) {
	if (argc != 3) {
		std::cout << "Command line parameters are not correct !!" << std::endl;
		return -1;
	}

	const int threadCount = stoi(argv[1]);
	const int totalIters = stoi(argv[2]);
	

	int iters = totalIters / threadCount;
	thread threadArr[threadCount];
	std::vector<TestThread*> threads;
	long totalTimeElapsed = 0;
	long maxTime = 0;

#ifdef	Enable_Hazard_Pointer
	HazardPointer* hp = new HazardPointer(threadCount);
#ifdef ENABLE_STACK_OPER
	LockFreeStack* obj = new LockFreeStack(hp);
#else
	LockFreeQueue* obj = new LockFreeQueue(hp);
#endif
#else
#ifdef ENABLE_STACK_OPER
	LockFreeStack* obj = new LockFreeStack();
#else
	LockFreeQueue* obj = new LockFreeQueue();
#endif
#endif

	

	for (int thread = 0; thread < threadCount; thread++) {
#ifdef	Enable_Hazard_Pointer
		threads.push_back(new TestThread(iters, obj, hp));
#else
		threads.push_back(new TestThread(iters, obj));
#endif
		threadArr[thread] = std::thread(&TestThread::run, threads[thread]);
	}


	for (int thread = 0; thread < threadCount; thread++) {
		threadArr[thread].join();
		std::cout << (threads[thread])->getEnqueueCounts() << std::endl;
		std::cout << (threads[thread])->getDequeueCounts() << std::endl;
		//totalTimeElapsed += (threads[thread])->getElapsedTime();
		maxTime = std::max(maxTime, (threads[thread])->getElapsedTime());
	}

#ifdef	Enable_Hazard_Pointer
	std::cout << hp->delCount.load() << std::endl;
#endif
	float ops = ((iters * threadCount) / maxTime)*1000;
	std::cout << ops << std::endl;
	cin.clear();
	cin.get();
	
	return 0;
}
