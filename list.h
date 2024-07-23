#pragma
#include "iterator.h"
#include "algobase.h"
#include "type_traits.h"
#include "allocator.h"


// 节点定义
namespace mystl {
    template<class T>
    struct __list_node {
        T data;
        __list_node* prev;
        __list_node* next;
    };
}


// 迭代器定义
namespace mystl {
    template<class T>
    struct __list_iterator {
        using iterator = __list_iterator<T>;
        using self = __list_iterator<T>;
        using link_type = __list_node<T> *;
        using iterator_category = bidirectional_iterator_tag;
        using reference = T &;
        using pointer = T *;
        using value_type = T;
        using difference_type = ptrdiff_t;

        // 数据成员
        link_type node;

        __list_iterator() {}
        explicit __list_iterator(link_type pos) : node(pos) {};

        bool operator==(const self &rhs) const noexcept {return node == rhs.node;}
        bool operator!=(const self &rhs) const noexcept {return node != rhs.node;}

        reference operator*() const {return node->data;}
        pointer operator->() const {return &(node->data);}

        self &operator++() {
            node = node->next;
            return *this;
        }

        self operator++(int i) {
            self temp = *this;
            ++(*this);
            return temp;
        }

        // decreasement
        self &operator--() {
            node = node->prev;
            return *this;
        }

        self operator--(int i) {
            self temp = *this;
            --(*this);
            return temp;
        }
    };


    template<class T>
    struct __list_const_iterator {
        using iterator = __list_iterator<T>;
        using self = __list_const_iterator<T>;
        using link_type = __list_node<T> *;
        using iterator_category = bidirectional_iterator_tag;
        using reference = const T &;
        using pointer = const T *;
        using value_type = T;
        using difference_type = ptrdiff_t;

        // 数据成员
        link_type node;

        __list_const_iterator() {}
        explicit __list_const_iterator(link_type pos) : node(pos) {};
        __list_const_iterator(const iterator &x) : node(x.node) {}

        bool operator==(const self &rhs) const noexcept {return node == rhs.node;}
        bool operator!=(const self &rhs) const noexcept {return node != rhs.node;}

        reference operator*() const {return node->data;}
        pointer operator->() const {return &(node->data);}

        self &operator++() {
            node = node->next;
            return *this;
        }

        self &operator++(int i) {
            self temp = *this;
            ++(*this);
            return temp;
        }

        // decreasement
        self &operator--() {
            node = node->prev;
            return *this;
        }

        self &operator--(int i) {
            self temp = *this;
            --(*this);
            return temp;
        }
    };
} // namespace


namespace mystl {

    template<class T, class Alloc = simpleAlloc<T>>
    class list {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T*;
        using reference = T &;
        using const_reference = const T&;
        using iterator = __list_iterator<T>;
        using const_iterator = const __list_const_iterator<T>;
        using reverse_iterator = mystl::__reverse_iterator<iterator>;
        using const_reverse_iterator = mystl::__reverse_iterator<const_iterator>;

        using size_type = size_t;
        using difference_type = ptrdiff_t;

    private: // 节点创建，销毁
        using list_node = __list_node<T>;
        using list_node_allocator = simpleAlloc<list_node>;

        list_node* get_node() { return list_node_allocator::allocate();}
        void rm_node(list_node* p) {list_node_allocator::deallocate(p);}
        template<class... Args>
        list_node* create_node(Args&&... args);
        void destroy_node(list_node * p) { // 两步释放节点，释放节点的数据，释放节点本身
            destroy(&p->data);
            rm_node(p);
        }

    private: // 虚拟节点，pre指向尾节点，next指向头节点
        list_node* _node;
        size_type _size;
    
    private: // aux interface
        void empty_initialized();
        void transfer(iterator position, iterator first, iterator last);
    
    public:// resize
        void resize(size_type, const value_type &val = value_type());
    
    // 迭代器相关操作
    public:
        iterator               begin()         noexcept
        { return iterator(_node->next); }
        const_iterator         begin()   const noexcept
        { return const_iterator(_node->next); }
        iterator               end()           noexcept 
        { return iterator(_node); }
        const_iterator         end()     const noexcept
        { return const_iterator(_node); }

        reverse_iterator       rbegin()        noexcept
        { return reverse_iterator(end()); }
        const_reverse_iterator rbegin()  const noexcept
        { return reverse_iterator(end()); }
        reverse_iterator       rend()          noexcept
        { return reverse_iterator(begin()); }
        const_reverse_iterator rend()    const noexcept
        { return reverse_iterator(begin()); }

        const_iterator         cbegin()  const noexcept
        { return begin(); }
        const_iterator         cend()    const noexcept
        { return end(); }
        const_reverse_iterator crbegin() const noexcept
        { return rbegin(); }
        const_reverse_iterator crend()   const noexcept
        { return rend(); }
    
    public: // 容量操作
        size_type size() {return _size;}
        size_type empty() {return _node == _node->next;}
        value_type& front() {return *begin();}
        value_type& back() {return *(--end());}

    public: // swap
        void swap(list& rhs) noexcept {
            mystl::swap(_node, rhs._node);
            mystl::swap(_size, rhs._size);
        }
    
    public: // 构造、复制、移动、析构函数
        list() {empty_initialized();}
        explicit list(size_type n, const value_type& value = value_type());

        // 迭代器构造
        template<class InputIterator>
        list(InputIterator first, InputIterator last) {
            empty_initialized();
            insert(begin(), first, last);
        }

        list(std::initializer_list<value_type> il) {
            empty_initialized();
            insert(begin(), il.begin(), il.end());
        }

        ~list() {
            clear();
            rm_node(_node);
        }
        
        // 复制构造
        list(const list & rhs);
        list& operator=(list& rhs) noexcept;

        // 移动构造
        list(list &&rhs) noexcept {
            empty_initialized();
            mystl::swap(_node, rhs._node);
            _size = rhs._size;
        }

    private: // aux interface for assign
        void fill_assign(size_type, const value_type& val);
        template<class Interger>
        void assign_dispatch(Interger n, Interger val, true_type) {
            fill_assign(static_cast<size_type>(n), static_cast<value_type>(val));
        }

        template<class InputIterator>
        void assign_dispatch(InputIterator first, InputIterator last, false_type);

    public: // assign
        void assign(size_type n, const value_type& val) {fill_assign(n, val);}
        void assign(std::initializer_list<value_type>& il) { assign(il.begin(), il.end());}

        template <class InputIterator>
        void assign(InputIterator first, InputIterator last) {
            assign_dispatch(first, last,  is_integral<InputIterator>());
        }

        list& operator=(std::initializer_list<value_type>& il) {        // 花括号赋值
            assign(il.begin(), il.end());
            return *this;
        }
    
    private: // aux interface for insert
        void fill_insert(iterator, size_type, const value_type&);
        template<class Integer>
        void insert_dispatch(iterator pos, Integer n, Integer val, true_type) {
            fill_insert(pos, static_cast<size_type>(n), static_cast<value_type>(val));
        }

        template<class InputIterator>
        void insert_dispatch(iterator pos, InputIterator first, InputIterator last, false_type);

    public: // insert
        iterator insert(iterator pos) {return insert(pos, value_type());}
        iterator insert(iterator pos, const value_type& val);
        iterator insert(iterator pos, const value_type&& val);

        template<class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last) {
            insert_dispatch(pos, first , last, is_integral<InputIterator>());
        }
    
    public:
        iterator erase(iterator);

        iterator erase(iterator, iterator);
        
        void clear();

    public: // 添加与删除
        void push_back(const value_type& val) {insert(end(), val);}

        void push_back(const value_type&& val) {emplace_back(mystl::move(val));}

        void push_front(const value_type& val) {insert(begin(), val);}

        void push_front(const value_type&& val) { emplace_front(mystl::move(val)); }

        void pop_front() { iterator cur = begin(); erase(cur); }
        void pop_back() {
            iterator temp = end();
            erase(--temp);
        }
        template<class... Args>
        void emplace(iterator pos, Args&&... args);

        template<class... Args>
        void emplace_front(Args&&... args);

        template<class... Args>
        void emplace_back(Args&&... args);

    public:// other interface
        void unique();
        void splice(iterator pos, list &rhs) {
            if (!rhs.empty()) transfer(pos, rhs.begin(), rhs.end());
        }
        void splice(iterator, list &, iterator);
        void splice(iterator pos, list &, iterator first, iterator last) {
            if (first != last) transfer(pos, first, last);
        }
        void merge(list &);
        void reverse();
        void sort();
        void remove(const T &);
    };
    

    template<class T, class Alloc>
    template<class... Args>
    inline typename list<T, Alloc>::list_node *
    list<T, Alloc>::create_node(Args&&... args) {
        list_node *p = get_node();
        try {
            construct(&p->data, mystl::forward<Args>(args)...);
        } catch (std::exception) {
            rm_node(p);
            throw;
        }
        return p;
    }

    template<class T, class Alloc>
    inline void list<T, Alloc>::empty_initialized() {
        _node = get_node();
        _node->prev = _node;
        _node->next = _node;
        _size = 0;
    }

    template<class T, class Alloc>
    inline void list<T, Alloc>::resize(size_type new_size, const value_type &val) {
        iterator cur = begin();
        size_type len = 0;
        for (; cur != end() && len < new_size; ++cur, ++len);
        if (len == new_size)
            erase(cur, end());
        else// i == end()
            fill_insert(end(), new_size - len, val);
    }

    // 构造函数(初始化n个value)
    template<class T, class Alloc>
    inline list<T, Alloc>::list(size_type n, const value_type& value) {
        empty_initialized();
        fill_insert(begin(), n, value);
    }

    // 拷贝构造
    template<class T, class Alloc>
    inline list<T, Alloc>::list(const list& rhs) {
        empty_initialized();
        insert(begin(), rhs.begin(), rhs.end());
    }


    // 赋值重载
    template<class T, class Alloc>
    list<T, Alloc>& list<T, Alloc>::operator=(list& rhs) noexcept {
        list temp(rhs);
        swap(temp);
        return *this;
    }

    // 填充赋值
    template<class T, class Alloc>
    void list<T, Alloc>::fill_assign(size_type n, const value_type& val) {
        iterator i = begin();
        for (; i != end(); ++i, --n) *i = val;
        if (n > 0)
            fill_insert(end(), n, val);
        else
            erase(i, end());
        _size += n;
    }

    // 非整形的dispatch重载
    template<class T, class Alloc>
    template<class InputIterator>
    void list<T, Alloc>::assign_dispatch(InputIterator first, InputIterator last, false_type) {
        iterator start = begin();
        iterator finish = end();
        for (; start != finish && first != last; ++start, ++first) {
            *start = *first;
        }
        if (first == last)
            erase(start, finish);
        else
            insert(finish, first, last);
    }


    // insert相关函数
    template<class T, class Alloc>
    typename list<T, Alloc>::iterator
    list<T, Alloc>::insert(iterator pos, const value_type& val) {
        list_node * temp = create_node(val);
        temp->next = pos.node;
        temp->prev = pos.node->prev;
        pos.node->prev->next = temp;
        pos.node->prev = temp;
        ++_size;
        return iterator(temp);
    }

    template<class T, class Alloc>
    typename list<T, Alloc>::iterator
    list<T, Alloc>::insert(iterator pos, const value_type&& val) {
        list_node * temp = create_node(mystl::move(val));
        temp->next = pos.node;
        temp->prev = pos.node->prev;
        pos.node->prev->next = temp;
        pos.node->prev = temp;
        ++_size;
        return iterator(temp);
    }

    template<class T, class Alloc>
    void list<T, Alloc>::fill_insert(iterator pos, size_type n, const value_type& val) {
        for (;n > 0; --n) pos = insert(pos, val);
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void list<T, Alloc>::insert_dispatch(iterator pos, InputIterator first, InputIterator last, false_type) {
        for (; first != last; ++first) insert(pos, *first);
    }


    // emplace方法，全部采用完美转发来构造节点，调用insert插入节点
    template<class T, class Alloc>
    template<class... Args>
    inline void list<T, Alloc>::emplace(iterator pos, Args&&... args) {
        list_node* p = create_node(mystl::forward<Args>(args)...);
        p->prev = pos.node->prev;
        p->next = pos.node;
        pos.node->prev->next = p;
        pos.node->prev = p;
        ++_size;
    }

    template<class T, class Alloc>
    template<class... Args>
    void list<T, Alloc>::emplace_front(Args&&... args) {
        list_node* p = create_node(mystl::forward<Args>(args)...);
        iterator pos = begin();
        p->prev = pos.node->prev;
        p->next = pos.node;
        pos.node->prev->next = p;
        pos.node->prev = p;
        ++_size;
    }

    template<class T, class Alloc>
    template<class... Args>
    void list<T, Alloc>::emplace_back(Args&&... args) {
        list_node* p = create_node(mystl::forward<Args>(args)...);
        iterator pos = end();
        p->prev = pos.node->prev;
        p->next = pos.node;
        pos.node->prev->next = p;
        pos.node->prev = p;
        ++_size;
    }



    // 删除节点
    template<class T, class Alloc>
    inline typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator position) {
        list_node *next_node = position.node->next;
        list_node *prev_node = position.node->prev;
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        --_size;
        return iterator(next_node);
    }

    template<class T, class Alloc>
    typename list<T, Alloc>::iterator 
    list<T, Alloc>::erase(iterator first, iterator last) {
        iterator res;
        while (first != last) res = erase(first++);
        return res;
    }

    // 移动链表
    template<class T, class Alloc>
    inline void list<T, Alloc>::transfer(iterator pos, iterator first, iterator last) {
        if (pos != last) { 
            size_type n = mystl::distance(first, last);                  // 从原来的list断开, 加入到当前的pos位置
            list_node* tail = last.node->prev;
            list_node* head = pos.node->prev;
            tail->next = pos.node;
            pos.node->prev = tail; // 尾部连接

            first.node->prev->next = last.node; 
            last.node->prev = first.node->prev; // 断开旧链

            head->next = first.node;
            first.node->prev = head; // 头部连接
            _size += n;
        }
    }

    // 清空链表节点
    template<class T, class Alloc>
    void list<T, Alloc>::clear() {
        list_node *cur = _node->next;
        while (cur != _node) {
            list_node *temp = cur;
            cur = cur->next;
            destroy_node(temp);
        }
        _node->next = _node;
        _node->prev = _node;
        _size = 0;
    }

    // 删除链表中的指定值
    template<class T, class Alloc>
    void list<T, Alloc>::remove(const T &value) {
        iterator first = begin();
        iterator last = end();
        while (first != last) {
            iterator next = first;
            ++next;
            if (*first == value) erase(first);
            first = next;
        }
    }

    // 删除相邻重复节点
    template<class T, class Alloc>
    void list<T, Alloc>::unique() {
        iterator first = begin();
        iterator last = end();
        if (first == last) return;
        iterator next = first;
        while (++next != last) {
            if (*first == *next) {
            erase(next);
            next = first;
            } else
            first = next;
        }
    }


    template<class T, class Alloc>
    inline void list<T, Alloc>::splice(iterator position, list &, iterator i) {
        iterator j = i;
        ++j;
        // i==pos 自身无法插于自身之前
        // j==pos 已处于pos之前
        if (position == i || position == j) return;
        transfer(position, i, j);
    }

    // 按顺序融合链表（从小到大），可作为归并排序的辅助函数用来合并两个有序链表
    template<class T, class Alloc>
    void list<T, Alloc>::merge(list &x) {
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();

        while (first1 != last1 && first2 != last2) {
            if (*first2 < *first1) {
            iterator next = first2;
            transfer(first1, first2, ++next);
            first2 = next;
            } else
            ++first1;
        }
        if (first2 != last2) transfer(last1, first2, last2);
    }

    template<class T, class Alloc>
    void list<T, Alloc>::reverse() {
        // empty || size()==1
        if (_node->next == _node || _node->next->next == _node) return;
        iterator first = begin();
        ++first;// begin自身并不需要移动，它将作为指示末元素的哨兵（确切地说，最终begin.node->next == end.node)
        while (first != end()) {
            iterator old = first;
            ++first;
            transfer(begin(), old, first);
        }
    }

    // 链表排序
    template<class T, class Alloc>
    void list<T, Alloc>::sort() {
        if (_node->next == _node || _node->next->next == _node) return;
        // 数据缓存区
        // counter[n]中最多存放2^(n+1)个元素，若大于则与counter[n+1]作归并
        list carry;
        list counter[64];
        int fill = 0;
        while (!empty()) {
            carry.splice(carry.begin(), *this, begin());
            int i = 0;
            while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry);
            carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if (i == fill) ++fill;
        }
        for (int i = 1; i < fill; ++i) counter[i].merge(counter[i - 1]);
        swap(counter[fill - 1]);
    }

    template<class T>
    inline void swap(list<T> &lhs, list<T> &rhs) noexcept {
        lhs.swap(rhs);
    }


    template<class T>
    bool operator==(const list<T> &lhs, const list<T> &rhs) {
        auto it1 = lhs.begin(), it2 = rhs.begin();// same as cbegin()
        for (; it1 != lhs.end() && it2 != rhs.end(); ++it1, ++it2)
            if (*it1 != *it2) return false;
        return it1 == lhs.end() && it2 == rhs.end();
    }

    template<class T>
    inline bool operator!=(const list<T> &lhs, const list<T> &rhs) {
        return !(lhs == rhs);
    }

    template<class T>
    inline bool operator<(const list<T> &lhs, const list<T> &rhs) {
        return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                                rhs.end());
    }

    template<class T>
    inline bool operator>(const list<T> &lhs, const list<T> &rhs) {
        return rhs < lhs;
    }

    template<class T>
    inline bool operator<=(const list<T> &lhs, const list<T> &rhs) {
        return !(rhs < lhs);
    }

    template<class T>
    inline bool operator>=(const list<T> &lhs, const list<T> &rhs) {
        return !(lhs < rhs);
    }

} // namespace mystl