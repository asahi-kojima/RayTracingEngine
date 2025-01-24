#include "thread-pool.h"

void WorkerThread::run()
{
	//全タスクが稼働し始めるまで停止しておく


	while (true)
	{
		//タスクを取ってくる作業はクリティカル
		{
			std::unique_lock<std::mutex> lock(mMutex);

			if (mManager->check_terminate_condition())
			{
				break;
			}

			mTaskAddCondition->wait(lock, [&]() {return !mManager->is_task_empty() || mManager->check_terminate_condition(); });

			if (mManager->check_terminate_condition())
			{
				std::cout << mThreadId << " running finish" << std::endl;
				break;
			}


			mCurrentTask = mManager->pull_task();
		}

		//タスクが用意されたことになるので実行を行う。
		//ここは排他制御はいらない
		mCurrentTask();
	}

	std::cout << mThreadId << " running finish" << std::endl;
}

void ThreadPool::consume_all_tasks()
{
	volatile bool isEmpty = false;
	while (!isEmpty)
	{
		isEmpty = mTasks->empty();
	}
	std::cout << mTasks->size() << std::endl;
}
