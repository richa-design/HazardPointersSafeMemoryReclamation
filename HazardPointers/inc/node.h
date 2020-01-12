#pragma once
#include <memory>
#include <atomic>
#include <list> 
#include "common.h"


class Node
{
public:
	int data;
	std::atomic<Node*> nextNode;

	Node(int data) {
		this->data = data;
		nextNode.store(NULL, std::memory_order_relaxed);
	}
};

#ifdef	Enable_Hazard_Pointer
class HPRecType
{
public:
	Node* HP[HAZARD_POINTER];
	std::atomic<HPRecType*> Next;
	std::atomic<bool> active;
	std::list<Node*> rlist;
	int rcount;

	HPRecType()
	{
		HP[0] = NULL;
		HP[1] = NULL;
		Next = NULL;
		active = ATOMIC_VAR_INIT(true);
		rcount = 0;
	}
};
#endif