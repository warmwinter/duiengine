#pragma once

#include "wtl.mini/duistr.h"

namespace DuiEngine
{

	class DUI_EXP CAccelerator
	{
	public:
		CAccelerator(void);
		~CAccelerator(void);

		CDuiStringT GetKeyName(WORD vk);

		CDuiStringT FormatHotkey();

		WORD GetModifier() const {return m_wModifier;}
		WORD GetKey() const {return m_wVK;}
	protected:
		WORD 	m_wModifier;
		WORD	m_wVK;
	};

	// ��Ҫע����̼��ټ�������Ҫʵ�ֱ��ӿ�.
	class DUI_EXP CAcceleratorTarget
	{
	public:
		// ������ټ�������, ����Ӧ�÷���true.
		virtual bool OnAcceleratorPressed(const CAccelerator& accelerator) = 0;

	protected:
		virtual ~CAcceleratorTarget() {}
	};

}//end of namespace DuiEngine
