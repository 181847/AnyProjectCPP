// DevelopSuitAndCase.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <MyTools/TestTool/Suit.h>
#include <future>
#include <thread>
#include <queue>
#include <ThreadPool/ThreadPool.h>

class FutureCase : public TestSuit::Case
{
protected:
    using Super = FutureCase;

public:
    FutureCase(const std::string& name) : TestSuit::Case(name) {}

    virtual void Run() override
    {
        // empty
    }

public:
    std::shared_future<void> m_futureResult;
};

class IntCase : public FutureCase
{
public:
    IntCase(const int index, const std::string& name) :Super(name), m_index(index) {}

    virtual void Run() override
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        printf("%d IntCase finished.\n", m_index);
        m_result = 25;
    }

    int m_result;
    int m_index;
};

/*!
    \brief the Suit run cases in multiple threads and get result asynchronously
*/
class FutureSuit : public TestSuit::Suit<>
{
public:
    FutureSuit() : TestSuit::Suit<>(), m_threadPool(20) {}

    virtual void AddCase(std::unique_ptr<TestSuit::Case> theCase) = delete;

    virtual void AddFutureCase(std::shared_ptr<FutureCase> theFutureCase)
    {
        theFutureCase->m_futureResult = m_threadPool.enqueue([theFutureCase] { theFutureCase->Run(); }).share();
        m_futureCases.push_back(theFutureCase);
    }

public:
    ThreadPool m_threadPool;

    std::vector<std::shared_ptr<FutureCase>> m_futureCases;
};

void RunInThreadPool(int threadIndex)
{
    printf("running task with index = %d\n", threadIndex);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    printf("completes running task with index = %d\n", threadIndex);
}

int main()
{
    {
        FutureSuit futSuit;
        for (int i = 0; i < 100; ++i)
        {
            auto theCase = std::make_shared<IntCase>(i, "a case in Future suit");
            futSuit.AddFutureCase(theCase);
        }
        printf("all case has been added.\n");
    }// wait all cases finished.
    

    printf("test a thread pool:\n");
    const int NUM_THREADS_IN_POOL = 3;
    printf("pool size is %d\n", NUM_THREADS_IN_POOL);
    ThreadPool tPool(NUM_THREADS_IN_POOL);
    int countTasks = 0;
    printf("enter to add a new task to the thread pool.\n");
    while (getchar() != EOF)
    {
        printf("new task confirmed.\n");
        tPool.enqueue(RunInThreadPool, countTasks++);
    }
    
    return 0;
}

