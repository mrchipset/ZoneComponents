#ifndef POOL_OBJECT_POOL_HPP
#define POOL_OBJECT_POOL_HPP

#include <cstdint>
#include <memory>
#include <mutex>
#include <map>

template <class Object>
class ObjectPool
{
public:
    ObjectPool(uint32_t capacity) : 
        mCapacity(capacity),
        mAllocated(0)
    {

    }

    inline uint32_t capacity() const {return mCapacity;}
    inline uint32_t allocated() const {return mAllocated;}

    template <class Object, class ... _Types>
    std::shared_ptr<Object> construct(_Types&&... _Args)
    {
        if (mAllocated >= mCapacity)
        {
            return nullptr;
        } else
        {
            // C++11 code?
            // Object* pObj = mAllocator.allocate(1);
            // mAllocator.construct(pObj, _Args...);

            // TODO allocate more data at once.
            
            Object* pObj = std::allocator_traits<std::allocator<Object>>::allocate(mAllocator, 1);
            std::allocator_traits<std::allocator<Object>>::construct(mAllocator, pObj, _Args...);
            ++mAllocated;
            return std::shared_ptr<Object>(pObj, [&](Object* p){
                std::allocator_traits<std::allocator<Object>>::destroy(mAllocator, pObj);
                std::allocator_traits<std::allocator<Object>>::deallocate(mAllocator, p, 1);
                --mAllocated;

                // TODO add code and do not deallocate the memory and reuse the available memory.
            
                // C++11 code?
                // mAllocator.destroy(p);
                // mAllocator.deallocate(p, 1);
            });
        }
    }

    template <class Object, class ... _Types>
    bool tryConstruct(std::shared_ptr<Object>& obj, _Types&&... _Args)
    {
        obj = construct(std::forward(_Args)...);
        return obj == nullptr;
    }

private:
    uint32_t mCapacity;
    uint32_t mAllocated;
    std::mutex mMutex;
    std::allocator<Object> mAllocator;
};

#endif