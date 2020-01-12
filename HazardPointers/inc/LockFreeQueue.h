#pragma once
#include <atomic>
#include <memory>
#include <iostream>
#include <thread>
#include "node.h"
#include "common.h"

#ifdef	Enable_Hazard_Pointer
#include "HazardPointer.h"
#endif
#include "TestThread.h"

class LockFreeQueue
{

public:
	std::atomic<Node*> head;
	std::atomic<Node*> tail;
#ifdef	Enable_Hazard_Pointer
	HazardPointer* hp;
#endif

#ifdef	Enable_Hazard_Pointer
	LockFreeQueue(HazardPointer* hp)
	{
		Node* sentinel = new Node(NULL);
		head.store(sentinel);
		tail.store(sentinel);
		this->hp = hp;
	}
#else
	LockFreeQueue()
	{
		Node* sentinel = new Node(NULL);
		head.store(sentinel);
		tail.store(sentinel);
	}
#endif


	void enq(int data)
	{
		//std::cout << "Enqueue entered!" << std::endl;
		Node* node = new Node(data);
		while (true) {
			Node* last = tail.load();
#ifdef	Enable_Hazard_Pointer
			hp->myhprec->HP[0] = last;
			std::atomic_thread_fence(std::memory_order_release);
			if (tail.load() != last)
				continue;
#endif
			Node* next = (last->nextNode).load();
			if (last == tail.load()) {
				if (next == NULL) {
					if (((tail.load())->nextNode).compare_exchange_weak(next, node)) {
						tail.compare_exchange_weak(last, node);
						return;
					}
				} else {
					tail.compare_exchange_weak(last, next);
				}
			}
		}
	}

	int deq() {
		while (true) {
			Node* first = head.load();
#ifdef	Enable_Hazard_Pointer
			hp->myhprec->HP[0] = first;
			std::atomic_thread_fence(std::memory_order_release);
			if (head.load() != first)
				continue;
#endif
			Node* last = tail.load();
			Node* next = (first->nextNode).load();
#ifdef	Enable_Hazard_Pointer
			hp->myhprec->HP[1] = next;
			std::atomic_thread_fence(std::memory_order_release);
#endif
			if (head.load() != first)
				continue;
			if (first == last) {
				if (next == NULL) {
					throw "Empty Exception."; 
				}
				tail.compare_exchange_weak(last, next);
			} else {
				int value = next->data;
				if (head.compare_exchange_weak(first, next)) {
#ifdef	Enable_Hazard_Pointer
					hp->RetireNode(first);
#endif
					return value;
				}

			}
		}
	}
};

