
#ifndef __DUIBSTR_H__
#define __DUIBSTR_H__

#pragma once

#include <windows.h>
#include <oleauto.h>

typedef WCHAR char16;
typedef unsigned int uint32;

// DISALLOW_COPY_AND_ASSIGN���ÿ����͸�ֵ���캯��.
// ��Ҫ�����private:���ʿ�������ʹ��.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&); \
	void operator=(const TypeName&)

namespace DuiEngine
{

        // Manages a BSTR string pointer.
        // The class interface is based on scoped_ptr.
        class CDuiBStr
        {
        public:
            CDuiBStr() : bstr_(NULL) {}

            // Constructor to create a new BSTR.
            //
            // NOTE: Do not pass a BSTR to this constructor expecting ownership to
            // be transferred - even though it compiles! ;-)
            explicit CDuiBStr(const char16* non_bstr);
            ~CDuiBStr();

            // Give ScopedBstr ownership over an already allocated BSTR or NULL.
            // If you need to allocate a new BSTR instance, use |allocate| instead.
            void Reset(BSTR bstr=NULL);

            // Releases ownership of the BSTR to the caller.
            BSTR Release();

            // Creates a new BSTR from a 16-bit C-style string.
            //
            // If you already have a BSTR and want to transfer ownership to the
            // ScopedBstr instance, call |reset| instead.
            //
            // Returns a pointer to the new BSTR, or NULL if allocation failed.
            BSTR Allocate(const char16* str);

            // Allocates a new BSTR with the specified number of bytes.
            // Returns a pointer to the new BSTR, or NULL if allocation failed.
            BSTR AllocateBytes(size_t bytes);

            // Sets the allocated length field of the already-allocated BSTR to be
            // |bytes|.  This is useful when the BSTR was preallocated with e.g.
            // SysAllocStringLen or SysAllocStringByteLen (call |AllocateBytes|) and then
            // not all the bytes are being used.
            //
            // Note that if you want to set the length to a specific number of
            // characters, you need to multiply by sizeof(wchar_t).  Oddly, there's no
            // public API to set the length, so we do this ourselves by hand.
            //
            // NOTE: The actual allocated size of the BSTR MUST be >= bytes.  That
            // responsibility is with the caller.
            void SetByteLen(size_t bytes);

            // Swap values of two ScopedBstr's.
            void Swap(CDuiBStr& bstr2);

            // Retrieves the pointer address.
            // Used to receive BSTRs as out arguments (and take ownership).
            // The function DCHECKs on the current value being NULL.
            // Usage: GetBstr(bstr.Receive());
            BSTR* Receive();

            // Returns number of chars in the BSTR.
            size_t Length() const;

            // Returns the number of bytes allocated for the BSTR.
            size_t ByteLength() const;

            operator BSTR() const
            {
                return bstr_;
            }

        protected:
            BSTR bstr_;

        private:
            // Forbid comparison of ScopedBstr types.  You should never have the same
            // BSTR owned by two different scoped_ptrs.
            bool operator==(const CDuiBStr& bstr2) const;
            bool operator!=(const CDuiBStr& bstr2) const;
            DISALLOW_COPY_AND_ASSIGN(CDuiBStr);
        };

} //namespace base

#endif //__DUIBSTR_H__