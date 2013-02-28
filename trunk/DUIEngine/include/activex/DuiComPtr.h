
#ifndef __DUICOMPTR_H__
#define __DUICOMPTR_H__

#pragma once

#include <unknwn.h>

// COMPILE_ASSERT�������ڱ���ʱ���Ա��ʽ. �������������֤��̬�����С:
//     COMPILE_ASSERT(ARRAYSIZE_UNSAFE(content_type_names)==CONTENT_NUM_TYPES,
//         content_type_names_incorrect_size);
//
// ����ȷ���ṹ��С��һ����С:
//     COMPILE_ASSERT(sizeof(foo)<128, foo_too_large);
// �ڶ���������Ǳ�����, ������ʽΪfalse, �����������һ�������������Ĵ���/����.
template<bool>
struct CompileAssert {};

// COMPILE_ASSERTʵ��ϸ��:
//
// - COMPILE_ASSERTͨ������һ������Ϊ-1������(�Ƿ�)��ʵ�ֵ�, ��ʱ���ʽfalse.
//
// - ����򻯵Ķ���
//       #define COMPILE_ASSERT(expr, msg) typedef char msg[(expr)?1:-1]
//   �ǷǷ���. ����gcc֧������ʱȷ�����ȵı䳤����(gcc��չ, ������C++��׼),
//   ����������μ򵥵Ĵ��붨�岻����:
//       int foo;
//       COMPILE_ASSERT(foo, msg); // not supposed to compile as foo is
//                                 // not a compile-time constant.
//
// - Ҫʹ������CompileAssert<(bool(expr))>, ����ȷ��expr�Ǳ���ʱ����.
//   (ģ������ڱ���ʱȷ��.)
//
// - CompileAssert<(bool(expr))>������Բ�������ڽ��gcc 3.4.4��4.0.1��
//   һ��bug. ���д��
//       CompileAssert<bool(expr)>
//   ���������޷�����
//       COMPILE_ASSERT(5>0, some_message);
//   ("5>0"�е�">"������Ϊ��ģ������б��β��">".)
//
// - �����С��(bool(expr)?1:-1)������((expr)?1:-1), ���Խ��MS VC 7.1
//   �а�((0.0)?1:-1)�������Ϊ1��bug.
#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
	typedef CompileAssert<(bool(expr))> msg[bool(expr)?1:-1]

namespace DuiEngine
{
	template<class T>
	class CDuiRefPtr
	{
	public:
		CDuiRefPtr() : ptr_(NULL) {}

		CDuiRefPtr(T* p) : ptr_(p)
		{
			if(ptr_)
			{
				ptr_->AddRef();
			}
		}

		CDuiRefPtr(const CDuiRefPtr<T>& r) : ptr_(r.ptr_)
		{
			if(ptr_)
			{
				ptr_->AddRef();
			}
		}

		template<typename U>
		CDuiRefPtr(const CDuiRefPtr<U>& r) : ptr_(r.get())
		{
			if(ptr_)
			{
				ptr_->AddRef();
			}
		}

		~CDuiRefPtr()
		{
			if(ptr_)
			{
				ptr_->Release();
			}
		}

		T* get() const { return ptr_; }
		operator T*() const { return ptr_; }
		T* operator->() const { return ptr_; }

		// �ͷ�ָ��.
		// ���ض���ǰӵ�е�ָ��. ���ָ��ΪNULL, ����NULL.
		// ������ɺ�, ����ӵ��һ��NULLָ��, ����ӵ���κζ���.
		T* release()
		{
			T* retVal = ptr_;
			ptr_ = NULL;
			return retVal;
		}

		CDuiRefPtr<T>& operator=(T* p)
		{
			// �ȵ���AddRef, �������Ҹ�ֵҲ�ܹ���.
			if(p)
			{
				p->AddRef();
			}
			if(ptr_ )
			{
				ptr_ ->Release();
			}
			ptr_ = p;
			return *this;
		}

		CDuiRefPtr<T>& operator=(const CDuiRefPtr<T>& r)
		{
			return *this = r.ptr_;
		}

		template<typename U>
		CDuiRefPtr<T>& operator=(const CDuiRefPtr<U>& r)
		{
			return *this = r.get();
		}

		void swap(T** pp)
		{
			T* p = ptr_;
			ptr_ = *pp;
			*pp = p;
		}

		void swap(CDuiRefPtr<T>& r)
		{
			swap(&r.ptr_);
		}

	protected:
		T* ptr_;
	};

	template<typename T>
	CDuiRefPtr<T> MakeRefPtr(T* t)
	{
		return CDuiRefPtr<T>(t);
	}

	// ģ��COM�ӿڵ�����ָ��. ʹ��scoped_refptr��Ϊ�����ṩ���ü�������,
	// �����IUnknown��һЩ�ض�����.
	template<class Interface, const IID* interface_id=&__uuidof(Interface)>
	class CDuiComPtr : public CDuiRefPtr<Interface>
	{
	public:
		// ����ģ��, ��ֹʹ����͸��ScopedComPtr����ýӿ�ָ���AddRef��(��)
		// Release.
		class BlockIUnknownMethods : public Interface
		{
		private:
			STDMETHOD(QueryInterface)(REFIID iid, void** object) = 0;
			STDMETHOD_(ULONG, AddRef)() = 0;
			STDMETHOD_(ULONG, Release)() = 0;
		};

		typedef CDuiRefPtr<Interface> ParentClass;

		CDuiComPtr() {}

		explicit CDuiComPtr(Interface* p) : ParentClass(p) {}

		CDuiComPtr(const CDuiComPtr<Interface, interface_id>& p)
			: ParentClass(p) {}

		~CDuiComPtr()
		{
			// ����ָ����Ĵ�С����ָ�뱣��һ��.
			COMPILE_ASSERT(sizeof(CDuiComPtr<Interface, interface_id>) ==
				sizeof(Interface*), ScopedComPtrSize);
		}

		// ��ʽ�����ڲ������Release()����. ΪScopedComPtrʵ���ṩ���ù���.
		// ������IUnknown::Release�ȼ�, ��Ҫ��scoped_ptr::release()����.
		void Release()
		{
			if(ptr_ != NULL)
			{
				ptr_->Release();
				ptr_ = NULL;
			}
		}

		// ʹ�ڲ������뱾������벢������ָ��.
		Interface* Detach()
		{
			Interface* p = ptr_;
			ptr_ = NULL;
			return p;
		}

		// ����һ���Ѿ���addref�Ľӿ�ָ��.
		void Attach(Interface* p)
		{
			DUIASSERT(ptr_ == NULL);
			ptr_ = p;
		}

		// ���ؽӿ�ָ��ĵ�ַ.
		// ���ڽ����������(�ӹ�����Ȩ). ����ʹ��DUIASSERTs��֤��ǰֵ�Ƿ�ΪNULL.
		// �÷�: Foo(p.Receive());
		Interface** Receive()
		{
			DUIASSERT(ptr_==NULL);
			return &ptr_;
		}

		// ����void**���͵Ľӿ�ָ��ĵ�ַ.
		void** ReceiveVoid()
		{
			return reinterpret_cast<void**>(Receive());
		}

		template<class Query>
		HRESULT QueryInterface(Query** p)
		{
			DUIASSERT(p != NULL);
			DUIASSERT(ptr_ != NULL);
			// IUnknown�Ѿ���һ��ģ��汾��QueryInterface, ����iid������
			// ��������ʽ��. ����ֻ�������DUIASSERTs.
			return ptr_->QueryInterface(p);
		}

		// ��IIDû�����͹���ʱQI.
		HRESULT QueryInterface(const IID& iid, void** obj)
		{
			DUIASSERT(obj != NULL);
			DUIASSERT(ptr_ != NULL);
			return ptr_->QueryInterface(iid, obj);
		}

		// ��|other|��ѯ�ڲ�����ӿ�, ������other->QueryInterface�����Ĵ�����.
		HRESULT QueryFrom(IUnknown* object)
		{
			DUIASSERT(object != NULL);
			return object->QueryInterface(Receive());
		}

		// CoCreateInstance��װ.
		HRESULT CreateInstance(const CLSID& clsid, IUnknown* outer=NULL,
			DWORD context=CLSCTX_ALL)
		{
			DUIASSERT(ptr_ == NULL);
			HRESULT hr = ::CoCreateInstance(clsid, outer, context,
				*interface_id, reinterpret_cast<void**>(&ptr_));
			return hr;
		}

		// ��鱾������|other|�Ƿ���ͬ.
		bool IsSameObject(IUnknown* other)
		{
			if(!other && !ptr_)
			{
				return true;
			}

			if(!other || !ptr_)
			{
				return false;
			}

			CDuiComPtr<IUnknown> my_identity;
			QueryInterface(my_identity.Receive());

			CDuiComPtr<IUnknown> other_identity;
			other->QueryInterface(other_identity.Receive());

			return static_cast<IUnknown*>(my_identity) ==
				static_cast<IUnknown*>(other_identity);
		}

		// �ṩ�ӿڵ�ֱ�ӷ���. ����ʹ����һ�����õļ������Ʒ���IUknown��
		// ����, ���ⷢ����������Ĵ�������:
		//     ScopedComPtr<IUnknown> p(Foo());
		//     p->Release();
		//     ... ������������ִ��ʱ, ���ٴε���Release().
		// ���ܵ�����QueryInterface��ӵ�DUIASSERTs. �����㾲̬��ǿתScopedComPtr��
		// ����װ�Ľӿ�, �㻹�ǿ���ͨ���ӿ�ָ�������Щ����, ����һ�㲻Ҫ������.
		BlockIUnknownMethods* operator->() const
		{
			DUIASSERT(ptr_ != NULL);
			return reinterpret_cast<BlockIUnknownMethods*>(ptr_);
		}

		// ʹ�ø����operator=().
		using CDuiRefPtr<Interface>::operator=;

		static const IID& iid()
		{
			return *interface_id;
		}
	};

} //namespace base

#endif //__DUICOMPTR_H__