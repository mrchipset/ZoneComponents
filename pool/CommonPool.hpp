#ifndef POOL_COMMON_POOL_HPP
#define POOL_COMMON_POOL_HPP

#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <list>
#include <memory>
#include <chrono>

template <class T>
class CommonPool;

template <class T>
class CommonInstance
{
public:
    CommonInstance(CommonPool<T>* pool)
    {
        mInnerInstance = std::make_shared<T>();
    }

    CommonInstance(std::shared_ptr<T> inner, CommonPool<T>* pool) :
        mInnerInstance(inner),
        mPool(pool)
    {

    }

    T* get() { return mInnerInstance.get(); }
    void releaseInstance() 
    {
        std::unique_lock<std::mutex> lk(mPool->mPoolMutex);
        mPool->mInstancePool.push_back(std::make_shared<CommonInstance<T>>(mInnerInstance, mPool));
        mPool->mPoolAvailableCond.notify_one();
    }
private:
    std::shared_ptr<T> mInnerInstance;
    CommonPool<T>* mPool;


};

template <class T>
class CommonPool
{
public:
    static std::shared_ptr<CommonPool<T>> CreatePool(uint32_t size);
    static std::shared_ptr<CommonPool<T>> CreatePoolFromInstances(std::list<T*> instances);

    std::shared_ptr<CommonInstance<T>> acquireInstance();
    bool tryAcquireInstance(std::shared_ptr<CommonInstance<T>>& instance, uint32_t timeout=0);

    inline uint32_t size() const;
    ~CommonPool();
private:
    uint32_t mPoolSize;
    std::list<std::shared_ptr<CommonInstance<T>>> mInstancePool;
    std::mutex mPoolMutex;
    std::condition_variable mPoolAvailableCond;

    CommonPool(uint32_t size);

protected:
    friend class CommonInstance<T>;
};

template <class T>
std::shared_ptr<CommonPool<T>> CommonPool<T>::CreatePool(uint32_t size)
{
    auto pPool = new CommonPool<T>(size);
    std::shared_ptr<CommonPool<T>> pool(pPool);
    std::unique_lock<std::mutex> lk(pool->mPoolMutex);
    for (uint32_t idx = 0; idx < pool->mPoolSize; ++idx)
    {
        auto& instancePtr = std::make_shared<CommonInstance<T>>(pool.get());
        pool->mInstancePool.push_back(instancePtr);
    }
    pool->mPoolAvailableCond.notify_all();
    return pool;
}

template <class T>
std::shared_ptr<CommonPool<T>> CommonPool<T>::CreatePoolFromInstances(std::list<T*> instances)
{
    auto pPool = new CommonPool<T>(instances.size());
    std::shared_ptr<CommonPool<T>> pool(pPool);
    std::unique_lock<std::mutex> lk(pool->mPoolMutex);
    for (auto pInstance : instances)
    {
        std::shared_ptr<T> ptr;
        ptr.reset(pInstance);
        auto& instancePtr = std::make_shared<CommonInstance<T>>(ptr, pool.get());
        pool->mInstancePool.push_back(instancePtr);
    }
    pool->mPoolAvailableCond.notify_all();
    return pool;
}

template <class T>
std::shared_ptr<CommonInstance<T>> CommonPool<T>::acquireInstance()
{
    std::unique_lock<std::mutex> lk(mPoolMutex);
    mPoolAvailableCond.wait(lk, [this](){
        return !mInstancePool.empty();
    });
    std::shared_ptr<CommonInstance<T>> instance = mInstancePool.front();
    mInstancePool.pop_front();
    return instance;
}

template <class T>
bool CommonPool<T>::tryAcquireInstance(std::shared_ptr<CommonInstance<T>>& instance, uint32_t timeout)
{
    std::unique_lock<std::mutex> lk(mPoolMutex);
    bool b = mPoolAvailableCond.wait_for(lk, std::chrono::duration(std::chrono::milliseconds(timeout)), [this](){
        return !mInstancePool.empty();
    });

    if (!b)
    {
        instance = nullptr;
        return false;
    } else
    {
        instance = mInstancePool.front();
        mInstancePool.pop_front();
        return true;
    }
}

template <class T>
uint32_t CommonPool<T>::size() const
{
    return mPoolSize;
}

template <class T>
CommonPool<T>::~CommonPool()
{

}

template <class T>
CommonPool<T>::CommonPool(uint32_t size) :
    mPoolSize(size)
{

}

#endif