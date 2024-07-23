#pragma once

#include "algobase.h"
#include "allocator.h"
#include "uninitialized.h"
#include "construct.h"

#include <cstddef>// ptrdiff_t

namespace mystl {
    // use sub_allocator as default allocator
    template<class T, class Alloc = simpleAlloc<T>>
    class vector {
    public:
        using value_type = T;
        using pointer = value_type*;
        using iterator = value_type*;// 原生指针作迭代器
        using const_iterator = const value_type*;
        using reverse_iterator = __reverse_iterator<iterator>;
        using const_reverse_iterator = __reverse_iterator<const_iterator>;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

    private:// data member
        // iterator to indicate the vector's memory location
        iterator start;
        iterator finish;
        iterator end_of_storage;

    private:// allocate and construct aux functions
        using data_allocator = Alloc;

        void fill_initialize(size_type n, const value_type& value) {
            start = allocate_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }
        template<class Integer>
        void initialize_aux(Integer n, Integer val, true_type) {
            fill_initialize(static_cast<size_type>(n),
                static_cast<value_type>(val));
        }
        template<class InputIterator>
        void initialize_aux(InputIterator first, InputIterator last, false_type) {
            start = allocate_and_copy(first, last);
            finish = end_of_storage = start + mystl::distance(first, last);
        }
        iterator allocate_and_fill(size_type n, const value_type& value) {
            iterator result = data_allocator::allocate(n);
            mystl::uninitialized_fill_n(result, n, value);
            return result;
        }
        template<class InputIterator>
        iterator allocate_and_copy(InputIterator first, InputIterator last) {
            size_type n = mystl::distance(first, last);
            iterator result = data_allocator::allocate(n);
            mystl::uninitialized_copy(first, last, result);
            return result;
        }
        void deallocate() noexcept {
            if (start) data_allocator::deallocate(start, end_of_storage - start);
        }
        void destroy_and_deallocate() noexcept {
            mystl::destroy(start, finish);// destroy in "construct.h"
            deallocate();
        }

    public:// swap
        void swap(vector&) noexcept;

    public:// ctor && dtor
        vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}
        explicit vector(size_type n) { fill_initialize(n, value_type()); }
        vector(size_type n, const value_type& value) { fill_initialize(n, value); }
        // 传入迭代器构造新vector， vector<T>(vec.begin(), vec.end());
        // InputIterator 可能会被推导为size_type，因此需要is_integral_t
        template<class InputIterator>
        vector(InputIterator first, InputIterator last) {
            initialize_aux(first, last, is_integral<InputIterator>());
        }
        vector(std::initializer_list<T>);
        vector(const vector&); // 复制构造函数
        vector(vector&&) noexcept; // 移动构造

        ~vector() {
            mystl::destroy(start, finish);// destory in "construct.h"
            deallocate();
        }

    public:// 重载赋值运算符
        vector& operator=(const vector&);

    public:// 移动赋值
        vector& operator=(vector&&) noexcept;

    public:// getter
        const_iterator begin() const noexcept { return start; }
        const_iterator end() const noexcept { return finish; }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(finish); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(start); }
        const_iterator cbegin() const noexcept { return start; }
        const_iterator cend() const noexcept { return finish; }
        const_reverse_iterator crbegin() const noexcept {
            return const_reverse_iterator(finish);
        }
        const_reverse_iterator crend() const noexcept {
            return const_reverse_iterator(start);
        }
        // TODO:
        // have no empty check
        const_reference front() const noexcept { return *begin(); }
        const_reference back() const noexcept { return *(end() - 1); }
        // TODO:
        // have no boundary check and don't use proxy
        const_reference operator[](const size_type n) const noexcept {
            return *(start + n);
        }
        size_type size() const noexcept {
            return static_cast<size_type>(finish - start);
        }
        size_type capacity() const noexcept {
            return static_cast<size_type>(end_of_storage - start);
        }
        bool empty() const noexcept { return start == finish; }

    public:// setter
        iterator begin() noexcept { return start; }
        iterator end() noexcept { return finish; }
        reverse_iterator rbegin() noexcept { return reverse_iterator(finish); }
        reverse_iterator rend() noexcept { return reverse_iterator(start); }
        reference operator[](const size_type n) noexcept { return *(start + n); }
        reference front() noexcept { return *begin(); }
        reference back() noexcept { return *(end() - 1); }

    public:// interface for size and capacity
        void resize(size_type, const value_type&);
        void resize(size_type new_size) { resize(new_size, value_type()); }
        void reserve(size_type);
        void shrink_to_fit() noexcept {
            vector temp(*this);
            swap(temp);
        }

    public:// compare operator(member function)
        bool operator==(const vector&) const noexcept;
        bool operator!=(const vector& rhs) const noexcept {
            return !(*this == rhs);
        }

    public:// push && pop
        void push_back(const value_type&);
        void pop_back() {
            --finish;
            destroy(finish);
        }
        void push_back(value_type&& value) { emplace_back(mystl::move(value)); }          // 移动数据

    public:// erase
        iterator erase(iterator, iterator);
        iterator erase(iterator position) { return erase(position, position + 1); }
        void clear() { erase(begin(), end()); }

    private:// aux_interface for insert
        void insert_aux(iterator, const value_type&);                                   // 插入单个值
        void fill_insert(iterator, size_type, const value_type&);                       // 插入多个值
        template<class InputIterator>
        void range_insert(iterator pos, InputIterator first, InputIterator last,        // 范围插入（输入迭代器）
            input_iterator_tag);
        template<class ForwardIterator>
        void range_insert(iterator pos, ForwardIterator first, ForwardIterator last,    // 范围插入（前向迭代器）
            forward_iterator_tag);
        template<class Integer>
        void insert_dispatch(iterator pos, Integer n, Integer value, true_type) {       // 范围插入的两个重载（针对整型和迭代器输入参数）
            fill_insert(pos, static_cast<int>(n), value_type(value));
        }
        template<class InputIterator>
        void insert_dispatch(iterator pos, InputIterator first, InputIterator last,
            false_type) {
            range_insert(pos, first, last, iterator_category_t<InputIterator>());
        }

    public:// insert
        iterator insert(iterator);
        iterator insert(iterator, const value_type&);
        // insert n values at pos
        void insert(iterator pos, size_type n, const value_type& val);
        template<class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last) {
            insert_dispatch(pos, first, last, is_integral<InputIterator>());
        }
        iterator insert(iterator pos, value_type&& value) { return emplace(pos, mystl::move(value)); }

    private:// aux_interface for assign
        void fill_assign(size_type, const value_type&);
        template<class Integer>
        void assign_dispatch(Integer n, Integer val, true_type) {
            fill_assign(static_cast<size_type>(n), static_cast<value_type>(val));
        }
        template<class InputIterator>
        void assign_dispatch(InputIterator first, InputIterator last, false_type) {
            assign_aux(first, last, iterator_category_t<InputIterator>());
        }
        template<class InputIterator>
        void assign_aux(InputIterator first, InputIterator last,
            input_iterator_tag);
        template<class ForwardIterator>
        void assign_aux(ForwardIterator first, ForwardIterator last,
            forward_iterator_tag);

    public:// assign
        void assign(size_type n, const value_type& val) { fill_assign(n, val); }
        template<class InputIterator>
        void assign(InputIterator first, InputIterator last) {
            assign_dispatch(first, last, is_integral<InputIterator>());
        }
        void assign(std::initializer_list<value_type> ils) {
            assign(ils.begin(), ils.end());
        }
        vector& operator=(std::initializer_list<value_type> ils) {
            assign(ils);
            return *this;
        }

    public:
        template<class... Args>
        iterator emplace(iterator pos, Args&& ...args);

        template<class... Args>
        iterator emplace_back(Args&& ...args);
       
    };

    // 在position位置插入一个值，插入多个值的特殊情况
    template<class T, class Alloc>
    void vector<T, Alloc>::insert_aux(iterator position, const value_type& value) {
        if (finish != end_of_storage) {// needn't expand
            construct(finish, *(finish - 1));
            ++finish;
            value_type value_copy = value;// STL copy in copy out
            mystl::copy_backward(position, finish - 2, finish - 1);
            *position = value_copy;
        }
        else {// expand
            const size_type old_size = size();      
            const size_type new_size =                                  // old_size == 0的话申请空间1，否则申请2 * old_size
                old_size ? 2 * old_size : 1;
            iterator new_start = data_allocator::allocate(new_size);
            iterator new_finish = new_start;
            try {
                new_finish = mystl::uninitialized_copy(
                    start, position, new_start);                        // Copy the first segment
                construct(new_finish, value);
                ++new_finish;
                new_finish = mystl::uninitialized_copy(
                    position, finish, new_finish);                      // Copy the second segment
            }
            catch (std::exception&) {
                // commit or rollback
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, new_size);
                throw;
            }
            destroy_and_deallocate();
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + new_size;
        }
    }

    // 范围插入（输入迭代器，标准库的容器至少为前向迭代器）
    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::range_insert(iterator pos, InputIterator first,
        InputIterator last, input_iterator_tag) {
        for (; first != last; ++first) {
            pos = insert(pos, *first);
            ++pos;
        }
    }

    // 范围插入，所有STL容器都匹配这个重载
    template<class T, class Alloc>
    template<class ForwardIterator>
    void vector<T, Alloc>::range_insert(iterator position, ForwardIterator first,
        ForwardIterator last,
        forward_iterator_tag) {
        if (first != last) {
            size_type n = mystl::distance(first, last);
            if (static_cast<size_type>(end_of_storage - finish) >= n) {
                const size_type elems_after = finish - position;
                iterator old_finish = finish;
                if (elems_after > n) {
                    mystl::uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    mystl::copy_backward(position, old_finish - n, old_finish);
                    mystl::copy(first, last, position);                             // 这里应该是first, last, position
                }
                else {                                                              // 三次拷贝
                    ForwardIterator mid = first;
                    advance(mid, elems_after);
                    mystl::uninitialized_copy(mid, last, finish);
                    finish += n - elems_after;
                    mystl::uninitialized_copy(position, old_finish, finish);
                    finish += elems_after;
                    mystl::copy(first, mid, position);
                }
            }
            else {// expand
                const size_type old_size = size();
                const size_type new_size = old_size + mystl::max(old_size, n);
                iterator new_start = data_allocator::allocate(new_size);
                iterator new_finish = new_start;
                try {
                    new_finish =
                        mystl::uninitialized_copy(start, position, new_start);
                    new_finish =
                        mystl::uninitialized_copy(first, last, new_finish);
                    new_finish =
                        mystl::uninitialized_copy(position, finish, new_finish);
                }
                catch (std::exception&) {
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, new_size);
                    throw;
                }
                destroy_and_deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + new_size;
            }
        }
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::swap(vector& rhs) noexcept {
        if (this != &rhs) {
            mystl::swap(start, rhs.start);
            mystl::swap(finish, rhs.finish);
            mystl::swap(end_of_storage, rhs.end_of_storage);
        }
    }


    template<class T, class Alloc>
    inline void vector<T, Alloc>::resize(size_type new_size, const value_type& value) {
        if (new_size < size()) {
            erase(begin() + new_size, end());
        }
        else
            fill_insert(end(), new_size - size(), value);
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::reserve(size_type new_capacity) {
        if (new_capacity <= capacity()) return;
        T* new_start = data_allocator::allocate(new_capacity);
        T* new_finish = mystl::uninitialized_copy(start, finish, new_start);
        destroy_and_deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = start + new_capacity;
    }

    template<class T, class Alloc>
    bool vector<T, Alloc>::operator==(const vector& rhs) const noexcept {
        if (size() != rhs.size()) {
            return false;
        }
        else {
            iterator ptr1 = start;
            iterator ptr2 = rhs.start;
            for (; ptr1 != finish && ptr2 != rhs.finish; ++ptr1, ++ptr2)
                if (*ptr1 != *ptr2) return false;
            return true;
        }
    }

    template<class T, class Alloc>
    inline void vector<T, Alloc>::push_back(const value_type& value) {
        if (finish != end_of_storage) {
            construct(finish, value);
            ++finish;
        }
        else
            insert_aux(end(), value);
    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(
        iterator first, iterator last) {
        iterator i = mystl::copy(last, finish, first);
        mystl::destroy(i, finish);
        finish -= (last - first);
        return first;
    }

    template<class T, class Alloc>
    void vector<T, Alloc>::fill_insert(iterator position, size_type n,
        const value_type& value) {
        if (n <= 0) {
            return;
        }
        if (static_cast<size_type>(end_of_storage - finish) >= n) {         // 备用空间大于等于“新增元素个数”
            value_type value_copy = value;
            const size_type elems_after = finish - position;                // 插入点之后的元素个数
            iterator old_finish = finish;
            if (elems_after > n) {                                          // 插入点之后元素个数大于新增元素个数
                mystl::uninitialized_copy(finish - n, finish, finish);      // 将finish前的n个元素复制到finish之后
                finish += n;
                mystl::copy_backward(position, old_finish - n, old_finish); // copy_backward从后往前复制，同样左闭右开
                mystl::fill(position, position + n, value_copy);            // 最后将新元素填充到position位置
            }
            else {                                                          // 插入点之后元素个数小于等于新增元素个数
                mystl::uninitialized_fill_n(finish, n - elems_after,        // 先填充多出来的元素，然后拷贝插入点后元素，最后覆盖剩余元素
                    value_copy);
                finish += n - elems_after;
                mystl::uninitialized_copy(position, old_finish, finish);
                finish += elems_after;
                mystl::fill(position, old_finish, value_copy);// complement
            }
        }
        else {// expand
            const size_type old_size = size();
            const size_type new_size = old_size + mystl::max(old_size, n);  // 扩容到原来的两倍或者old_size + n大小
            iterator new_start = data_allocator::allocate(new_size);        // 拷贝数据到新内存中
            iterator new_finish = new_start;
            try {
                new_finish =
                    mystl::uninitialized_copy(start, position, new_start);
                new_finish =
                    mystl::uninitialized_fill_n(new_finish, n, value);
                new_finish =
                    mystl::uninitialized_copy(position, finish, new_finish);
            }
            catch (std::exception&) {
                mystl::destroy(new_start, new_finish);                      // 调用析构函数、释放原内存空间
                data_allocator::deallocate(new_start, new_size);
                throw;
            }
            destroy_and_deallocate();
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + new_size;
        }
    }


    // insert外部接口定义
    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(
        iterator position) {
        return insert(position, value_type());
    }

    template<class T, class Alloc>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(
        iterator position, const value_type& value) {
        size_type n = position - begin();
        if (finish != end_of_storage && position == end()) {
            construct(finish, value);
            ++finish;
        }
        else
            insert_aux(position, value);
        return begin() + n;
    }

    // 在position位置插入多个值
    template<class T, class Alloc>
    void vector<T, Alloc>::insert(
        iterator position, size_type n, const value_type& value) {
        fill_insert(position, n, value);
    }

    // 接受n, val参数，清空原内容
    template<class T, class Alloc>
    void vector<T, Alloc>::fill_assign(size_type n, const value_type& val) {
        if (n > capacity()) {
            vector<T, Alloc> temp(n, val);
            temp.swap(*this);
        }
        else if (n > size()) {
            mystl::fill(begin(), end(), val);
            finish = mystl::uninitialized_fill_n(finish, n - size(), val);
        }
        else
            erase(mystl::fill_n(begin(), n, val), end());
    }

    // 接受迭代器参数
    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::assign_aux(InputIterator first, InputIterator last,
        input_iterator_tag) {
        iterator cur = begin();
        for (; first != last && cur != end(); ++cur, ++first) *cur = *first;
        if (first == last)
            erase(cur, end());
        else
            insert(end(), first, last);                             // 原数据数量小于分配的元素数量，在尾端插入剩余元素
    }

    template<class T, class Alloc>
    template<class ForwardIterator>
    void vector<T, Alloc>::assign_aux(ForwardIterator first, ForwardIterator last,
        forward_iterator_tag) {
        size_type len = mystl::distance(first, last);
        if (len > capacity()) {
            iterator temp = allocate_and_copy(first, last);
            destroy_and_deallocate();
            start = temp;
            end_of_storage = finish = start + len;
        }
        else if (size() >= len) {
            iterator new_finish = mystl::copy(first, last, start);
            destroy(new_finish, finish);                           // 删除多余数据但不释放空间
            finish = new_finish;
        }
        else {                                                     // 新的元素个数大于原本元素个数但小于capacity
            ForwardIterator mid = first;
            mystl::advance(mid, size());
            mystl::copy(first, mid, start);
            finish = mystl::uninitialized_copy(mid, last, finish);
        }
    }

    // 在pos位置构造元素，如果空间不够就重新分配再构造元素
    template<class T, class Alloc>
    template<class... Args>
    inline typename vector<T, Alloc>::iterator vector<T, Alloc>::emplace(iterator pos, Args&&... args) {
        const size_type n = pos - start;
        if (finish != end_of_storage && pos == finish) {
            construct(pos, mystl::forward<Args>(args)...);
            ++finish;
        }
        else if (finish != end_of_storage) {
            iterator new_end = finish;
            construct(finish, *(finish - 1));
            ++new_end;
            mystl::copy_backward(pos, finish - 1, finish);
            construct(pos, mystl::forward<Args>(args)...);
            finish = new_end;
        }
        else { // reallocate and emplace
            const size_type old_size = size();
            const size_type new_size = old_size + mystl::max(old_size, static_cast<const size_type>(1));
            iterator new_start = data_allocator::allocate(new_size);
            iterator new_finish = new_start;
            try {
                new_finish = mystl::uninitialized_copy(start, pos, new_start);
                construct(new_finish, mystl::forward<Args>(args)...);
                ++new_finish;
                new_finish = mystl::uninitialized_copy(pos, finish, new_finish);
            }
            catch (std::exception&) {
                mystl::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, new_size);
                throw;
            }
            destroy_and_deallocate();
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + new_size;
        }
        return begin() + n;
    }

    // 尾部构造元素
    template<class T, class Alloc>
    template<class... Args>
    inline typename vector<T, Alloc>::iterator
    vector<T, Alloc>::emplace_back(Args&&... args) {
        if (finish == end_of_storage) {
            emplace(finish, mystl::forward<Args>(args)...);
        }
        else {
            construct(finish, mystl::forward<Args>(args)...);
            ++finish;
        }
        return finish;
    }


    // 复制构造函数
    template<class T, class Alloc>
    inline vector<T, Alloc>::vector(const vector& rhs) {
        start = allocate_and_copy(rhs.begin(), rhs.end());
        finish = end_of_storage = start + rhs.size();
    }

    template<class T, class Alloc>
    inline vector<T, Alloc>::vector(std::initializer_list<T> il) { // std::initializer_list代表用花括号初始化vector
        start = allocate_and_copy(il.begin(), il.end());
        finish = end_of_storage = start + il.size();
    }

    // 移动构造函数
    template<class T, class Alloc>
    inline vector<T, Alloc>::vector(vector&& rhs) noexcept {
        start = rhs.start;
        finish = rhs.finish;
        end_of_storage = rhs.end_of_storage;
        rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
    }

    // 重载赋值运算符
    template<class T, class Alloc>
    inline vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& rhs) {
        vector temp(rhs);
        swap(temp);
        return *this;
    }

    // 重载移动赋值运算符
    template<class T, class Alloc>
    inline vector<T, Alloc>& vector<T, Alloc>::operator=(vector&& rhs) noexcept {
        if (this != &rhs) {
            destroy_and_deallocate();
            start = rhs.start;
            finish = rhs.finish;
            end_of_storage = rhs.end_of_storage;
            rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
        }
        return *this;
    }

    template<class T, class Alloc>
    inline bool operator==(const vector<T, Alloc>& lhs,
        const vector<T, Alloc>& rhs) {
        return lhs.operator==(rhs);
    }

    template<class T, class Alloc>
    inline bool operator!=(const vector<T, Alloc>& lhs,
        const vector<T, Alloc>& rhs) {
        return !(lhs == rhs);
    }

    // compare operator
    template<class T>
    inline bool operator<(const vector<T>& lhs, const vector<T>& rhs) {
        return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
            rhs.end());// in stl_algobase.h
    }

    template<class T>
    inline bool operator>(const vector<T>& lhs, const vector<T>& rhs) {
        return rhs < lhs;
    }

    template<class T>
    inline bool operator<=(const vector<T>& lhs, const vector<T>& rhs) {
        return !(rhs < lhs);
    }

    template<class T>
    inline bool operator>=(const vector<T>& lhs, const vector<T>& rhs) {
        return !(lhs < rhs);
    }

    template<class T, class Alloc>
    inline void swap(vector<T, Alloc>& lhs, vector<T, Alloc>& rhs) noexcept {
        lhs.swap(rhs);
    }

}// namespace mystl