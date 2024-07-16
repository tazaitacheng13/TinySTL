#pragma once
#include <cstddef>
#include <cstring>
#include "type_traits.h"
#include "iterator.h"

namespace mystl {


    template<class T>
    inline const T& max(const T& a, const T& b) {
        return a < b ? b : a;
    }

    template<class T, class Compare>
    inline const T& max(const T& a, const T& b, Compare comp) {
        return comp(a, b) ? b : a;
    }

    template<class T>
    inline const T& min(const T& a, const T& b) {
        return b < a ? b : a;
    }

    template<class T, class Compare>
    inline const T& min(const T& a, const T& b, Compare comp) {
        return comp(b, a) ? b : a;
    }

    template<class InputIterator1, class InputIterator2, class Compare>
    bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
        InputIterator2 first2, InputIterator2 last2,
        Compare comp) {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if (comp(*first1, *first2))
                return true;
            else if (comp(*first2, *first1))
                return false;
        }
        return first1 == last1 && first2 != last2;// ���ڶ��������࣬����true������false
    }

    // ���ԭʼָ��const unsigned char*��ȫ�ػ��汾
    inline bool lexicographical_compare(const unsigned char* first1,
        const unsigned char* last1,
        const unsigned char* first2,
        const unsigned char* last2) {
        const size_t len1 = last1 - first1;
        const size_t len2 = last2 - first2;
        // �ȱȽϳ�����ͬ�Ķ���
        const int result = memcmp(first1, first2, min(len1, len2));
        return result != 0 ? result < 0 : len1 < len2;
    }



    template<class InputIterator, class OutputIterator>
    struct _copy_dispatch {// �º�������
        OutputIterator operator()(InputIterator first, InputIterator last,
            OutputIterator result) {
            return _copy(first, last, result,
                iterator_category_t<InputIterator>());
        }
    };

    // ƫ�ػ�����
    template<class T>
    struct _copy_dispatch<T*, T*> {
        T* operator()(T* first, T* last, T* result) {
            using t = typename type_traits<T>::has_trivial_assignment_operator;
            return _copy_t(first, last, result, t());
        }
    };

    // ƫ�ػ�����
    template<class T>
    struct _copy_dispatch<const T*, T*> {
        T* operator()(const T* first, const T* last, T* result) {
            using t = typename type_traits<T>::has_trivial_assignment_operator;
            return _copy_t(first, last, result, t());
        }
    };

    template<class InputIterator, class OutputIterator>
    inline OutputIterator copy(InputIterator first, InputIterator last,
        OutputIterator result) {
        return _copy_dispatch<InputIterator, OutputIterator>()(
            first, last, result);// _copy_dispatch��һ���º�������
    }

    // ���ָ���ƫ�ػ�
    inline char* copy(const char* first, const char* last, char* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t* copy(const wchar_t* first, const wchar_t* last,
        wchar_t* result) {
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }

    // InputIterator
    template<class InputIterator, class OutputIterator>
    inline OutputIterator _copy(InputIterator first, InputIterator last,
        OutputIterator result, input_iterator_tag) {
        for (; first != last; ++first, ++result)// ��������ͬ���ٶȽ���
            *result = *first;
        return result;
    }

    template<class InputIterator, class OutputIterator, class Distance>
    inline OutputIterator _copy_d(InputIterator first, InputIterator last,
        OutputIterator result, Distance) {
        for (Distance n = last - first; n > 0;
            --n, ++first, ++result)// ��n����ѭ���������ٶȽϿ�
            *result = *first;
        return result;
    }

    // RandomIterator
    template<class InputIterator, class OutputIterator>
    inline OutputIterator _copy(InputIterator first, InputIterator last,
        OutputIterator result,
        random_access_iterator_tag) {
        return _copy_d(first, last, result,
            difference_type_t<InputIterator>());// ��ϸ�ֺ����Ա㸴��
    }

    // �߱�trivial copy assignment operator����ִ��memmove
    template<class T>
    inline T* _copy_t(const T* first, const T* last, T* result, true_type) {
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }


    // ԭʼָ����һ��random_access_iterator
    template<class T>
    inline T* _copy_t(const T* first, const T* last, T* result, false_type) {
        return _copy_d(first, last, result, ptrdiff_t());
    }

    template<class BidirectionalIter1, class BidirectionalIter2, class Distance>
    inline BidirectionalIter2 _copy_backward(BidirectionalIter1 first,
        BidirectionalIter1 last,
        BidirectionalIter2 result,
        bidirectional_iterator_tag,
        Distance) {
        while (first != last) *--result = *--last;
        return result;
    }

    template<class RandomAccessIter, class BidirectionalIter2, class Distance>
    inline BidirectionalIter2 _copy_backward(RandomAccessIter first,
        RandomAccessIter last,
        BidirectionalIter2 result,
        random_access_iterator_tag,
        Distance) {
        for (Distance n = last - first; n > 0; --n) *--result = *--last;
        return result;
    }

    template<class BidirectionalIter1, class BidirectionalIter2, class BoolType>
    struct _copy_backward_dispatch {
        BidirectionalIter2 operator()(BidirectionalIter1 first,
            BidirectionalIter1 last,
            BidirectionalIter2 result) {
            return _copy_backward(first, last, result,
                iterator_category_t<BidirectionalIter1>(),
                difference_type_t<BidirectionalIter1>());
        }
    };

    template<class T>
    struct _copy_backward_dispatch<T*, T*, true_type> {
        T* operator()(const T* first, const T* last, T* result) {
            const ptrdiff_t n = last - first;
            memmove(result - n, first, sizeof(T) * n);
            return result - n;
        }
    };

    template<class T>
    struct _copy_backward_dispatch<const T*, T*, true_type> {
        T* operator()(const T* first, const T* last, T* result) {
            return _copy_backward_dispatch<T*, T*, true_type>()(first, last,
                result);
        }
    };

    template<class BI1, class BI2>
    inline BI2 copy_backward(BI1 first, BI1 last, BI2 result) {
        using Trivial = typename type_traits<
            value_type_t<BI2>>::has_trivial_assignment_operator;
        return _copy_backward_dispatch<BI1, BI2, Trivial>()(first, last, result);
    }


    // ��д��Χ�ڵ�ֵ
    template<class ForwardIterator, class T>
    void fill(ForwardIterator first, ForwardIterator last, const T& value) {
        for (; first != last; ++first) *first = value;
    }

    // ��д����������insert���ʹ��
    template<class OutputIterator, class Size, class T>
    OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
        for (; n > 0; --n, ++first) *first = value;
        return first;
    }

    // ����
    template<class T>
    inline void swap(T& a, T& b) {
        T temp = a;
        a = b;
        b = temp;
    }

    // ����Ҫ֪��������ָ��Ķ������ͣ����ܹ����������˱���ʹ����value_type
    template<class ForwardIterator1, class ForwardIterator2, class T>
    inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b, T) {
        T temp = *a;
        *a = *b;
        *b = temp;
    }

    template<class ForwardIterator1, class ForwardIterator2>
    inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
        return iter_swap(a, b, value_type_t<ForwardIterator1>());
    }

}