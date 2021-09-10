#ifndef CACHE_LRU_CACHING_HPP
#define CACHE_LUR_CACHING_HPP

#include <cstdint>
#include <string>
#include <list>
#include <unordered_map>
#include <memory>
#include <iostream>

template <class Key, class Value>
class LRUCaching
{
public:
    LRUCaching(uint32_t capacity);
    inline uint32_t size() const { return mSize; }
    inline uint32_t capacity() const { return mCapacity; }
    
    std::shared_ptr<Value> get(const Key &key);
    bool set(const Key &key, std::shared_ptr<Value> valuePtr);
    inline bool set(const Key &key, const Value& value);
    inline bool set(const Key &key, Value&& value);
    inline bool set(const Key &key, Value* pValue);

#if _DEBUG
    void debugPrint()
    {
        std::cout << "----List start----\n";
        for (auto& obj : mLRUList)
        {
            std::cout << obj.key << '\n';
        }
        std::cout << "----List end----\n" << std::endl;
    }
#endif
private:
    typedef struct {
        Key key;
        std::shared_ptr<Value> ptr;
    } CacheObject;

    uint32_t mCapacity;
    uint32_t mSize;
    std::unordered_map<std::string, CacheObject> mCacheObjDict;
    std::list<CacheObject> mLRUList;

    bool moveKeyToHead(const Key& key);
};

template <class Key, class Value>
LRUCaching<Key, Value>::LRUCaching(uint32_t capacity) :
    mCapacity(capacity),
    mSize(0)
{

}

template <class Key, class Value>
std::shared_ptr<Value> LRUCaching<Key, Value>::get(const Key &key)
{
    auto obj = mCacheObjDict.find(key);
    if (obj == mCacheObjDict.end())
    {
        return nullptr;
    } else
    {
        if (!moveKeyToHead(key))
        {
            // found in dictory, but not found in LRU list
            mCacheObjDict.erase(key);
            return nullptr;
        } else
        {
            return obj->second.ptr;
        }
    }
}

template <class Key, class Value>
bool LRUCaching<Key, Value>::set(const Key &key, std::shared_ptr<Value> valuePtr)
{
    auto obj = mCacheObjDict.find(key);
    if (obj == mCacheObjDict.end())
    {
        // key is not existed.
        if (++mSize > mCapacity)
        {
            --mSize;
            auto _key = mLRUList.back().key;
            mCacheObjDict.erase(_key);
            mLRUList.pop_back();
        }
        // CacheObject tmpObj{key, valuePtr};
        CacheObject tmpObj;
        tmpObj.key = key;
        tmpObj.ptr = valuePtr;
        mCacheObjDict.insert(std::make_pair<>(key, tmpObj));
        mLRUList.push_front(tmpObj);
    } else {
        // key is already existed.
        obj->second.ptr = valuePtr;
        moveKeyToHead(key);
    }

    return obj == mCacheObjDict.end();
}

template <class Key, class Value>
bool LRUCaching<Key, Value>::set(const Key &key, const Value& value)
{
    return set(key, std::make_shared<Value>(value));
}

template <class Key, class Value>
bool LRUCaching<Key, Value>::set(const Key &key, Value&& value)
{
    return set(key, std::make_shared<Value>(value));
}

template <class Key, class Value>
bool LRUCaching<Key, Value>::set(const Key &key, Value* pValue)
{
    return set(key, std::shared_ptr<Value>(pValue));
}

template <class Key, class Value>
bool LRUCaching<Key, Value>::moveKeyToHead(const Key& key)
{
    CacheObject object;
    for (auto iter=mLRUList.begin(); iter!=mLRUList.end(); ++iter)
    {
        if (iter->key == key)
        {
            object = *iter;
            mLRUList.erase(iter);
            mLRUList.push_front(object);
            return true;
        }
    }
    return false;
}

#endif