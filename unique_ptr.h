#pragma once
#include <functional>
#include <atomic>
#include "algobase.h"
#include "type_traits.h"

namespace mystl
{

class Deleter {
public:
	template<class T>
	void operator()(T* ptr) const { delete ptr; }
};

template<class T, class D = Deleter>
class unique_ptr {
public:
	unique_ptr(T* p = nullptr, const D &d = D()) : ptr(p), deleter(d) {};
	~unique_ptr() { deleter(ptr); }

public: // ���ÿ���
	unique_ptr(const unique_ptr& other) = delete;
	unique_ptr& operator=(const unique_ptr& other) = delete;

public: // �����ƶ�
	unique_ptr(unique_ptr&& other) noexcept : ptr(other.ptr), deleter(mystl::move(other.deleter)) { other.ptr = nullptr; };
	unique_ptr& operator=(unique_ptr&& rhs) noexcept;

public: // ����=nullptr
	unique_ptr& operator=(std::nullptr_t) noexcept;

public:
	operator bool() const { return ptr; }

public: // ���ؽ����úͳ�Ա���������
	T& operator*() const { return *ptr; }
	T* operator->() const { return &(operator*()); }
public:
	T* get() const noexcept { return ptr; }
	D& get_deleter() noexcept { return deleter; }
	const D& get_deleter() const noexcept { return deleter; }
	void reset(T* p = nullptr) noexcept;
	T* release() noexcept;

public:// swap for operator=
	void swap(unique_ptr& other) noexcept;

private:
	T* ptr;
	D deleter;
};

template<class T, class D>
inline void unique_ptr<T, D>::swap(unique_ptr<T, D>& other) noexcept {
	std::swap(ptr, other.ptr);
	std::swap(deleter, other.deleter);
}

// �ƶ���ֵ
template<class T, class D>
inline unique_ptr<T, D>& unique_ptr<T, D>::operator=(unique_ptr<T, D>&& other) noexcept {
	if (ptr != other.ptr) {
		reset(other.ptr);
		deleter = mystl::move(other.deleter);
		other.ptr = nullptr;
	}
	deleter = mystl::move(other.deleter);
	return *this;
}

// ��ֵΪnullptr
template<class T, class D>
inline unique_ptr<T, D>& unique_ptr<T, D>::operator=(std::nullptr_t) noexcept {
	reset();
	return *this;
}

// ����������ͷ�ԭ�ڴ棬ָ�����ڴ棩
template<class T, class D>
inline void unique_ptr<T, D>::reset(T* p) noexcept {
	if (ptr != p) {
		deleter(ptr);
		ptr = p;
	}
}

// ָ���µ�ַ��������ԭ��ַ
template<class T, class D>
inline T* unique_ptr<T, D>::release() noexcept {
	T* res = ptr;
	ptr = nullptr;
	return res;
}

} // namespace mystl



