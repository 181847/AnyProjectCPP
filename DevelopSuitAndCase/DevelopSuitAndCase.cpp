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
        return 25;
    }
};


int main()
{
    IntCase intcase("a case return a int");

    auto task = intcase.GetTask();


    std::future<int> fut = task.get_future();

    std::thread t(std::move(task));
    
    fut.wait();

    printf("the case has finished, returned value is %d.\n", fut.get());

    t.join();
    
    getchar();
    
    return 0;
}

