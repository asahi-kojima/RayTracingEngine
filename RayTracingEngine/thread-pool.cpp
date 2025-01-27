#include <string>
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

	const std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	const s32 TotalTasks = mTasks->size();

	f32 x_sum = 0.0f;
	f32 y_sum = 0.0f;
	f32 x2_sum = 0.0f;
	f32 xy_sum = 0.0f;
	s32 count = 1;

	while (!isEmpty)
	{
		isEmpty = mTasks->empty();
		s32 res_tasks = mTasks->size();


		if (res_tasks % 10000 == 0)
		{
			auto expected_time = (res_tasks * 1.0f / (TotalTasks - res_tasks)) * std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count();

			const f32 x = res_tasks;//こなしたタスク
			const f32 y = expected_time;
			x_sum += x;
			y_sum += y;
			x2_sum += x * x;
			xy_sum += x * y;

			const f32 A = (count * xy_sum - x_sum * y_sum) / (count * x2_sum - x_sum * x_sum);
			const f32 B = (x2_sum * y_sum - xy_sum * x_sum) / (count * x2_sum - x_sum * x_sum);
			const s32 true_expected_time = static_cast<s32>(A * res_tasks + B);

			count++;
			count++;

			std::cout << res_tasks << " : " << (true_expected_time > 0 ? std::to_string(true_expected_time) : "Undecided") << "\n";
		}
	}
}
