#include "algobase.h"
#include "type_traits.h"
#include "deque.h"

namespace mystl {

template<class T, class Container = mystl::deque<T>>
class queue
{
public:
    using container_type = Container;
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    
    static_assert(std::is_same<T, value_type>::value, "the value type of Conintainer should be same with T");

private:
    container_type c_;

public:
    queue() = default;
    explicit queue(size_type n) : c_(n) {};
    queue(size_type n, const value_type& val) : c_(n, val) {};
    template<class InputerIterator>
    queue(InputerIterator first, InputerIterator last) : c_(first, last) {};

    template<class Val>
    queue(std::initializer_list<Val> ilist) : c_(ilist.begin(), ilist.begin()) {};

    queue(const Container& c) 
        :c_(c) 
    {
    }
    queue(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
        :c_(mystl::move(c)) 
    {
    }

public: // 复制&移动
    queue(const queue& rhs) : c_(rhs.c_) {};
    queue(queue&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
    : c_(mystl::move(rhs.c_)) {};
    
    queue& operator=(const queue &rhs) {
        c_ = rhs.c_;
        return *this;
    }

    queue& operator=(queue&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value)
    {
        c_ = mystl::move(rhs.c_);
        return *this;
    }

public: // 插入
    void push(const value_type& val) {
        c_.push_back(val);
    }
    
    void push(value_type&& val) {
        c_.emplace_back(mystl::move(val));
    }

    template<class... Args>
    void emplace_back(Args&&... args) {
        c_.emplace_back(mystl::forward<Args>(args)...);
    }

    void pop() {
        c_.pop_front();
    }

    reference front() {
        return c_.front();
    }

    const_reference front() const {
        return c_.front();
    }

    reference back() {
        return c_.back();
    }

    const_reference back() const {
        return c_.back();
    }

    void clear() {
        c_.clear();
    }

    void swap(queue& rhs) noexcept(noexcept(mystl::swap(c_, rhs.c_)))
    { mystl::swap(c_, rhs.c_); }
public:
    bool empty() const noexcept { return c_.empty(); }
    size_type size() const noexcept { return c_.size(); }
public:
    friend bool operator==(const queue& lhs, const queue& rhs) { return lhs.c_ == rhs.c_; }
    friend bool operator< (const queue& lhs, const queue& rhs) { return lhs.c_ <  rhs.c_; }
};

// 重载比较操作符
template <class T, class Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return lhs == rhs;
}

template <class T, class Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return !(lhs == rhs);
}

template <class T, class Container>
bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return lhs < rhs;
}

template <class T, class Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return rhs < lhs;
}

template <class T, class Container>
bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return !(rhs < lhs);
}

template <class T, class Container>
bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class T, class Container>
void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}


}
