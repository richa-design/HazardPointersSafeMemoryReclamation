#pragma once
#include <atomic>
#include <chrono>
#include "LockFreeQueue.h"
#include "LockFreeStack.h"
#include "common.h"



class TestThread
{
public:
#ifdef ENABLE_STACK_OPER
	LockFreeStack* stack;
#else
	LockFreeQueue* queue;
#endif
#ifdef	Enable_Hazard_Pointer
	HazardPointer* hp;
#endif
	static int ID_GEN;
	std::atomic<long> enqueueCounts;
	std::atomic<long> dequeueCounts;
	int iter;
	int id;
	std::chrono::milliseconds duration;
	

#ifdef	Enable_Hazard_Pointer
#ifdef ENABLE_STACK_OPER
	TestThread(int iter, LockFreeStack* stack, HazardPointer* hp)
	{
#else
	TestThread(int iter, LockFreeQueue* queue, HazardPointer* hp)
	{
#endif
		id = ID_GEN++;
		this->iter = iter;
#ifdef ENABLE_STACK_OPER
		this->stack = stack;
#else
		this->queue = queue;
#endif
		this->hp = hp;
		duration = std::chrono::milliseconds(0);
	}
#else
#ifdef ENABLE_STACK_OPER
	TestThread(int iter, LockFreeStack* stack)
	{
#else
	TestThread(int iter, LockFreeQueue* queue)
	{
#endif
		id = ID_GEN++;
		this->iter = iter;
#ifdef ENABLE_STACK_OPER
		this->stack = stack;
#else
		this->queue = queue;
#endif
		duration = std::chrono::milliseconds(0);
	}
#endif

	void run()
	{
		auto start = std::chrono::high_resolution_clock::now();

#ifdef	Enable_Hazard_Pointer
		hp->AllocateHPRec();
#endif

		//std::cout << "Allocate" << hp->myhprec << " " << "ThreadID" << id << std::endl;

		for (int i = 0; i < iter; i++)
		{
			if(i%2 == 0) {
#ifdef ENABLE_STACK_OPER
				this->stack->push(i);
#else
				this->queue->enq(i);
#endif
				enqueueCounts.fetch_add(1, std::memory_order_relaxed);
			}
			else {
#ifdef ENABLE_STACK_OPER
				this->stack->pop();
#else
				this->queue->deq();
#endif
				dequeueCounts.fetch_add(1, std::memory_order_relaxed);
			}
		}
#ifdef	Enable_Hazard_Pointer
		hp->RetireHPRec();	
#endif

		auto end = std::chrono::high_resolution_clock::now();
		this->duration = (std::chrono::duration_cast<std::chrono::milliseconds>(end - start));

	}

	int getThreadID() {
		return id;
	}

	long getEnqueueCounts() {
		return enqueueCounts.load();
	}
	long getDequeueCounts() {
		return dequeueCounts.load();
	}
	long getElapsedTime() {
		return duration.count();
	}
};

int TestThread::ID_GEN = 0;
