enum EL_TraderMenuUITab
{
	TAB_BUY = 0,
	TAB_SELL,
}

class EL_TraderMenuUI: ChimeraMenuBase
{
	protected Widget m_wLayoutRoot;
	
	//! Main layout elements
	protected TextWidget m_wTraderName;
	protected OverlayWidget m_wContentWidget;
	protected VerticalLayoutWidget m_wTabContent;
		
	protected SCR_TabViewComponent m_TabViewComponent;
	
	protected ResourceName m_TraderConfigResource;
	protected ref EL_TraderInfoList m_TraderConfigData;
	protected ref array<ref EL_TraderItemInfo> m_aTraderItemBuyList;
	protected ref array<ref EL_TraderItemInfo> m_aTraderItemSellList;
	
	protected ref array<ref EL_TraderMenuUI_ItemElement> m_aTraderItemUIElements;
	protected static const int S_TILES_GRID_WIDTH = 3;
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		super.OnMenuUpdate(tDelta);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuInit()
	{
		Print(ToString() + "::OnMenuInit - Start");
		
		super.OnMenuInit();
				
		Print(ToString() + "::OnMenuInit - End");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{	
		Init();
		
		if (!m_TraderConfigData)
			return;
		
		m_wTraderName = TextWidget.Cast(m_wLayoutRoot.FindAnyWidget("Title"));
		m_wTraderName.SetText(m_TraderConfigData.GetTraderName());
				
		m_wTabContent = VerticalLayoutWidget.Cast(m_wLayoutRoot.FindAnyWidget("TabViewRoot"));
		if (!m_wTabContent)
			return;
		
		m_TabViewComponent = SCR_TabViewComponent.Cast(m_wTabContent.FindHandler(SCR_TabViewComponent));
		SCR_TabViewContent activeTab = m_TabViewComponent.GetShownTabComponent();
		if (activeTab)
		{
			CreateItemUIElements(m_TabViewComponent, activeTab.m_wTab);
		}
		
		//m_TabViewComponent.m_OnContentCreate.Insert(CreateItemUIElements);
		m_TabViewComponent.m_OnContentShow.Insert(CreateItemUIElements);
	}
	
	//------------------------------------------------------------------------------------------------
	void CreateItemUIElements(SCR_TabViewComponent comp, Widget scrollerRoot)
	{
		if (comp.m_iSelectedTab == EL_TraderMenuUITab.TAB_BUY)
		{
			CreateItems(m_aTraderItemBuyList, scrollerRoot);
		}
		else if (comp.m_iSelectedTab == EL_TraderMenuUITab.TAB_SELL)
		{
			CreateItems(m_aTraderItemSellList, scrollerRoot);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void CreateItems(array<ref EL_TraderItemInfo> items, Widget scrollerRoot)
	{
		GridLayoutWidget grid = GridLayoutWidget.Cast(scrollerRoot.FindAnyWidget("ContentGrid"));
		if (!grid)
			return;
		
		if (!m_aTraderItemUIElements)
		{
			m_aTraderItemUIElements = new array<ref EL_TraderMenuUI_ItemElement>;
		}
		else
		{
			m_aTraderItemUIElements.Clear();
		}
		
		int column, line;
		for (int i = 0; i < items.Count(); i++) 
		{
			EL_TraderMenuUI_ItemElement itemElement = new EL_TraderMenuUI_ItemElement(this, grid, items[i]);
			m_aTraderItemUIElements.Insert(itemElement);
						
			GridSlot.SetColumn(itemElement.GetRootWidget(), column);
			GridSlot.SetRow(itemElement.GetRootWidget(), line);
			GridSlot.SetPadding(itemElement.GetRootWidget(), 4.0 , 5.0, 0.0, 0.0);
			
			column++;
			if (column >= S_TILES_GRID_WIDTH)
			{
				column = 0;
				line += 1;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Init()
	{
		if (!m_wLayoutRoot)
			m_wLayoutRoot = GetRootWidget();
		
		//! For testing only. Need to be handled by the trader entity component later
		SetTraderConfig("{B443D5321B09D546}Configs/Traders/EL_AppleTraderConfig.conf");
		
		GetTraderInfoFromConfig();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetTraderConfig(ResourceName traderConfig)
	{
		m_TraderConfigResource = traderConfig
	}

	//------------------------------------------------------------------------------------------------
	void GetTraderInfoFromConfig()
	{	
		if (m_TraderConfigResource == string.Empty)
			return;
		
		//Parse & register tarder configurations	
		m_aTraderItemBuyList = new array<ref EL_TraderItemInfo>;
		m_aTraderItemSellList = new array<ref EL_TraderItemInfo>;
		
		Resource container = BaseContainerTools.LoadContainer(m_TraderConfigResource);
		if (!container || !container.IsValid())
			return;

		m_TraderConfigData = EL_TraderInfoList.Cast(BaseContainerTools.CreateInstanceFromContainer(container.GetResource().ToBaseContainer()));
		m_TraderConfigData.GetTraderItemBuyList(m_aTraderItemBuyList);
		m_TraderConfigData.GetTraderItemSellList(m_aTraderItemSellList);
	}
};