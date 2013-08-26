#include "duistd.h"
#include "DuiEventSet.h"

namespace DuiEngine
{
	//////////////////////////////////////////////////////////////////////////
	// CDuiEvent
	bool CDuiEvent::subscribe( const SlotFunctorBase& slot )
	{
		if(findSlotFunctor(slot) != -1) return false;
		m_evtSlots.Add(slot.Clone());
		return true;
	}

	bool CDuiEvent::unsubscribe( const SlotFunctorBase& slot )
	{
		int idx=findSlotFunctor(slot);
		if(idx==-1) return false;

		delete m_evtSlots[idx];
		m_evtSlots.RemoveAt(idx);
		return true;
	}

	int CDuiEvent::findSlotFunctor( const SlotFunctorBase& slot )
	{
		for(UINT i=0;i<m_evtSlots.GetCount();i++)
		{
			if(m_evtSlots[i]->Equal(slot))
			{
				return i;
			}
		}
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////
	// CDuiEventSet
	CDuiEventSet::CDuiEventSet(void):m_bMuted(FALSE)
	{
	}

	CDuiEventSet::~CDuiEventSet(void)
	{
		removeAllEvents();
	}

	CDuiEvent * CDuiEventSet::GetEventObject(const DWORD dwEventID )
	{
		for(UINT i=0;i<m_evtArr.GetCount();i++)
		{
			if(m_evtArr[i]->GetEventID()==dwEventID) return m_evtArr[i];
		}
		return NULL;
	}

	void CDuiEventSet::FireEvent( const DWORD dwEventID, EventArgs& args )
	{
		// find event object
		CDuiEvent* ev = GetEventObject(dwEventID);

		// fire the event if present and set is not muted
		if ((ev != 0) && !m_bMuted)
		{
			(*ev)(args);
		}
	}

	void CDuiEventSet::addEvent( const DWORD dwEventID )
	{
		if(!isEventPresent(dwEventID))
		{
			m_evtArr.Add(new CDuiEvent(dwEventID));
		}
	}

	void CDuiEventSet::removeEvent( const DWORD dwEventID )
	{
		for(UINT i=0;i<m_evtArr.GetCount();i++)
		{
			if(m_evtArr[i]->GetEventID()==dwEventID)
			{
				delete m_evtArr[i];
				m_evtArr.RemoveAt(i);
				return;
			}
		}
	}

	bool CDuiEventSet::isEventPresent( const DWORD dwEventID )
	{
		return GetEventObject(dwEventID)!=NULL;
	}

	void CDuiEventSet::removeAllEvents( void )
	{
		for(UINT i=0;i<m_evtArr.GetCount();i++)
		{
			delete m_evtArr[i];
		}
		m_evtArr.RemoveAll();
	}

	bool CDuiEventSet::subscribeEvent( const DWORD dwEventID, const SlotFunctorBase & subscriber )
	{
		if(!isEventPresent(dwEventID)) return false;
		return GetEventObject(dwEventID)->subscribe(subscriber);
	}

	bool CDuiEventSet::unsubscribeEvent( const DWORD dwEventID, const SlotFunctorBase & subscriber )
	{
		if(!isEventPresent(dwEventID)) return false;
		return GetEventObject(dwEventID)->unsubscribe(subscriber);
	}


}//end of namespace