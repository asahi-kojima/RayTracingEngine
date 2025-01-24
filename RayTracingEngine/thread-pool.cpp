#include "thread-pool.h"

void WorkerThread::run()
{
	//�S�^�X�N���ғ����n�߂�܂Œ�~���Ă���


	while (true)
	{
		//�^�X�N������Ă����Ƃ̓N���e�B�J��
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

		//�^�X�N���p�ӂ��ꂽ���ƂɂȂ�̂Ŏ��s���s���B
		//�����͔r������͂���Ȃ�
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
