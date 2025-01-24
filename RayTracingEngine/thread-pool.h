#pragma once
#include <condition_variable>
#include <functional>
#include <deque>
#include <iostream>
#include "typeinfo.h"
#include <cassert>


/*
|-----|  |-----|  |-----|  |-----|  |-----| ...
|     |  |     |  |     |  |     |  |     | ...
|     |  |     |  |     |  |     |  |     | ...
|     |  |     |  |     |  |     |  |     | ...
|-----|  |-----|  |-----|  |-----|  |-----| ...




|======================|
|                      |
|                      |
|                      |
|======================|


*/



class ThreadPool;

class WorkerThread
{
public:
	WorkerThread(std::mutex& mutex, /*std::shared_ptr<ThreadPool>*/ThreadPool* manager, const u32 thread_id, std::shared_ptr<std::condition_variable> cv)
		: mMutex(mutex)
		, mManager(manager)
		, mThreadId(thread_id)
		, mTaskAddCondition(cv)
	{
		mThread = std::thread(&WorkerThread::run, this);
	}


	void run();

	void stop()
	{
		std::cout << mThreadId << " Stop\n";
		mThread.join();
	}


private:
	ThreadPool* mManager = nullptr;
	//std::shared_ptr<ThreadPool> mManager = nullptr;

	std::shared_ptr<std::condition_variable> mTaskAddCondition;
	std::mutex& mMutex;

	std::function<void()> mCurrentTask;
	const u32 mThreadId;

	//�X���b�h
	std::thread mThread;

	bool mIsTerminatRequested = false;
};


class ThreadPool/* : public std::enable_shared_from_this<ThreadPool>*/
{
public:
	ThreadPool()
		:mTaskAddCondition(std::make_shared<std::condition_variable>())
		,mTasks(std::make_shared<std::deque<std::function<void()>>>())
	{
		for (u32 i = 0; i < ThreadCount; i++)
		{
			mWorkers.push_back(std::make_unique<WorkerThread>(mMutex, this, i, mTaskAddCondition));
			//mWorkers.push_back(std::make_unique<WorkerThread>(mMutex, shared_from_this(), i, mTaskAddCondition));
		}
	}
	~ThreadPool()
	{
		std::cout << "Thread Pool destroy" << std::endl;
		{
			std::unique_lock<std::mutex> lock(mMutex);
			isTerminateRequested = true;
			mTaskAddCondition->notify_all();
		}


		for (auto& worker : mWorkers)
		{
			worker->stop();
		}
	}

	std::function<void()> pull_task()
	{
		//�^�X�N���󂩂ǂ����͌Ăяo�����������߂Ă���̂�
		if (mTasks->empty())
		{
			assert(0);
		}

		std::function<void()> next_task = mTasks->front();
		mTasks->pop_front();

		return next_task;
	}

	bool push_task(std::function<void()>&& task)
	{
		std::unique_lock<std::mutex> lock(mMutex);

		if (isTerminateRequested)
		{
			std::cout << "Thread Pool Already stoped\n";
			return false;
		}

		//�^�X�N�̒ǉ�
		mTasks->push_back(std::move(task));

		//���̏����ϐ��őҋ@���̃X���b�h���N�����B
		mTaskAddCondition->notify_one();

		return true;
	}

	bool check_terminate_condition() const { return isTerminateRequested; }

	bool is_task_empty() const { return mTasks->empty(); }

	void consume_all_tasks();

private:
	//Worker�X���b�h�̕��X
	std::vector<std::unique_ptr<WorkerThread> > mWorkers;
	//WorkerThread mWorkers[ThreadCount];

	static const u32 ThreadCount = 8;

	//���b�N�ϐ�
	std::mutex mMutex;

	//�����ϐ��i���N���E�ҋ@�̊Ǘ��j
	//std::condition_variable mTaskAddCondition;
	std::shared_ptr<std::condition_variable> mTaskAddCondition;

	//�^�X�N�̃L���[
	std::shared_ptr<std::deque<std::function<void()> > > mTasks;

	//
	bool isTerminateRequested = false;
};
