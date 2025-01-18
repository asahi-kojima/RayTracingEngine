#pragma once
#include <condition_variable>
#include <functional>
#include <deque>
#include "typeinfo.h"

class WorkerThread
{
public:
	WorkerThread(const u32 thread_id)
		:mThreadId(thread_id)
	{
	
	}

	void run()
	{
		//std::unique_lock<std::mutex> lock(mMutex);
		//mTaskAddCondition.wait(lock);
	}
	void order_task();
private:
	//std::condition_variable& mTaskAddCondition;
	std::function<void()> mCurrentTask;
	const u32 mThreadId;
};


class ThreadPool
{
public:
	ThreadPool()
	{
		for (u32 i = 0; i < ThreadCount; i++)
		{
			mWorkers.push_back(std::make_unique<WorkerThread>(i));
		}
	}
	~ThreadPool()
	{
	}

		void add_task(std::function<void()>&& task)
	{
		std::unique_lock<std::mutex> lock(mMutex);
		mTasks.push_back(task);

		mTaskAddCondition.notify_one();
	}

	void run(std::vector<std::function<void()> > initial_tasks)
	{
		std::thread mManagerThread([]() {});
	}

	void stop(){}



private:
	//Workerスレッド数の設定
	static const size_t ThreadCount = 8;

	//Workerスレッドの方々
	std::vector<std::unique_ptr<WorkerThread> > mWorkers;
	//WorkerThread mWorkers[ThreadCount];

	//ロック変数
	std::mutex mMutex;

	//条件変数（＝起動・待機の管理）
	//std::condition_variable mTaskAddCondition;
	std::condition_variable mTaskAddCondition;

	//タスクのキュー
	std::deque<std::function<void()> > mTasks;

	//
	bool isTerminateRequested = false;
};
