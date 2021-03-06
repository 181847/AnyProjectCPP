﻿// DevelopSuitAndCase.cpp: 定义控制台应用程序的入口点。
//

#include <MyTools/TestTool/Suit.h>
#include <future>
#include <thread>
#include <queue>
#include <ThreadPool/ThreadPool.h>

class FutureSuit;

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

    bool IsRunning() const
    {
        return m_isRunning;
    }

public:
    std::shared_future<void> m_futureResult;

    /*!
        \brief memorize whether this case has been read nor not,
        this member has not functionality, just a usable mark for development,
        this is up to you that set its status.
    */
    bool m_hasBeenRead = false;

protected:
    /*!
        \brief mark whether this case is under testing.
        this variable will be changed by FutureSuit.
    */
    volatile bool m_isRunning = false;

    /*!
        \brief let FutureSuit access to those sensitive data.
    */
    friend class FutureSuit;
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
protected:
    using Super = FutureSuit;

public:
    FutureSuit() : TestSuit::Suit<>(), m_threadPool(20) 
    {
        // once the FutureSuit constructed, it should have started.
        PrepareTotal();
    }
    ~FutureSuit()
    {
        // clear up when this Suit deconstructed.
        FinishAllCases();
    }

    /*!
        \brief Future case do not support ordinary case.
    */
    void AddCase(std::unique_ptr<TestSuit::Case> theCase) = delete;
    void Start() = delete;

    /*!
        \brief add a case and push it into the tasks queue of the thread pool, waiting for starting when available.
    */
    virtual void AddFutureCase(std::shared_ptr<FutureCase> theFutureCase)
    {
        theFutureCase->m_futureResult = m_threadPool.enqueue(
            [theFutureCase, this] {
                {
                    std::lock_guard<std::mutex> lk(m_CaseHelpWorkMutex);
                    PrepareBeforeEachCase(theFutureCase.get());
                }
                theFutureCase->Run(); 
                {
                    std::lock_guard<std::mutex> lk(m_CaseHelpWorkMutex);
                    FinishEachCase(theFutureCase.get());
                }
            }
        ).share();
        m_futureCases.push_back(theFutureCase);
    }

    /*!
        \brief start case mark it as running.
    */
    virtual void PrepareBeforeEachCase(TestSuit::Case * pTheCase) override
    {
        FutureCase* futCase = dynamic_cast<FutureCase*>(pTheCase);
        if ( ! futCase)
        {
            assert(false && "");
        }
        futCase->m_isRunning = true;
    }

    /*!
        \brief finish case and mark it as stopping.
    */
    virtual void FinishEachCase(TestSuit::Case * pTheCase) override
    {
        FutureCase* futCase = dynamic_cast<FutureCase*>(pTheCase);
        if (!futCase)
        {
            assert(false && "");
        }
        futCase->m_isRunning = false;
    }

public:
    /*!
        \brief thread pool to run cases.
    */
    ThreadPool m_threadPool;

    /*!
        \brief mutex for works before and after each case.
    */
    std::mutex m_CaseHelpWorkMutex;

    /*!
        \brief store all the cases.
    */
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
