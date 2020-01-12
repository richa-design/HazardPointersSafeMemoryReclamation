#ifndef LOCKFREE_STACK_H
#define LOCKFREE_STACK_H

#include "common.h"
#include "node.h"

#ifdef	Enable_Hazard_Pointer
#include "HazardPointer.h"
#endif

class LockFreeStack {
public:
	std::atomic<Node*> top;

#ifdef Enable_Hazard_Pointer
	HazardPointer* hp;
#endif
#ifdef	Enable_Hazard_Pointer
	LockFreeStack(HazardPointer* hp) {
		top.store(new Node(NULL));
		this->hp = hp;
	}
#else
	LockFreeStack() {
		top.store(new Node(NULL));
	}
#endif
	
	void push(int data) {

		Node* node = new Node(data);
		Node* oldTop = NULL;
		bool success = false;
		while (!success) {
			oldTop = top.load();
			node->nextNode.store(oldTop);
			success = top.compare_exchange_strong(oldTop, node);
		}
	}

	int pop() {

		Node* oldTop = NULL;
		Node* newTop = NULL;
		bool success = false;
		int data = 0;

		while (!success) {
			oldTop = top.load();
#ifdef	Enable_Hazard_Pointer
			hp->myhprec->HP[0] = oldTop;
			std::atomic_thread_fence(std::memory_order_release);
			if (top.load() != oldTop)
				continue;
#endif
			newTop = oldTop->nextNode.load();
			success = top.compare_exchange_strong(oldTop, newTop);
		}
		data = oldTop->data;

#ifdef	Enable_Hazard_Pointer
		hp->RetireNode(oldTop);
#endif

		return data;
	}
};

#endif /* LOCKFREE_STACK_H */