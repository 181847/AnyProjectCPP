// DevelopSuitAndCase.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <MyTools/TestTool/Suit.h>
#include <future>
#include <thread>

template<typename FUTURE_RESULT>
class FutureCase : public TestSuit::Case
{
protected:
    using Super = FutureCase<FUTURE_RESULT>;

public:
    FutureCase(const std::string& name) : TestSuit::Case(name) {}

    virtual void Run() override
    {
        // empty
    }

    virtual FUTURE_RESULT AsyncRun() { return FUTURE_RESULT(); }

    std::packaged_task<FUTURE_RESULT()> GetTask()
    {
        return std::packaged_task<FUTURE_RESULT()>([this]()->FUTURE_RESULT {return this->AsyncRun(); });
    }

};

class IntCase : public FutureCase<int>
{
public:
    IntCase(const std::string& name) :Super(name) {}

    virtual int AsyncRun() override
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return 25;
    }
};

/*!
    \brief the Suit run cases in multiple threads and get result asynchronously
*/
template<typename FUTURE_RESULT>
class FutureSuit : public TestSuit::Suit<>
{
public:
    virtual void AddCase(std::unique_ptr<TestSuit::Case> theCase) = delete;

    virtual void AddFutureCase(std::unique_ptr<FutureCase<FUTURE_RESULT>> theFutureCase)
    {
        m_taskes.push_back(theFutureCase->GetTask());
        m_cases. push_back(std::move(theFutureCase));
    }

    
    std::vector<std::future<FUTURE_RESULT>> GetFutures()
    {
        std::vector<std::future<FUTURE_RESULT>> retFutures;
        for (auto& task : m_taskes)
        {
            retFutures.push_back(task.get_future());
        }
        return retFutures;
    }

public:
    std::vector<std::thread> m_threadPool;

    std::vector<std::packaged_task<FUTURE_RESULT()>> m_taskes;
};

class NormalCase : public TestSuit::Case
{
public:
    NormalCase() : TestSuit::Case("a normal case") {}

    virtual void Run() override
    {
        printf("Normal case run.\n");
    }
};


int main()
{
    FutureSuit<int> futSuit;
    futSuit.AddFutureCase(std::make_unique<IntCase>("a case in Future suit"));

    auto futures = futSuit.GetFutures();
    auto& tasks = futSuit.m_taskes;
    std::thread t(std::move(tasks[0]));

    printf("is the task still valid? ans = %d\n", tasks[0].valid());
    
    futures[0].wait();

    printf("the future is still valid? ans = %d\n", futures[0].valid());
    printf("the case has finished, returned value is %d.\n", futures[0].get());
    printf("afther get(), the future is still valid? ans = %d\n", futures[0].valid());

    t.join();
    
    getchar();
    
    return 0;
}

