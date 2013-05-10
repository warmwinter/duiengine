#pragma once
#include "duisingletonmap.h"
#include "duiwnd.h"

namespace DuiEngine
{

class CDuiWindowFactory
{
public:
    virtual ~CDuiWindowFactory() {}
    virtual CDuiWindow* NewWindow() = 0;

    virtual void DeleteWindow(CDuiWindow* pWnd) = 0;

    virtual const CDuiStringA & getWindowType()=0;

	virtual CDuiWindowFactory* Clone() const =0;
};

template <typename T>
class TplDuiWindowFactory : public CDuiWindowFactory
{
public:
    //! Default constructor.
	TplDuiWindowFactory():m_strTypeName(T::GetClassName())
    {
    }

    // Implement WindowFactory interface
    CDuiWindow* NewWindow()
    {
        return new T;
    }

    void DeleteWindow(CDuiWindow* pWnd)
    {
        delete static_cast<T*>(pWnd);
    }

    virtual const CDuiStringA & getWindowType()
    {
        return m_strTypeName;
    }

	virtual CDuiWindowFactory* Clone() const 
	{
		return new TplDuiWindowFactory();
	}
protected:
    CDuiStringA m_strTypeName;
};



typedef CDuiWindowFactory* CDuiWindowFactoryPtr;
class DUI_EXP DuiWindowFactoryManager :
    public DuiSingletonMap<DuiWindowFactoryManager,CDuiWindowFactoryPtr,CDuiStringA>
{
public:
    DuiWindowFactoryManager(void);

    //************************************
    // Method:    RegisterFactory,ע��APP�Զ���Ĵ�����
    // FullName:  DuiEngine::DuiWindowFactoryManager::RegisterFactory
    // Access:    public
    // Returns:   bool
    // Qualifier:
    // Parameter: CDuiWindowFactory * pWndFactory:���ڹ���ָ��
	// Parameter: bool bReplace:ǿ���滻ԭ�й�����־
    //************************************
    bool RegisterFactory(CDuiWindowFactory & wndFactory,bool bReplace=false)
    {
		if(HasKey(wndFactory.getWindowType()))
		{
			if(!bReplace) return false;
			RemoveKeyObject(wndFactory.getWindowType());
		}
        AddKeyObject(wndFactory.getWindowType(),wndFactory.Clone());
        return true;
    }

    //************************************
    // Method:    UnregisterFactor,��ע��APP�Զ���Ĵ�����
    // FullName:  DuiEngine::DuiWindowFactoryManager::UnregisterFactor
    // Access:    public
    // Returns:   bool
    // Qualifier:
    // Parameter: CDuiWindowFactory * pWndFactory
    //************************************
    bool UnregisterFactory(const CDuiStringA & strClassType)
    {
		return  RemoveKeyObject(strClassType);
    }

	CDuiWindow *CreateWindowByName(LPCSTR pszClassName);
protected:
    static void OnWndFactoryRemoved(const CDuiWindowFactoryPtr & obj);

    void AddStandardWindowFactory();
};

}//namespace DuiEngine