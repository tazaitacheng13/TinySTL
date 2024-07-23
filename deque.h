#pragma once

#include "iterator.h"
#include "allocator.h"
#include "uninitialized.h"

#include <cstddef>

// deque map 初始化的大小
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

namespace mystl {
    // 缓冲区大小设定函数（在预设情况下传回可容纳的元素个数）
    // 若n不为0，则传回n，表示由用户自定
    // 若n为0则采用预设值 预设值根据sz（元素大小）而定
    inline size_t __deque_buf_size(size_t sz) {
        return sz < 512 ? size_t(512 / sz) : size_t(1);
    }

    template<class T, class Ref, class Ptr>
    struct __deque_iterator {
        using iterator = __deque_iterator<T, T &, T *>;
        using const_iterator = __deque_iterator<T, const T &, const T *>;
        using self = __deque_iterator;

        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using pointer = Ptr;
        using reference = Ref;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using map_pointer = T **;

        // data member
        value_type * cur;       // 当前位置
        value_type * first;     // 缓存区开头
        value_type * last;      // 当前缓存区末尾
        map_pointer node;       // 指向中控数组的节点

        static size_t buffer_size() { return __deque_buf_size(sizeof(value_type)); }

        // ctor
        __deque_iterator()
            : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
        __deque_iterator(pointer x, map_pointer y)
            : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
        __deque_iterator(const iterator &rhs)
            : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
        
        // move
        __deque_iterator(iterator&& rhs) :
        cur(rhs.cur), first(rhs.first), last(rhs.last),node(rhs.node)
         {
            rhs.cur = nullptr;
            rhs.first = nullptr;
            rhs.last = nullptr;
            rhs.node = nullptr;
        }

        self& operator=(const iterator& rhs)
        {
            if (this != &rhs)
            {
                cur = rhs.cur;
                first = rhs.first;
                last = rhs.last;
                node = rhs.node;
            }
            return *this;
        }

        self& operator=(iterator&& rhs) {
            if (this != &rhs) {
                cur = rhs.cur;
                first = rhs.first;
                last = rhs.last;
                node = rhs.node;
                rhs.cur = nullptr;
                rhs.first = nullptr;
                rhs.last = nullptr;
                rhs.node = nullptr;
            }
            return *this;
        }

        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + static_cast<difference_type>(buffer_size());
        }

        // dereference
        reference operator*() const { return *cur; }
        pointer operator->() const { return &(operator*()); }

        self &operator++() {
            ++cur;
            if (cur == last) {// 若已抵达尾端
            set_node(node + 1);
            cur = first;
            }
            return *this;
        }

        self operator++(int) {
            self temp = *this;
            ++*this;
            return temp;
        }

        self &operator--() {
            if (cur == first) {
            set_node(node - 1);
            cur = last;
            }
            --cur;
            return *this;
        }

        self operator--(int) {
            self temp = *this;
            --*this;
            return temp;
        }

        self &operator+=(difference_type n) {
            difference_type offset = n + (cur - first);
            if (offset >= 0 && offset < static_cast<difference_type>(buffer_size())) {  // 无需跳转
                cur += n;
            } else {
                difference_type node_offset =
                    offset > 0
                    ? offset / static_cast<difference_type>(buffer_size())
                    : -static_cast<difference_type>((-offset - 1) / buffer_size()) - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size()));
            }
            return *this;
        }

        self operator+(difference_type n) const {
            self temp = *this;
            return temp += n;
        }

        self &operator-=(difference_type n) { return *this += -n; }

        self operator-(difference_type n) {
            self temp = *this;
            return temp -= n;
        }

        reference operator[](difference_type n) {return *(*this + n);}
    };


    template<class T, class Ref, class Ptr>
    inline bool operator==(const __deque_iterator<T, Ref, Ptr> &lhs,
                        const __deque_iterator<T, Ref, Ptr> &rhs) {
    return lhs.cur == rhs.cur;
    }

    // compare with const
    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator==(const __deque_iterator<T, RefL, PtrL> &lhs,
                        const __deque_iterator<T, RefR, PtrR> &rhs) {
    return lhs.cur == rhs.cur;
    }

    template<class T, class Ref, class Ptr>
    inline bool operator!=(const __deque_iterator<T, Ref, Ptr> &lhs,
                        const __deque_iterator<T, Ref, Ptr> &rhs) {
    return !(lhs == rhs);
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator!=(const __deque_iterator<T, RefL, PtrL> &lhs,
                        const __deque_iterator<T, RefR, PtrR> &rhs) {
    return !(lhs == rhs);
    }

    template<class T, class Ref, class Ptr>
    inline bool operator<(const __deque_iterator<T, Ref, Ptr> &lhs,
                        const __deque_iterator<T, Ref, Ptr> &rhs) {
    return (lhs.node == rhs.node) ? (lhs.cur < rhs.cur) : (lhs.node < rhs.node);
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator<(const __deque_iterator<T, RefL, PtrL> &lhs,
                        const __deque_iterator<T, RefR, PtrR> &rhs) {
    return (lhs.node == rhs.node) ? (lhs.cur < rhs.cur) : (lhs.node < rhs.node);
    }

    template<class T, class Ref, class Ptr>
    inline bool operator>(const __deque_iterator<T, Ref, Ptr> &lhs,
                        const __deque_iterator<T, Ref, Ptr> &rhs) {
    return rhs < lhs;
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator>(const __deque_iterator<T, RefL, PtrL> &lhs,
                        const __deque_iterator<T, RefR, PtrR> &rhs) {
    return rhs < lhs;
    }

    template<class T, class Ref, class Ptr>
    inline bool operator<=(const __deque_iterator<T, Ref, Ptr> &lhs,
                        const __deque_iterator<T, Ref, Ptr> &rhs) {
    return !(rhs < lhs);
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator<=(const __deque_iterator<T, RefL, PtrL> &lhs,
                        const __deque_iterator<T, RefR, PtrR> &rhs) {
    return !(rhs < lhs);
    }

    template<class T, class Ref, class Ptr>
    inline bool operator>=(const __deque_iterator<T, Ref, Ptr> &lhs,
                        const __deque_iterator<T, Ref, Ptr> &rhs) {
    return !(lhs < rhs);
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline bool operator>=(const __deque_iterator<T, RefL, PtrL> &lhs,
                        const __deque_iterator<T, RefR, PtrR> &rhs) {
    return !(lhs < rhs);
    }

    template<class T, class Ref, class Ptr>
    inline typename __deque_iterator<T, Ref, Ptr>::difference_type operator-(
        const __deque_iterator<T, Ref, Ptr> &lhs,
        const __deque_iterator<T, Ref, Ptr> &rhs) {
        return typename __deque_iterator<T, Ref, Ptr>::difference_type(
            __deque_iterator<T, Ref, Ptr>::buffer_size() * (lhs.node - rhs.node - 1) + (lhs.cur - lhs.first) + (rhs.last - rhs.cur));
    }

    template<class T, class RefL, class PtrL, class RefR, class PtrR>
    inline typename __deque_iterator<T, RefL, PtrL>::difference_type operator-(
        const __deque_iterator<T, RefL, PtrL> &lhs,
        const __deque_iterator<T, RefR, PtrR> &rhs) {
        return typename __deque_iterator<T, RefL, PtrL>::difference_type(
            __deque_iterator<T, RefL, PtrL>::buffer_size() * (lhs.node - rhs.node - 1) + (lhs.cur - lhs.first) + (rhs.last - rhs.cur));
    }

    template<class T, class Ref, class Ptr>
    inline __deque_iterator<T, Ref, Ptr> operator+(
        ptrdiff_t n, const __deque_iterator<T, Ref, Ptr> &x) {
        return x + n;
    }
} // namespace mystl


namespace mystl {
    template<class T, class Alloc = simpleAlloc<T>>
    class deque {
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using iterator = __deque_iterator<T, reference, pointer>;
        using reverse_iterator = __reverse_iterator<iterator>;
        using const_iterator = __deque_iterator<T, const T&, const T*>;
        using const_reverse_iterator = __reverse_iterator<const_iterator>;
    private:
        using map_pointer = pointer *;                      // 中控数组指针
        using node_allocator = simpleAlloc<value_type>;     // 缓存区空间构造器
        using map_allocator = simpleAlloc<pointer>;         // 中控数组空间构造器

    private:
        iterator begin_;                                     // 第一个节点（缓存区）
        iterator end_;                                    // 最后一个节点
        map_pointer map_;                                    // 中控指针
        size_type map_size_;                                 // 中控数组大小
    
    private: // alloc for buffer
        value_type* allocate_node() {                                                   // 开辟一个缓存区
            return node_allocator::allocate(__deque_buf_size(sizeof(value_type)));
        }

        void deallocate_node(value_type* p) {                                             // 释放一个缓存区
            return node_allocator::deallocate(p, __deque_buf_size(sizeof(value_type)));
        }

        void create_nodes(map_pointer, map_pointer);
        void destroy_nodes(map_pointer, map_pointer);
    
    private: // alloc for map
        map_pointer create_map(size_type);
        void initialize_map(size_type);
        void deallocate_map(map_pointer p, size_type n) {
            map_allocator::deallocate(p, n);
        }
       
        // reallocate
        void        require_capacity(size_type n, bool front);
        void        reallocate_map_at_front(size_type need);
        void        reallocate_map_at_back(size_type need);
    
    private: // aux_interface for ctor
        size_type initial_map_size() const noexcept {return 8U; }
        size_type buffer_size() const noexcept {return iterator::buffer_size();}
        void fill_initialize(const value_type&);

        // dispatch trick for init
        template<class Integer>
        void initialize_dispatch(Integer n, Integer val, true_type) {
            initialize_map(static_cast<size_type>(n));
            fill_initialize(static_cast<value_type>(val));
        }

        template<class InputIterator>
        void initialize_dispatch(InputIterator first, InputIterator last, false_type) {
            copy_init(first, last, iterator_category_t<InputIterator>());
        }

        // // copy init
        // template<class InputIterator>
        // void copy_initialize(InputIterator first, InputIterator last, input_iterator_tag);
        // template<class ForwardIterator>
        // void copy_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag);

        template <class IIter>
        void        copy_init(IIter, IIter, input_iterator_tag);
        template <class FIter>
        void        copy_init(FIter, FIter, forward_iterator_tag);
    
    public: // ctor & dtor
        deque() : begin_(), end_(), map_(nullptr), map_size_(0) {
            initialize_map(0);
        }

        explicit deque(size_type n) : begin_(), end_(), map_(nullptr), map_size_(0) {
            initialize_map(n);
            fill_initialize(value_type());
        }

        deque(size_type n, const value_type& val) : begin_(), end_(), map_(nullptr), map_size_(0) {
            initialize_map(n);
            fill_initialize(val);
        }

        template<class InputIterator>
        deque(InputIterator first, InputIterator last) {
            initialize_dispatch(first, last, is_integral<InputIterator>());
        }

        deque(std::initializer_list<value_type> ils) {
            copy_init(ils.begin(), ils.end(), random_access_iterator_tag());
        }

        ~deque() {
            if (map_ != nullptr)
            {
            clear();
            deallocate_node(*begin_.node);
            *begin_.node = nullptr;
            map_allocator::deallocate(map_, map_size_);
            map_ = nullptr;
            }
        }

    public: // copy operation
        deque(const deque &rhs) {
            initialize_map(rhs.size());
            mystl::uninitialized_copy(rhs.begin(), rhs.end(), begin_);
        }
        deque &operator=(const deque &);

    public: // move operation
        deque(deque&& rhs) noexcept
            :begin_(mystl::move(rhs.begin_)),
            end_(mystl::move(rhs.end_)),
            map_(rhs.map_),
            map_size_(rhs.map_size_)
        {
            rhs.map_ = nullptr;
            rhs.map_size_ = 0;
        }
        deque &operator=(deque&& rhs); 
    
    public:// getter
        const_iterator begin() const noexcept { return begin_; }
        const_iterator end() const noexcept { return end_; }
        const_iterator cbegin() const noexcept { return begin_; }
        const_iterator cend() const noexcept { return end_; }
        const_reverse_iterator crbegin() const noexcept {
            return const_reverse_iterator(end_);
        }
        const_reverse_iterator crend() const noexcept {
            return const_reverse_iterator(begin_);
        }
        const_reference operator[](size_type n) const noexcept {
            return begin_[static_cast<difference_type>(n)];
        }
        const_reference front() const noexcept { return *begin_; }
        const_reference back() const noexcept { return *(end_ - 1); }
        size_type size() const noexcept { return end_ - begin_; }
        bool empty() const noexcept { return end_ == begin_; }

    public:// setter
        iterator begin() noexcept { return begin_; }
        iterator end() noexcept { return end_; }
        reverse_iterator rbegin() noexcept { return reverse_iterator(end_); }
        reverse_iterator rend() noexcept { return reverse_iterator(begin_); }
        reference operator[](size_type n) {
            return begin_[static_cast<difference_type>(n)];
        }
        reference front() noexcept { return *begin_; }
        reference back() noexcept { return *(end_ - 1); }
    
    public:
        template <class ...Args>
        void     emplace_front(Args&& ...args);
        template <class ...Args>
        void     emplace_back(Args&& ...args);
        template <class ...Args>
        iterator emplace(iterator pos, Args&& ...args);

    public:// push && pop
        void push_back(const value_type &);
        void push_front(const value_type &);
        void pop_back();
        void pop_front();

        void     push_front(value_type&& value) { emplace_front(mystl::move(value)); }
        void     push_back(value_type&& value)  { emplace_back(mystl::move(value)); }

    private:// aux_interface for assign
        void fill_assign(size_type, const value_type &);
        template<class Integer>
        void assign_dispatch(Integer n, Integer val, true_type) {
            fill_assign(static_cast<size_type>(n), static_cast<value_type>(val));
        }
        template<class InputIterator>
        void assign_dispatch(InputIterator first, InputIterator last, false_type) {
            assign_aux(first, last, iterator_category_t<InputIterator>());
        }
        template<class InputIterator>
        void assign_aux(InputIterator, InputIterator, input_iterator_tag);
        template<class ForwardIterator>
        void assign_aux(ForwardIterator, ForwardIterator, forward_iterator_tag);

    public:// assign
        void assign(size_type n, const value_type &val) { fill_assign(n, val); }
        void assign(std::initializer_list<value_type> ils) {
            assign(ils.begin(), ils.end());
        }
        template<class InputIterator>
        void assign(InputIterator first, InputIterator last) {
            assign_dispatch(first, last, is_integral<InputIterator>());
        }
    public: // 运算符重载
        deque &operator=(std::initializer_list<value_type> ils) {
            assign(ils.begin(), ils.end());
            return *this;
        }

    private: // aux_interface for insert
        template <class... Args>
        iterator    insert_aux(iterator position, Args&& ...args);
        void        fill_insert(iterator position, size_type n, const value_type& x);
        template <class FIter>
        void        copy_insert(iterator, FIter, FIter, size_type);
        template<class Iter>
        void range_insert(iterator pos, Iter first, Iter last, false_type) {
            insert_dispatch(pos, first, last, iterator_category_t<Iter>());
        }
        template<class Integer>
        void range_insert(iterator pos, Integer n, Integer val, true_type) {
            fill_insert(pos, static_cast<size_type>(n), static_cast<value_type>(val));
        }

        template <class IIter>
        void        insert_dispatch(iterator, IIter, IIter, input_iterator_tag);
        template <class FIter>
        void        insert_dispatch(iterator, FIter, FIter, forward_iterator_tag);

    public:// insert
        iterator insert(iterator, const value_type &);
        iterator insert(iterator, value_type &&);
        iterator insert(iterator pos) { return insert(pos, value_type()); }
        void insert(iterator pos, size_type n, const value_type &val);
        template<class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last) {
            range_insert(pos, first, last, is_integral<InputIterator>());
        }
    
    public:// erase
        iterator erase(iterator);
        iterator erase(iterator, iterator);
        void shrink_to_fit() noexcept;
        void clear();

    public:// resize
        void resize(size_type, const value_type &);
        void resize(size_type new_size) { resize(new_size, value_type()); }

    public:// swap
        void swap(deque &rhs) noexcept;

    };

    // 复制赋值运算符
    template <class T, class Alloc>
    deque<T, Alloc>& deque<T, Alloc>::operator=(const deque& rhs)
    {
        if (this != &rhs)
        {
            const size_type len = size();
            if (len >= rhs.size())
            {
                erase(mystl::copy(rhs.begin_, rhs.end_, begin_), end_);
            }
            else
            {
                const_iterator mid = rhs.begin() + static_cast<difference_type>(len);
                mystl::copy(rhs.begin(), mid, begin_);
                insert(end_, mid, rhs.end());
            }
        }
        return *this;
    }

    // 移动赋值运算符
    template <class T, class Alloc>
    deque<T, Alloc>& deque<T, Alloc>::operator=(deque&& rhs)
    {
        clear();
        begin_ = mystl::move(rhs.begin_);
        end_ = mystl::move(rhs.end_);
        map_ = rhs.map_;
        map_size_ = rhs.map_size_;
        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
        return *this;
    }


    template<class T, class Alloc>
    void deque<T, Alloc>::create_nodes(map_pointer nstart, map_pointer nfinish) {
        map_pointer cur;
        try {
            for (cur = nstart; cur <= nfinish; ++cur) * cur = allocate_node();
        } catch (std::exception &) {
            destroy_nodes(nstart, cur);
            throw;
        }
    }

    template<class T, class Alloc>
    void deque<T, Alloc>::destroy_nodes(map_pointer nstart, map_pointer nfinish) {
        for (map_pointer n = nstart; n <= nfinish; ++n) {
            deallocate_node(*n);
            *n = nullptr;
        }
    }


    template<class T, class Alloc>
    typename deque<T, Alloc>::map_pointer
    deque<T, Alloc>::create_map(size_type size) {
        map_pointer mp = nullptr;
        mp = map_allocator::allocate(size);
        for (size_type i = 0; i < size; ++i)
            *(mp + i) = nullptr;
        return mp;
    }

    template<class T, class Alloc>
    void deque<T, Alloc>::initialize_map(size_type n) {
        // 所需节点数
        const size_type nNode = n / buffer_size() + 1;
        map_size_ = max(initial_map_size(), nNode + 2);
        try {
            map_ = create_map(map_size_);
        } catch (...) {
            map_ = nullptr;
            map_size_ = 0;
            throw;
        }

        // nstart和nfinish指向node中部，往两边扩充
        map_pointer nstart = map_ + (map_size_ - nNode) / 2;
        map_pointer nfinish = nstart + nNode - 1;
        try {
            create_nodes(nstart, nfinish);
        } catch(...) {
            deallocate_map(nstart, map_size_);
            map_ = nullptr;
            map_size_ = 0;
            throw;
        }

        begin_.set_node(nstart);
        end_.set_node(nfinish);
        begin_.cur = begin_.first;
        end_.cur = end_.first + (n % buffer_size());  // 整除的话刚好指向多出来的缓冲区的第一个
    }

    template<class T, class Alloc>
    void deque<T, Alloc>::fill_initialize(const value_type &val) {
        map_pointer cur;
        try {
            // 为每个缓冲区设定初值
            for (cur = begin_.node; cur < end_.node; ++cur)
                mystl::uninitialized_fill(*cur, *cur + buffer_size(), val);
            // 最后一个缓冲区设定到最后一个元素处
            mystl::uninitialized_fill(end_.first, end_.cur, val);
        } catch (std::exception &) {
            mystl::destroy(begin_, iterator(*cur, cur));
            throw;
        }
    }

    // template<class T, class Alloc>
    // template<class InputIterator>
    // void deque<T, Alloc>::copy_initialize(InputIterator first, InputIterator last,
    //                                         input_iterator_tag) {
    //     initialize_map(0);
    //     try {
    //         for (; first != last; ++first) push_back(*first);
    //     } catch (std::exception &) {
    //         clear();
    //         throw;
    //     }
    // }

    // template<class T, class Alloc>
    // template<class ForwardIterator>
    // void deque<T, Alloc>::copy_initialize(ForwardIterator first,
    //                                     ForwardIterator last,
    //                                     forward_iterator_tag) {
    //     size_type n = mystl::distance(first, last);
    //     initialize_map(n);
    //     map_pointer cur_node;
    //     // 创建足够大小的缓存区并拷贝数据
    //     try {
    //         for (cur_node = begin_.node; cur_node < end_.node; ++cur_node) {
    //             ForwardIterator mid = first;
    //             mystl::advance(mid, buffer_size());
    //             mystl::uninitialized_copy(first, mid, *cur_node);
    //             first = mid;
    //         }
    //         mystl::uninitialized_copy(first, last, end_.first);
    //     } catch (std::exception &) {
    //         mystl::destroy(begin_, iterator(*cur_node, cur_node));
    //         throw;
    //     }
    // }

    // ----------------------------空间分配模块----------------------------------------

    // reallocate_map_at_front 函数
    template <class T, class Alloc>
    void deque<T, Alloc>::reallocate_map_at_front(size_type need_buffer)
    {
        const size_type new_map_size = mystl::max(map_size_ << 1,
                                                    map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE);
        map_pointer new_map = create_map(new_map_size);
        const size_type old_buffer = end_.node - begin_.node + 1;
        const size_type new_buffer = old_buffer + need_buffer;

        // 另新的 map 中的指针指向原来的 buffer，并开辟新的 buffer
        auto begin = new_map + (new_map_size - new_buffer) / 2;
        auto mid = begin + need_buffer;
        auto end = mid + old_buffer;
        create_nodes(begin, mid - 1);
        for (auto begin1 = mid, begin2 = begin_.node; begin1 != end; ++begin1, ++begin2)
            *begin1 = *begin2;

        // 更新数据
        map_allocator::deallocate(map_, map_size_);
        map_ = new_map;
        map_size_ = new_map_size;
        begin_ = iterator(*mid + (begin_.cur - begin_.first), mid);
        end_ = iterator(*(end - 1) + (end_.cur - end_.first), end - 1);
    }

    // reallocate_map_at_back 函数
    template <class T, class Alloc>
    void deque<T, Alloc>::reallocate_map_at_back(size_type need_buffer)
    {
        const size_type new_map_size = mystl::max(map_size_ << 1,
                                                    map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE);
        map_pointer new_map = create_map(new_map_size);
        const size_type old_buffer = end_.node - begin_.node + 1;
        const size_type new_buffer = old_buffer + need_buffer;

        // 另新的 map 中的指针指向原来的 buffer，并开辟新的 buffer
        auto begin = new_map + ((new_map_size - new_buffer) / 2);
        auto mid = begin + old_buffer;
        auto end = mid + need_buffer;
        for (auto begin1 = begin, begin2 = begin_.node; begin1 != mid; ++begin1, ++begin2)
            *begin1 = *begin2;
        create_nodes(mid, end - 1);

        // 更新数据
        map_allocator::deallocate(map_, map_size_);
        map_ = new_map;
        map_size_ = new_map_size;
        begin_ = iterator(*begin + (begin_.cur - begin_.first), begin);
        end_ = iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
    }


    // require_capacity 函数
    template <class T, class Alloc>
    void deque<T, Alloc>::require_capacity(size_type n, bool front)
    {
        if (front && (static_cast<size_type>(begin_.cur - begin_.first) < n))
        {
            const size_type need_buffer = (n - (begin_.cur - begin_.first)) / buffer_size() + 1;
            if (need_buffer > static_cast<size_type>(begin_.node - map_))
            {
                reallocate_map_at_front(need_buffer);
                return;
            }
            create_nodes(begin_.node - need_buffer, begin_.node - 1);
        }
        else if (!front && (static_cast<size_type>(end_.last - end_.cur - 1) < n))
        {
            const size_type need_buffer = (n - (end_.last - end_.cur - 1)) / buffer_size() + 1;
            if (need_buffer > static_cast<size_type>((map_ + map_size_) - end_.node - 1))
            {
                reallocate_map_at_back(need_buffer);
                return;
            }
            create_nodes(end_.node + 1, end_.node + need_buffer);
        }
    }

    // ------------------------------空间分配模块----------------------------------------
    



    // ------------------------------push pop方法-------------------------------------------

    // 在尾部插入元素
    template <class T, class Alloc>
    void deque<T, Alloc>::push_back(const value_type& value)
    {
        if (end_.cur != end_.last - 1)
        {
            construct(end_.cur, value);
            ++end_.cur;
        }
        else
        {
            require_capacity(1, false);
            construct(end_.cur, value);
            ++end_;
        }
    }

    // 弹出头部元素
    template <class T, class Alloc>
    void deque<T, Alloc>::pop_front()
    {
        if (begin_.cur != begin_.last - 1)
        {
            destroy(begin_.cur);
            ++begin_.cur;
        }
        else
        {
            destroy(begin_.cur);
            ++begin_;
            destroy_nodes(begin_.node - 1, begin_.node - 1);
        }
    }

    // 弹出尾部元素
    template<class T, class Alloc>
    void deque<T, Alloc>::pop_back() {
        if (end_.cur != end_.first) {
            --end_.cur;
            destroy(end_.cur);
        }
         else
        {
            --end_;
            destroy(end_.cur);
            destroy_nodes(end_.node + 1, end_.node + 1);
        }
    }


// -----------------------------------insert方法------------------------------------------

    // 在 position 处插入元素
    template <class T, class Alloc>
    typename deque<T, Alloc>::iterator
    deque<T, Alloc>::insert(iterator position, const value_type& value)
    {
        if (position.cur == begin_.cur)
        {
            push_front(value);
            return begin_;
        }
        else if (position.cur == end_.cur)
        {
            push_back(value);
            auto tmp = end_;
            --tmp;
            return tmp;
        }
        else
        {
            return insert_aux(position, value);
        }
    }

    // 在 position 处插入元素
    template <class T, class Alloc>
    typename deque<T, Alloc>::iterator
    deque<T, Alloc>::insert(iterator position, value_type&& value)
    {
        if (position.cur == begin_.cur)
        {
            emplace_front(mystl::move(value));
            return begin_;
        }
        else if (position.cur == end_.cur)
        {
            emplace_back(mystl::move(value));
            auto tmp = end_;
            --tmp;
            return tmp;
        }
        else
        {
            return insert_aux(position, mystl::move(value));
        }
    }


    // 在 position 位置插入 n 个元素
    template <class T, class Alloc>
    void deque<T, Alloc>::insert(iterator position, size_type n, const value_type& value)
    {
        if (position.cur == begin_.cur)
        {
            require_capacity(n, true);
            auto new_begin = begin_ - n;
            mystl::uninitialized_fill_n(new_begin, n, value);
            begin_ = new_begin;
        }
        else if (position.cur == end_.cur)
        {
            require_capacity(n, false);
            auto new_end = end_ + n;
            mystl::uninitialized_fill_n(end_, n, value);
            end_ = new_end;
        }
        else
        {
            fill_insert(position, n, value);
        }
    }


    // 删除 position 处的元素
    // O(n)复杂度，需要移动前放或者后方元素
    template <class T, class Alloc>
    typename deque<T, Alloc>::iterator
    deque<T, Alloc>::erase(iterator position)
    {
        auto next = position;
        ++next;
        const size_type elems_before = position - begin_;
        if (elems_before < (size() / 2))
        {
            mystl::copy_backward(begin_, position, next);
            pop_front();
        }
        else
        {
            mystl::copy(next, end_, position);
            pop_back();
        }
        return begin_ + elems_before;
    }


    // 删除[first, last)上的元素
    template <class T, class Alloc>
    typename deque<T, Alloc>::iterator
    deque<T, Alloc>::erase(iterator first, iterator last)
    {
        if (first == begin_ && last == end_)
        {
            clear();
            return end_;
        }
        else
        {
            const size_type len = last - first;
            const size_type elems_before = first - begin_;
            if (elems_before < ((size() - len) / 2))
            {
                mystl::copy_backward(begin_, first, last);
                auto new_begin = begin_ + len;
                destroy(begin_.cur, new_begin.cur);
                begin_ = new_begin;
            }
            else
            {
                mystl::copy(last, end_, first);
                auto new_end = end_ - len;
                destroy(new_end.cur, end_.cur);
                end_ = new_end;
            }
            return begin_ + elems_before;
        }
    }


    // 重置容器大小
    template<class T, class Alloc>
    void deque<T, Alloc>::resize(size_type new_size, const value_type& val) {
        const auto len = size();
        if (len > new_size) {
            erase(begin_ + new_size, end_);
        }
        else {
            insert(end_, new_size - len, val);
        }
    }


    // 减小容器容量
    template <class T, class Alloc>
    void deque<T, Alloc>::shrink_to_fit() noexcept
    {
        // 至少会留下头部缓冲区
        for (auto cur = map_; cur < begin_.node; ++cur)
        {
            deallocate_node(*cur);
            *cur = nullptr;
        }
        for (auto cur = end_.node + 1; cur < map_ + map_size_; ++cur)
        {
            deallocate_node(*cur);
            *cur = nullptr;
        }
    }

    // 清空 deque
    template <class T, class Alloc>
    void deque<T, Alloc>::clear()
    {
        // clear 会保留头部的缓冲区
        for (map_pointer cur = begin_.node + 1; cur < end_.node; ++cur)
        {
            destroy(*cur, *cur + buffer_size());
        }
        if (begin_.node != end_.node)
        { // 有两个以上的缓冲区
            mystl::destroy(begin_.cur, begin_.last);
            mystl::destroy(end_.first, end_.cur);
        }
        else
        {
            mystl::destroy(begin_.cur, end_.cur);
        }
        shrink_to_fit();
        end_ = begin_;
    }


    // 头部就地构造元素
    template<class T, class Alloc>
    template<class... Args>
    void deque<T, Alloc>::emplace_front(Args&&...args) {
        if (begin_.cur != begin_.first) {
            construct(begin_.cur - 1, mystl::forward<Args>(args)...);
            --begin_.cur;
        }
        else {
            require_capacity(1, true);
            try {
                --begin_;
                construct(begin_.cur, mystl::forward<Args>(args)...);
            } catch(...) {
                ++begin_;
                throw;
            }
        }
    }

    // 尾部就地构造元素
    template<class T, class Alloc>
    template<class... Args>
    void deque<T, Alloc>::emplace_back(Args&&...args) {
        if (end_.cur != end_.last - 1) {
            construct(end_.cur, mystl::forward<Args>(args)...);
            ++end_.cur;
        }
        else {
            require_capacity(1, false);
            construct(end_.cur, mystl::forward<Args>(args)...);
            ++end_;
        }
    }

    // 在 pos 位置就地构建元素
    template <class T, class Alloc>
    template <class ...Args>
    typename deque<T, Alloc>::iterator deque<T, Alloc>::emplace(iterator pos, Args&& ...args)
    {
        if (pos.cur == begin_.cur)
        {
            emplace_front(mystl::forward<Args>(args)...);
            return begin_;
        }
        else if (pos.cur == end_.cur)
        {
            emplace_back(mystl::forward<Args>(args)...);
            return end_ - 1;
        }
        return insert_aux(pos, mystl::forward<Args>(args)...);
    }


    // 交换两个 deque
    template <class T, class Alloc>
    void deque<T, Alloc>::swap(deque& rhs) noexcept
    {
        if (this != &rhs)
        {
            mystl::swap(begin_, rhs.begin_);
            mystl::swap(end_, rhs.end_);
            mystl::swap(map_, rhs.map_);
            mystl::swap(map_size_, rhs.map_size_);
        }
    }

    // copy_init 函数
    template <class T, class Alloc>
    template <class IIter>
    void deque<T, Alloc>::
    copy_init(IIter first, IIter last, input_iterator_tag)
    {
        const size_type n = mystl::distance(first, last);
        initialize_map(n);
        for (; first != last; ++first)
            emplace_back(*first);
    }


    template <class T, class Alloc>
    template <class FIter>
    void deque<T, Alloc>::
    copy_init(FIter first, FIter last, forward_iterator_tag)
    {
        const size_type n = mystl::distance(first, last);
        initialize_map(n);
        for (auto cur = begin_.node; cur < end_.node; ++cur)
        {
            auto next = first;
            mystl::advance(next, buffer_size());
            mystl::uninitialized_copy(first, next, *cur);
            first = next;
        }
        mystl::uninitialized_copy(first, last, end_.first);
    }

    // fill_assign 函数
    template <class T, class Alloc>
    void deque<T, Alloc>::
    fill_assign(size_type n, const value_type& value)
    {
        if (n > size())
        {
            mystl::fill(begin(), end(), value);
            insert(end(), n - size(), value);
        }
        else
        {
            erase(begin() + n, end());
            mystl::fill(begin(), end(), value);
        }
    }

    // copy_assign 函数
    template <class T, class Alloc>
    template <class IIter>
    void deque<T, Alloc>::
    assign_aux(IIter first, IIter last, input_iterator_tag)
    {
        auto first1 = begin();
        auto last1 = end();
        for (; first != last && first1 != last1; ++first, ++first1)
        {
            *first1 = *first;
        }
        if (first1 != last1)
        {
            erase(first1, last1);
        }
        else
        {
            insert_dispatch(end_, first, last, input_iterator_tag{});
        }
    }


    template <class T, class Alloc>
    template <class FIter>
    void deque<T, Alloc>::
    assign_aux(FIter first, FIter last, forward_iterator_tag)
    {  
        const size_type len1 = size();
        const size_type len2 = mystl::distance(first, last);
        if (len1 < len2)
        {
            auto next = first;
            mystl::advance(next, len1);
            mystl::copy(first, next, begin_);
            insert_dispatch(end_, next, last, forward_iterator_tag{});
        }
        else
        {
            erase(mystl::copy(first, last, begin_), end_);
        }
    }


    // insert_aux 函数
    template <class T, class Alloc>
    template <class... Args>
    typename deque<T, Alloc>::iterator
    deque<T, Alloc>::insert_aux(iterator position, Args&& ...args)
    {
        const size_type elems_before = position - begin_;
        value_type value_copy = value_type(mystl::forward<Args>(args)...);
        if (elems_before < (size() / 2))
        { // 在前半段插入
            emplace_front(front());
            auto front1 = begin_;
            ++front1;
            auto front2 = front1;
            ++front2;
            position = begin_ + elems_before;
            auto pos = position;
            ++pos;
            mystl::copy(front2, pos, front1);
        }
        else
        { // 在后半段插入
            emplace_back(back());
            auto back1 = end_;
            --back1;
            auto back2 = back1;
            --back2;
            position = begin_ + elems_before;
            mystl::copy_backward(position, back2, back1);
        }
        *position = mystl::move(value_copy);
        return position;
    }


    // fill_insert 函数
    template <class T, class Alloc>
    void deque<T, Alloc>::
    fill_insert(iterator position, size_type n, const value_type& value)
    {
    const size_type elems_before = position - begin_;
    const size_type len = size();
    auto value_copy = value;
    if (elems_before < (len / 2))
    {
        require_capacity(n, true);
        // 原来的迭代器可能会失效
        auto old_begin = begin_;
        auto new_begin = begin_ - n;
        position = begin_ + elems_before;
        try
        {
        if (elems_before >= n)
        {
            auto begin_n = begin_ + n;
            mystl::uninitialized_copy(begin_, begin_n, new_begin);
            begin_ = new_begin;
            mystl::copy(begin_n, position, old_begin);
            mystl::fill(position - n, position, value_copy);
        }
        else
        {
            mystl::uninitialized_fill(
            mystl::uninitialized_copy(begin_, position, new_begin), begin_, value_copy);
            begin_ = new_begin;
            mystl::fill(old_begin, position, value_copy);
        }
        }
        catch (...)
        {
        if (new_begin.node != begin_.node)
            destroy_nodes(new_begin.node, begin_.node - 1);
        throw;
        }
    }
    else
    {
        require_capacity(n, false);
        // 原来的迭代器可能会失效
        auto old_end = end_;
        auto new_end = end_ + n;
        const size_type elems_after = len - elems_before;
        position = end_ - elems_after;
        try
        {
        if (elems_after > n)
        {
            auto end_n = end_ - n;
            mystl::uninitialized_copy(end_n, end_, end_);
            end_ = new_end;
            mystl::copy_backward(position, end_n, old_end);
            mystl::fill(position, position + n, value_copy);
        }
        else
        {
            mystl::uninitialized_fill(end_, position + n, value_copy);
            mystl::uninitialized_copy(position, end_, position + n);
            end_ = new_end;
            mystl::fill(position, old_end, value_copy);
        }
        }
        catch (...)
        {
        if(new_end.node != end_.node)
            destroy_nodes(end_.node + 1, new_end.node);
        throw;
        }
    }
    }


    // copy_insert
    template <class T, class Alloc>
    template <class FIter>
    void deque<T, Alloc>::
    copy_insert(iterator position, FIter first, FIter last, size_type n)
    {
        const size_type elems_before = position - begin_;
        auto len = size();
        if (elems_before < (len / 2))
        {
            require_capacity(n, true);
            // 原来的迭代器可能会失效
            auto old_begin = begin_;
            auto new_begin = begin_ - n;
            position = begin_ + elems_before;
            try
            {
            if (elems_before >= n)
            {
                auto begin_n = begin_ + n;
                mystl::uninitialized_copy(begin_, begin_n, new_begin);
                begin_ = new_begin;
                mystl::copy(begin_n, position, old_begin);
                mystl::copy(first, last, position - n);
            }
            else
            {
                auto mid = first;
                mystl::advance(mid, n - elems_before);
                mystl::uninitialized_copy(first, mid,
                                        mystl::uninitialized_copy(begin_, position, new_begin));
                begin_ = new_begin;
                mystl::copy(mid, last, old_begin);
            }
            }
            catch (...)
            {
            if(new_begin.node != begin_.node)
                destroy_nodes(new_begin.node, begin_.node - 1);
            throw;
            }
        }
        else
        {
            require_capacity(n, false);
            // 原来的迭代器可能会失效
            auto old_end = end_;
            auto new_end = end_ + n;
            const auto elems_after = len - elems_before;
            position = end_ - elems_after;
            try
            {
            if (elems_after > n)
            {
                auto end_n = end_ - n;
                mystl::uninitialized_copy(end_n, end_, end_);
                end_ = new_end;
                mystl::copy_backward(position, end_n, old_end);
                mystl::copy(first, last, position);
            }
            else
            {
                auto mid = first;
                mystl::advance(mid, elems_after);
                mystl::uninitialized_copy(position, end_,
                                        mystl::uninitialized_copy(mid, last, end_));
                end_ = new_end;
                mystl::copy(first, mid, position);
            }
            }
            catch (...)
            {
            if(new_end.node != end_.node)
                destroy_nodes(end_.node + 1, new_end.node);
            throw;
            }
        }
    }


    // insert_dispatch 函数
    template <class T, class Alloc>
    template <class IIter>
    void deque<T, Alloc>::
    insert_dispatch(iterator position, IIter first, IIter last, input_iterator_tag)
    {
        if (last <= first)  return;
        const size_type n = mystl::distance(first, last);
        const size_type elems_before = position - begin_;
        if (elems_before < (size() / 2))
        {
            require_capacity(n, true);
        }
        else
        {
            require_capacity(n, false);
        }
        position = begin_ + elems_before;
        auto cur = --last;
        for (size_type i = 0; i < n; ++i, --cur)
        {
            insert(position, *cur);
        }
    }


    template <class T, class Alloc>
    template <class FIter>
    void deque<T, Alloc>::
    insert_dispatch(iterator position, FIter first, FIter last, forward_iterator_tag)
    {
        if (last <= first)  return;
        const size_type n = mystl::distance(first, last);
        if (position.cur == begin_.cur)
        {
            require_capacity(n, true);
            auto new_begin = begin_ - n;
            try
            {
            mystl::uninitialized_copy(first, last, new_begin);
            begin_ = new_begin;
            }
            catch (...)
            {
            if(new_begin.node != begin_.node)
                destroy_nodes(new_begin.node, begin_.node - 1);
            throw;
            }
        }
        else if (position.cur == end_.cur)
        {
            require_capacity(n, false);
            auto new_end = end_ + n;
            try
            {
            mystl::uninitialized_copy(first, last, end_);
            end_ = new_end;
            }
            catch (...)
            {
            if(new_end.node != end_.node)
                destroy_nodes(end_.node + 1, new_end.node);
            throw;
            }
        }
        else
        {
            copy_insert(position, first, last, n);
        }
    }


    // 重载比较操作符
    template <class T>
    bool operator==(const deque<T>& lhs, const deque<T>& rhs)
    {
    return lhs.size() == rhs.size() && 
        std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <class T>
    bool operator<(const deque<T>& lhs, const deque<T>& rhs)
    {
    return mystl::lexicographical_compare(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <class T>
    bool operator!=(const deque<T>& lhs, const deque<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T>
    bool operator>(const deque<T>& lhs, const deque<T>& rhs)
    {
        return rhs < lhs;
    }

    template <class T>
    bool operator<=(const deque<T>& lhs, const deque<T>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T>
    bool operator>=(const deque<T>& lhs, const deque<T>& rhs)
    {
        return !(lhs < rhs);
    }

    // 重载 mystl 的 swap
    template <class T>
    void swap(deque<T>& lhs, deque<T>& rhs)
    {
        lhs.swap(rhs);
    }

} // namespace mystl