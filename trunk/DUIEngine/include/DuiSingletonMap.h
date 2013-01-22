#pragma once
#include "DUISingleton.h"
namespace DuiEngine
{

template<class TClass,class TObj,class TKey=CDuiStringA>
class DUI_EXP DuiSingletonMap :public Singleton<TClass>
{
public:
    DuiSingletonMap(void (*funOnKeyRemoved)(const TObj &)=NULL):m_pFunOnKeyRemoved(funOnKeyRemoved)
    {
        m_mapNamedObj=new CDuiMap<TKey,TObj>;
    }
    virtual ~DuiSingletonMap()
    {
        RemoveAll();
        delete m_mapNamedObj;
    }

    bool HasKey(const TKey & key)
    {
        return m_mapNamedObj->Lookup(key)!=NULL;
    }
    bool GetKeyObject(const TKey & key,TObj & obj)
    {
        if(!HasKey(key)) return false;
        obj=(*m_mapNamedObj)[key];
        return true;
    }
    TObj & GetKeyObject(const TKey & key)
    {
        return (*m_mapNamedObj)[key];
    }
    bool AddKeyObject(const TKey & key,const TObj & obj)
    {
        if(HasKey(key)) return false;
        (*m_mapNamedObj)[key]=obj;
        return true;
    }
    void SetKeyObject(const TKey & key,const TObj & obj)
    {
        (*m_mapNamedObj)[key]=obj;
    }
    bool RemoveKeyObject(const TKey & key)
    {
        if(!HasKey(key)) return false;
        if(m_pFunOnKeyRemoved)
        {
            m_pFunOnKeyRemoved((*m_mapNamedObj)[key]);
        }
        m_mapNamedObj->RemoveKey(key);
        return true;
    }
    void RemoveAll()
    {
        if(m_pFunOnKeyRemoved)
        {
            POSITION pos=m_mapNamedObj->GetStartPosition();
            while(pos)
            {
                CDuiMap<TKey,TObj>::CPair *p=m_mapNamedObj->GetNext(pos);
                m_pFunOnKeyRemoved(p->m_value);
            }
        }
        m_mapNamedObj->RemoveAll();
    }
    size_t GetCount()
    {
        return m_mapNamedObj->GetCount();
    }
protected:
    void (*m_pFunOnKeyRemoved)(const TObj &obj);

    CDuiMap<TKey,TObj> *m_mapNamedObj;
};



}//namespace DuiEngine