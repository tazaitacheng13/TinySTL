#pragma once

#include "type_traits.h"
#include "algobase.h"
#include <new>// placement new


namespace mystl {
    template <class T>
    void construct(T* ptr)
    {
        new ((void*)ptr) T();
    }

    template<class T1, class T2>
    inline void construct(T1* p, const T2& value) {
        new (p) T1(value);
    }


    template<class T1, class... Args>
    inline void construct(T1* ptr, Args&&... args) {
        new (ptr) T1(mystl::forward<Args>(args)...);
    }

    template<class T>
    inline void destroy(T* p) {
        p->~T();
    }

    // �跨����traits������������
    template<class ForwardIterator>
    inline void destroy(ForwardIterator beg, ForwardIterator end) {
        using is_POD_type = typename type_traits<ForwardIterator>::is_POD_type;
        _destroy_aux(beg, end, is_POD_type());
    }

    // ���Ԫ�ص�value_type������non��trivial destructor
    template<class ForwardIterator>
    inline void _destroy_aux(ForwardIterator beg, ForwardIterator end,
        false_type) {
        for (; beg != end; ++beg) destroy(&*beg); // ȡ���ݣ�ȡ��ַ
    }

    // ����trivial destructor
    // �����������������޹�ʹ������ô������������һ��Ч���ϵľ޴��˷�
    template<class ForwardIterator>
    inline void _destroy_aux(ForwardIterator, ForwardIterator, true_type) {}

    // ���char*��wchar_t*���ػ�
    inline void destroy(char*, char*) {}
    inline void destroy(wchar_t*, wchar_t*) {}

}// namespace mystl