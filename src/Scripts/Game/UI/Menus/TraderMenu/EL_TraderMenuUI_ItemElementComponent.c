class EL_TraderMenuUI_ItemElementComponent : ScriptedWidgetComponent
{
	protected EL_TraderMenuUI m_TraderMenu;
	
	protected ButtonWidget m_QuantitySelectionButton;
	protected EL_TraderMenuUI_ItemElementQuantityComponent m_QuantityComponent;
	
	protected Widget m_wRoot;
	protected Widget m_wElementBorder;
	
	void SetTraderMenu(EL_TraderMenuUI menu)
	{
		m_TraderMenu = menu;
	}
	
	override void HandlerAttached(Widget w)
	{
		DebugPrint("::HandlerAttached - Start");
		
		m_wRoot = w;
		
		m_QuantitySelectionButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("PagesBox0"));	
		m_QuantityComponent = EL_TraderMenuUI_ItemElementQuantityComponent.Cast(m_QuantitySelectionButton.FindHandler(EL_TraderMenuUI_ItemElementQuantityComponent));
		
		m_wElementBorder = m_wRoot.FindAnyWidget("ElementBorder");
		
		DebugPrint("::HandlerAttached - End");
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{		
		return false;
	}
	
	override bool OnModalResult(Widget w, int x, int y, int code, int result);
	override bool OnDoubleClick(Widget w, int x, int y, int button);
	override bool OnSelect(Widget w, int x, int y);
	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn);
	
	override bool OnFocus(Widget w, int x, int y)
	{
		DebugPrint("::OnFocus - Start");
		DebugPrint("::OnFocus - Widget: " + w.GetName());
		
		if (w == m_wRoot)
		{
			m_wElementBorder.SetVisible(true);
		}
		
		DebugPrint("::OnFocus - End");
		
		return false;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		DebugPrint("::OnFocusLost - Start");
		DebugPrint("::OnFocusLost - Widget: " + w.GetName());		
		
		if (w == m_wRoot)
		{
			m_wElementBorder.SetVisible(false);
		}
		
		DebugPrint("::OnFocusLost - End");
		
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y);
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y);
	override bool OnMouseWheel(Widget w, int x, int y, int wheel);
	override bool OnMouseButtonDown(Widget w, int x, int y, int button);
	override bool OnMouseButtonUp(Widget w, int x, int y, int button);
	//! control is one of ControlID
	override bool OnController(Widget w, int control, int value);
	override bool OnKeyDown(Widget w, int x, int y, int key);
	override bool OnKeyUp(Widget w, int x, int y, int key);
	override bool OnKeyPress(Widget w, int x, int y, int key);
	override bool OnChange(Widget w, int x, int y, bool finished);
	override bool OnNeedScroll(Widget w, int x, int y, float prevScrollPosX, float prevScrollPosY, float newScrollPosX, float newScrollPosY);
	override bool OnResize(Widget w, int x, int y);
	override bool OnChildAdd(Widget w, Widget child);
	override bool OnChildRemove(Widget w, Widget child);
	override bool OnUpdate(Widget w);
	override bool OnEvent(EventType eventType, Widget target, int parameter0, int parameter1);
	override bool OnModalClickOut(Widget modalRoot, int x, int y, int button);
	override void HandlerDeattached(Widget w);
	
	//------------------------------------------------------------------------------------------------
	void DebugPrint(string text)
	{
	#ifdef EL_TRADER_SYSTEM_DEBUG
		Print(ToString() + text);
	#endif
	}
};