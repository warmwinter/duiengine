function onEvtTest2(sender,nmhdr)
	DuiMessageBox(DuiThreadActiveWndManager_GetActive(),L "onEvtTest2", L "msgbox", 1);
	return 1;
end

function onEvtTstClick(sender,nmhdr)
	--��֧����һ��������������������A():B():C()
	local luabtn=sender:GetParent():LoadXmlChildren("<button pos=\"0,0,100,30\" class=\"normalbtn\" id=\"12321\">lua btn</button>");
	local duiSys=DuiSystem_getSingleton();
	local scriptmod=duiSys:GetScriptModule();
	scriptmod:subscribeEvent(luabtn,1,"onEvtTest2");
	sender:SetVisible(0,1);
	return 1;
end
