#pragma once


#include <functional>
#include <atomic>
#include "algobase.h"
#include "type_traits.h"


namespace mystl
{
template<class T>
class shared_ptr {
    using m_del = std::function<void(T*)>;
public:
    shared_ptr(T* p = nullptr, m_del del = [](T* p) {delete p; }) : ptr(p), ref_count(new std::atomic<size_t>(p != nullptr)), deleter(del) {}
    ~shared_ptr() {
        decrementAndDestory();
    }
public:
    // 复制构造函数
    shared_ptr(const shared_ptr& other) : ptr(other.ptr), ref_count(other.ref_count), deleter(other.del) {
        ++* ref_count;
    }
    shared_ptr& operator=(const shared_ptr& rhs);

    // 移动构造函数
    shared_ptr(const shared_ptr&& other) : ptr(other.ptr), ref_count(other.ref_count), deleter(mystl::move(other.del)) {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }
    shared_ptr& operator=(const shared_ptr&& rhs) noexcept;

public: // 运算符重载
    T &operator*() const noexcept { return *ptr; }
    T* operator->() const noexcept { return ptr; }

public:
    operator bool() const noexcept { return ptr; }
public:
    T* get() const noexcept { return ptr; }
    size_t use_count() const noexcept { return *ref_count; }
    bool unique() const noexcept { return *ref_count == 1; }
    // 重置指针
    void reset(T* p = nullptr, const m_del& d = [](T* p) {delete p; });

private:
    void decrementAndDestory();
    void swap(shared_ptr& other) noexcept;

private:
    std::atomic<size_t>* ref_count;
    T* ptr;
    std::function<void(T*)> deleter;
};

template<class T>
inline void shared_ptr<T>::swap(shared_ptr& other) noexcept {
    std::swap(this->ptr, other.ptr);
    std::swap(this->ref_count, other.ref_count);
    std::swap(this->deleter, other.deleter);
}

template<class T>
inline shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr<T>& rhs) {
    // increment first to ensure safty for self-assignment and avoid identity test
    ++*rhs.ref_count;
    decrementAndDestory();
    ptr = rhs.ptr, ref_count = rhs.ref_count, deleter = rhs.deleter;
    return *this;
}

template<class T>
inline shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr<T>&& rhs) noexcept {
    swap(rhs);
    rhs.decrementAndDestory();
    return *this;
}

template<class T>
inline void shared_ptr<T>::reset(T* p, const m_del& d) {
    if (p != ptr)
    {
        decrementAndDestory();
        ptr = p;
        *ref_count = new std::atomic<size_t>(p == nullptr);
    }
    deleter = d;
}

template<class T>
inline void shared_ptr<T>::decrementAndDestory() {
    if (ptr && --*ref_count == 0) {
        delete ref_count;
        deleter(ptr);
    }
    else if (!ptr) {
        delete ref_count;
    }
    ptr = nullptr;
    ref_count = nullptr;
}

}