#pragma once
#include <atomic>
#include "node.h"
#include <vector>
#include <algorithm>
#include "common.h"

#ifdef	Enable_Hazard_Pointer


class HazardPointer
{
public:
	thread_local static HPRecType* myhprec;
	std::atomic<HPRecType*> HeadHPRec;
	std::atomic<int> H;
	std::atomic<int> delCount;
	int num_thread;

	HazardPointer(int threadCount)
	{
		HeadHPRec = NULL;
		H = 0;
		delCount.store(0);
		this->num_thread = threadCount;
	}

	void AllocateHPRec()
	{
		HPRecType* hprec=NULL;
		HPRecType* oldhead=NULL;
		bool expected = false;
		int oldcount= 0;

		for (hprec = HeadHPRec; hprec != NULL; hprec = hprec->Next)
		{
			if (hprec->active)
				continue;
			
			if (!((hprec->active).compare_exchange_strong(expected, true)))
			{
				continue;
			}

			myhprec = hprec;
			return;
		}

		do {
			oldcount = H;
		} while (!(H.compare_exchange_strong(oldcount, oldcount + 1)));

		hprec = new HPRecType();

		do {
			oldhead = HeadHPRec;
			hprec->Next = oldhead;
		} while (!(HeadHPRec.compare_exchange_strong(oldhead, hprec)));
		myhprec = hprec;
	}

	void RetireHPRec()
	{
		myhprec->HP[0] = NULL;
		std::atomic_thread_fence(std::memory_order_release);
		myhprec->HP[1] = NULL;
		std::atomic_thread_fence(std::memory_order_release);
		myhprec->active = ATOMIC_VAR_INIT(false);
	}

	void RetireNode(Node* node)
	{
		HPRecType* head = NULL;
		myhprec->rlist.push_back(node);
		myhprec->rcount++;
		std::atomic_thread_fence(std::memory_order_release);
		head = HeadHPRec;
		std::atomic_thread_fence(std::memory_order_acquire);
		if (myhprec->rcount >= 2* num_thread * HAZARD_POINTER)
		{
			Scan(head);
			HelpScan();
		}
	}

	void Scan(HPRecType* head)
	{
		// Stage 1: Scan HP list and insert non-null values into plist
		HPRecType* hprec = NULL;
		Node* hptr = NULL;
		std::list<Node*> tmplist;
		Node* node = NULL;
		Node* ppt = NULL;
		std::vector<Node*> plist;

		hprec = head;
		std::atomic_thread_fence(std::memory_order_acquire);
		while (hprec != NULL)
		{
			for (int i = 0; i < HAZARD_POINTER; i++)
			{
				hptr = hprec->HP[i];
				if (hptr != NULL)
				{
					plist.push_back(hptr);
				}
			}
			hprec = hprec->Next;
		}

		std::sort(plist.begin(), plist.end());

		// Stage 2: Search plist
		tmplist.splice(tmplist.begin(), myhprec->rlist);
		myhprec->rcount = 0;
		node = tmplist.front();
		tmplist.pop_front();
		while (node != NULL)
		{
			if (std::binary_search(plist.begin(), plist.end(), node))
			{
				myhprec->rlist.push_back(node);
				myhprec->rcount++;
				std::atomic_thread_fence(std::memory_order_release);
			}
			else
			{

				delete node;
				delCount.fetch_add(1, std::memory_order_relaxed);
			}
			if (!tmplist.empty())
			{
				node = tmplist.front();
				tmplist.pop_front();
			}
			else
			{
				node = NULL;
			}
		}
		while (!plist.empty()) {
			ppt = plist.back();
			plist.pop_back();
		}
	}

	void HelpScan()
	{
		bool expected = false;
		Node* node = NULL;
		HPRecType* hprec = NULL;
		HPRecType* head = NULL;

		for (hprec = HeadHPRec; hprec != NULL; hprec = hprec->Next)
		{
			if (hprec->active)
				continue;
			if (!((hprec->active).compare_exchange_strong(expected, true)))
			{
				continue;
			}
			std::atomic_thread_fence(std::memory_order_acquire);
			while (hprec->rcount > 0)
			{
				node = myhprec->rlist.back();
				myhprec->rlist.pop_back();
				hprec->rcount--;
				myhprec->rlist.push_back(node);
				myhprec->rcount++;
				head = HeadHPRec;
				if (myhprec->rcount >= 2 * num_thread * HAZARD_POINTER)
				{
					Scan(head);
				}
				hprec->active = false;
			}

		}
	}
};

thread_local HPRecType* HazardPointer::myhprec = NULL;

#endif