#include "CommonPool.hpp"
#include <iostream>
#include <thread>

class Foo
{
public:
    Foo() : mIdx(-1) {};
    Foo(int idx) : mIdx(idx) {}
    ~Foo()
    {
        std::cout << "deconstruct:\t" << mIdx << std::endl;
    }
public:
    int mIdx;
};

void test1()    // create from list
{
    std::list<Foo*> instances;
    for (int idx = 0; idx < 100; ++idx)
    {
        instances.push_back(new Foo(idx));
    }
    auto& pool = CommonPool<Foo>::CreatePoolFromInstances(instances);
}

void test2()    // create
{
    auto& pool = CommonPool<Foo>::CreatePool(100);
}

void test3()    // acquire/release
{
    std::list<Foo*> instances;
    for (int idx = 0; idx < 100; ++idx)
    {
        instances.push_back(new Foo(idx));
    }
    auto& pool = CommonPool<Foo>::CreatePoolFromInstances(instances);

    for (int idx = 0; idx < 101; ++idx)
    {
        auto instance = pool->acquireInstance();
        std::cout << instance->get()->mIdx << std::endl;
        instance->releaseInstance();
    }
}

void test4()
{
    std::list<Foo*> instances;
    for (int idx = 0; idx < 100; ++idx)
    {
        instances.push_back(new Foo(idx));
    }
    auto& pool = CommonPool<Foo>::CreatePoolFromInstances(instances);

    for (int idx = 0; idx < 200; ++idx)
    {
        std::shared_ptr<CommonInstance<Foo>> instance;
        if(!pool->tryAcquireInstance(instance, 20))
        {
            std::cout << "get instance failed\t" << idx << std::endl;
        } else
        {
            std::cout << "get instance:\t" << instance->get()->mIdx << std::endl;
        }
    }
}

void test5()
{
    std::list<Foo*> instances;
    for (int idx = 0; idx < 100; ++idx)
    {
        instances.push_back(new Foo(idx));
    }
    auto& pool = CommonPool<Foo>::CreatePoolFromInstances(instances);

    std::thread t1([&](){
        for (int idx = 0; idx < 100; ++idx)
        {
            auto p = pool->acquireInstance();
            std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(5)));
            p->releaseInstance();
        }
    });

    std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(200)));
    std::thread t2([&](){
        for (int idx = 0; idx < 200; ++idx)
        {
            std::shared_ptr<CommonInstance<Foo>> instance;
            if(!pool->tryAcquireInstance(instance, 20))
            {
                std::cout << "get instance failed\t" << idx << std::endl;
                std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(100)));
            } else
            {
                std::cout << "get instance:\t" << instance->get()->mIdx << std::endl;
            }
        }
    });
    t1.join();
    t2.join();

}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
}