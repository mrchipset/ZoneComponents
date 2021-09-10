#include <LRUCaching.hpp>
#include <string>
#include <iostream>

struct Foo
{
public:
    Foo (std::string str) : content(str) {}
    ~Foo() { std::cout << "Deconstruct:\t" << content << std::endl; }
    std::string content;
};

void test1()
{
    std::cout << "\n----TEST 1----\n" << std::endl;
    LRUCaching<std::string, Foo> cache(2);
    std::cout << "Initialized Size:\t" << cache.size() << std::endl;
    auto foo1 = std::make_shared<Foo>("foo1");
    cache.set(foo1->content, foo1);
    foo1.reset();
    std::cout << "inserted 1 Size:\t" << cache.size() << std::endl;
    auto foo2 = std::make_shared<Foo>("foo2");
    cache.set(foo2->content, foo2);
    foo2.reset();
    std::cout << "inserted 2 Size:\t" << cache.size() << std::endl;
    auto foo3 = std::make_shared<Foo>("foo3");
    cache.set(foo3->content, foo3);
    foo3.reset();
    std::cout << "inserted 3 Size:\t" << cache.size() << std::endl;
    auto foo4 = std::make_shared<Foo>("foo4");
    cache.set(foo4->content, foo4);
    foo4.reset();
    std::cout << cache.get("foo3")->content << std::endl;
}

void test2()
{
    std::cout << "\n----TEST 2----\n" << std::endl;
        LRUCaching<std::string, Foo> cache(4);
    std::cout << "Initialized Size:\t" << cache.size() << std::endl;
    auto foo1 = Foo("foo1");
    cache.set(foo1.content, foo1);
    std::cout << "inserted 1 Size:\t" << cache.size() << std::endl;
    auto foo2 = Foo("foo2");
    cache.set(foo2.content, foo2);
    std::cout << "inserted 2 Size:\t" << cache.size() << std::endl;
    auto foo3 = Foo("foo3");
    cache.set(foo3.content, foo3);
    std::cout << "inserted 3 Size:\t" << cache.size() << std::endl;
    auto foo4 = Foo("foo4");
    cache.set(foo4.content, foo4);
    cache.debugPrint();
    std::cout << cache.get("foo3")->content << std::endl;
    cache.debugPrint();
}

void test3()
{
    std::cout << "\n----TEST 3----\n" << std::endl;
    LRUCaching<std::string, Foo> cache(4);
    std::cout << "Initialized Size:\t" << cache.size() << std::endl;
    auto foo1 = Foo("foo1");
    cache.set(foo1.content, std::move(foo1));
    std::cout << "inserted 1 Size:\t" << cache.size() << std::endl;
    auto foo2 = Foo("foo2");
    cache.set(foo2.content, std::move(foo2));
    std::cout << "inserted 2 Size:\t" << cache.size() << std::endl;
    auto foo3 = Foo("foo3");
    cache.set(foo3.content, std::move(foo3));
    std::cout << "inserted 3 Size:\t" << cache.size() << std::endl;
    auto foo4 = Foo("foo4");
    cache.set(foo4.content, std::move(foo4));
    cache.debugPrint();
    std::cout << cache.get("foo3")->content << std::endl;
    cache.debugPrint();
}

void test4()
{
    std::cout << "\n----TEST 4----\n" << std::endl;
    LRUCaching<std::string, Foo> cache(4);
    std::cout << "Initialized Size:\t" << cache.size() << std::endl;
    auto foo1 = new Foo("foo1");
    cache.set(foo1->content, foo1);
    std::cout << "inserted 1 Size:\t" << cache.size() << std::endl;
    auto foo2 = new Foo("foo2");
    cache.set(foo2->content, foo2);
    std::cout << "inserted 2 Size:\t" << cache.size() << std::endl;
    auto foo3 = new Foo("foo3");
    cache.set(foo3->content, foo3);
    std::cout << "inserted 3 Size:\t" << cache.size() << std::endl;
    auto foo4 = new Foo("foo4");
    cache.set(foo4->content, foo4);
    cache.debugPrint();
    std::cout << cache.get("foo3")->content << std::endl;
    cache.debugPrint();
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    return 0;
}

