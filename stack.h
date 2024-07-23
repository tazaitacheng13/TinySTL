#pragma
#include "deque.h"
#include "algobase.h"

namespace mystl {
    template<class T, class Container = mystl::deque<T>>
    class stack
    {
    public:
        using container_type = Container;
        using value_type = typename Container::value_type;
        using pointer = typename Container::value_type;
        using size_type = typename Container::size_type;
        using reference = typename Container::reference;
        using const_reference = typename Container::const_reference;

        static_assert(std::is_same<T, value_type>::value, "the value_type of Container should be same with T");
    
    private:
        container_type this_;
    
    public:
        stack() = default;
        explicit stack(size_type n) : this_(n) {};
        stack(size_type n, const value_type& val) : this_(n, val) {};
        
        template<class InputIterator>
        stack(InputIterator first, InputIterator last) : this_(first, last) {};

        stack(std::initializer_list<T> ilist) : this_(ilist.begin(), ilist.end()) {};

        stack(const Container& c)
            :this_(c)
        {
        }

         stack(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
            :this_(mystl::move(c)) 
        {
        }

        stack(const stack& rhs) 
            :this_(rhs.this_) 
        {
        }

        stack(stack&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value) 
        : this_(mystl::move(rhs.this_)) {};

        stack& operator=(const stack& rhs)
        {
            this_ = rhs.this_;
            return *this;
        }

        stack& operator=(stack&& rhs) noexcept(std::is_nothrow_constructible<Container>::value)
        {
            this_ = mystl::move(rhs.this_);
            return *this;
        }

        ~stack() = default;

        // 访问元素
        reference top() {return this_.back();}
        size_type size() {return this_.size();}

        // push & pop
        void push(const value_type& val) {this_.push_back(val);}
        void push(value_type&& val) {this_.push_back(mystl::move(val));}
        void pop() {this_.pop_back();}
        template<class... Args>
        void emplace_back(Args&&... args) {
            this_.emplace_back(mystl::forward<Args>(args)...);
        }
        void clear() {
            this_.clear();
        }

        void swap(stack& rhs) noexcept(noexcept(mystl::swap(this_, rhs.this_))) {
            mystl::swap(this_, rhs.this_);
        }

        public:
            friend bool operator==(const stack& lhs, const stack& rhs) {return lhs.this_ == rhs.this_;}
            friend bool operator<(const stack& lhs, const stack& rhs) {return lhs.this_ < rhs.this_;}

    };
    
    template<class T, class Container>
    bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
        return lhs== rhs;
    }

    template<class T, class Container>
    bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
        return lhs < rhs;
    }

    template<class T, class Container>
    bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
        return !(lhs == rhs);
    }

    template<class T, class Container>
    bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
        return rhs < lhs;
    }

    template<class T, class Container>
    bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
        return !(rhs < lhs);
    }

    template<class T, class Container>
    bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
        return rhs <= lhs;
    }

    template<class T, class Container>
    void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs))){
        return lhs.swap(rhs);
    }

}