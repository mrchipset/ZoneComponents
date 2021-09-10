#include <ObjectPool.hpp>
#include <iostream>
#include <vector>

class Foo
{
public:
    Foo() 
    {
        std::cout << "Default Constructor." << std::endl;
    }

    Foo(int arg1, float arg2, double arg3)
    {
        std::cout << "Param Constructor: Arg1:\t" << arg1 
        << "\tArg2:\t" << arg2 << "\tArg3:\t" << arg3 << std::endl;
    }

    ~Foo()
    {
        std::cout << "Deconstruct Foo" << std::endl;
    }
};

void test1()    // full
{
    ObjectPool<Foo> pool(128);
    std::vector<std::shared_ptr<Foo>> ptrs;
    for (int i = 0; pool.allocated() < pool.capacity(); ++i)
    {
        auto ptr = pool.construct<Foo>(i, 0.0f, 0.0);
        ptrs.push_back(ptr);
    }
}

void test2()    // over allocated
{
    ObjectPool<Foo> pool(128);
    std::vector<std::shared_ptr<Foo>> ptrs;
    for (int i = 0; i < 256; ++i)
    {
        auto ptr = pool.construct<Foo>(i, 0.0, 0.0);
        ptrs.push_back(ptr);
    }
}

void test3()    // allocated and destroy
{
    ObjectPool<Foo> pool(128);
    std::vector<std::shared_ptr<Foo>> ptrs;
    for (int i = 0; i < 256; ++i)
    {
        auto ptr = pool.construct<Foo>(i, 0.0, 0.0);
    }
}

int main()
{
    test1();
    test2();
    test3();
    return 0;
}