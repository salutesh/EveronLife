class EL_TraderMenuUI_ItemElementQuantityComponent : ScriptedWidgetComponent
{
	protected Widget m_wRoot;

	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
	}
};