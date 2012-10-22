#pragma once

template<typename T, int t_nFixedBytes = 128>
class CMyBuffer
{
public:
	CMyBuffer() : m_p(NULL),m_nSize(0),m_bExternalBuf(FALSE)
	{
	}

	CMyBuffer(size_t nElements) : m_p(NULL)
	{
		Allocate(nElements);
	}

	~CMyBuffer()
	{
		Free();
	}

	operator T*() const
	{
		return m_p;
	}

	T* operator ->() const
	{
		ATLASSERT(m_p != NULL);
		return m_p;
	}

	const T & operator[] (int i) const
	{
		ATLASSERT(m_p != NULL);
		return m_p[i];
	}

	T & operator[] (int i)
	{
		ATLASSERT(m_p != NULL);
		return m_p[i];
	}

	void Attach(T *pBuf,size_t size)
	{
		Free();
		m_p=pBuf;
		m_nSize=size;
		m_bExternalBuf=TRUE;
	}

	T* Allocate(size_t nElements)
	{
		ATLASSERT(nElements <= (SIZE_MAX / sizeof(T))-1);
		return AllocateBytes(nElements * sizeof(T));
	}

	T* AllocateBytes(size_t nBytes)
	{
		ATLASSERT(m_p == NULL);
		if(nBytes >= t_nFixedBytes)
			m_p = static_cast<T*>(malloc(nBytes+1));
		else
			m_p = reinterpret_cast<T*>(m_abFixedBuffer);
		m_nSize=nBytes/sizeof(T);
		memset(m_p,0,sizeof(T));//�Զ��ڵ�һ��Ԫ����0
		memset(m_p+m_nSize,0,sizeof(T));//�Զ����������һ��0��λ��
		m_bExternalBuf=FALSE;
		return m_p;
	}

	size_t size(){return m_nSize;}

	void Free()
	{
		if(!m_bExternalBuf && m_p != reinterpret_cast<T*>(m_abFixedBuffer))
			free(m_p);
		m_p=NULL;
		m_nSize=0;
		m_bExternalBuf=FALSE;
	}
private:
	T* m_p;
	unsigned char m_abFixedBuffer[t_nFixedBytes];
	size_t m_nSize;
	BOOL	m_bExternalBuf;
};
