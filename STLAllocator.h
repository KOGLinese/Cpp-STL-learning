#pragma once
#include <new>      // for placement new
#include <cstddef>  // for ptrdiff_t, size_t
#include <cstdlib>  // for exit()
#include <climits>  // for UINT_MAX
#include <iostream> // for cerr

// 只能有限度 搭配 PJ STL 和 RW STL
namespace JJ
{
	template <class T>
	inline T* _allocate(ptrdiff_t size, T*)
	{
		std::set_new_handler(0); //为了卸载目前的内存分配异常处理函数，这样一来一旦分配内存失败的话，C++就会强制性抛出std:bad_alloc异常，而不是跑到处理某个异常处理函数去处理
		T* tmp = static_cast<T*>(::operator new(static_cast<size_t>(size * sizeof(T))));
		if (tmp == 0)
		{
			std::cerr << "out of memory" << std::endl;
			exit(1);
		}
		return tmp;
	}

	template <class T>
	inline void _deallocate(T* buffer)
	{
		::operator delete(buffer);
	}

	template <class T1, class T2>
	inline void _construct(T1* p, const T2& value)
	{
		new (p) T1(value); // 指定的地址构造一个对象，调用他的构造函数
	}

	template <class T>
	inline void _destroy(T* ptr)
	{
		ptr->~T();
	}

	template <class T>
	class allocator
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		// rebind allocator of type U
		// 将 类型T 更换为 U
		template <class U>
		struct rebind
		{
			typedef allocator<U> other;
		};

		pointer allocate(size_type n, const void* hint = 0)
		{
			return _allocate(static_cast<difference_type>(n), static_cast<pointer>(0));
		}

		void deallocate(pointer p, size_type n) { _deallocate(p); }

		void construct(pointer p, const T& value)
		{
			_construct(p, value);
		}

		void destroy(pointer p) { _destroy(p); }

		pointer address(reference x) { return static_cast<pointer>(&x); }

		const_pointer const_address(const_reference x)
		{
			return static_cast<const_pointer>(&x);
		}

		size_type max_size() const // 返回可成功配置的最大量
		{
			return size_type(UINT_MAX / sizeof(T));
		}
	};

}