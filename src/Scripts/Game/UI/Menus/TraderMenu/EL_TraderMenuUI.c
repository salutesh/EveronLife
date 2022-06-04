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
	
	protected ButtonWidget m_wNextPage;
	protected SCR_PagingButtonComponent m_NextPageButton;
	
	protected ButtonWidget m_wPrevPage;
	protected SCR_PagingButtonComponent m_PrevPageButton;
	
	protected SCR_TabViewComponent m_TabViewComponent;
	
	protected ResourceName m_TraderConfigResource;
	protected ref EL_TraderInfoList m_TraderConfigData;
	protected ref array<ref EL_TraderItemInfo> m_aTraderItemBuyList;
	protected ref array<ref EL_TraderItemInfo> m_aTraderItemSellList;
	
	protected ref array<ref EL_TraderMenuUI_ItemElement> m_aTraderItemUIElements;
	protected const int TILES_GRID_WIDTH = 1;
	protected const int MAX_ITEMS_PER_PAGE = 32;
	
	protected RichTextWidget m_wPagesText;
	
	protected SCR_NavigationButtonComponent	m_CloseButton;
	
	protected int m_CurrentPage = 1;
	protected int m_LastPageItemIndex = 0;
	
	//------------------------------------------------------------------------------------------------
	void EL_TraderMenuUI()
	{
	
	}
	
	//------------------------------------------------------------------------------------------------
	void ~EL_TraderMenuUI()
	{
		m_aTraderItemUIElements.Clear();
	}
		
	//------------------------------------------------------------------------------------------------
	override void OnMenuInit()
	{
		super.OnMenuInit();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		super.OnMenuUpdate(tDelta);
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
	void CreateItems(array<ref EL_TraderItemInfo> items, Widget scrollerRoot, int itemsCount = -1)
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
		
		int itemsToAdd = 0;
		if (itemsCount > -1 && itemsCount < MAX_ITEMS_PER_PAGE)
		{
			itemsToAdd = itemsCount;
		}
		else if (items.Count() < MAX_ITEMS_PER_PAGE)
		{
			itemsToAdd = items.Count();
		}
		else if (items.Count() > MAX_ITEMS_PER_PAGE || itemsCount > MAX_ITEMS_PER_PAGE)
		{
			itemsToAdd = MAX_ITEMS_PER_PAGE;
		}
		
		int column, line;
		for (int i = 0; i < itemsToAdd; i++) 
		{
			EL_TraderMenuUI_ItemElement itemElement = new EL_TraderMenuUI_ItemElement(this, grid, items[i]);
			m_aTraderItemUIElements.Insert(itemElement);
						
			GridSlot.SetColumn(itemElement.GetRootWidget(), column);
			GridSlot.SetRow(itemElement.GetRootWidget(), line);
			GridSlot.SetPadding(itemElement.GetRootWidget(), 5.0 , 5.0, 0.0, 0.0);
			
			column++;
			if (column >= TILES_GRID_WIDTH)
			{
				column = 0;
				line += 1;
			}
		}
		
		m_LastPageItemIndex = itemsToAdd;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Init()
	{
		if (!m_wLayoutRoot)
			m_wLayoutRoot = GetRootWidget();
		
		m_CloseButton = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Back", m_wLayoutRoot);
		if (!m_CloseButton)
			return;
		
		m_CloseButton.m_OnActivated.Insert(CloseTraderMenu);
			
		GetTraderInfoFromConfig();
		
		if (!m_TraderConfigData)
			return;
		
		m_wTraderName = TextWidget.Cast(m_wLayoutRoot.FindAnyWidget("Title"));
		m_wTraderName.SetText(m_TraderConfigData.GetTraderName());

		m_wPagesText = RichTextWidget.Cast(m_wLayoutRoot.FindAnyWidget("PagesText"));
		
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
				
		m_wNextPage = ButtonWidget.Cast(m_wLayoutRoot.FindAnyWidget("ButtonRight"));
		m_NextPageButton = SCR_PagingButtonComponent.Cast(m_wNextPage.FindHandler(SCR_PagingButtonComponent));
		if (!m_NextPageButton)
			return;
		
		m_NextPageButton.m_OnClicked.Insert(OnNextPageButtonClick);
		
		m_wPrevPage = ButtonWidget.Cast(m_wLayoutRoot.FindAnyWidget("ButtonLeft"));
		m_PrevPageButton = SCR_PagingButtonComponent.Cast(m_wPrevPage.FindHandler(SCR_PagingButtonComponent));
		if (!m_PrevPageButton)
			return;
		
		m_PrevPageButton.m_OnClicked.Insert(OnPreviousPageButtonClick);
		
		int pageCount = m_aTraderItemUIElements.Count() / MAX_ITEMS_PER_PAGE;
		if (pageCount > 1)
		{
			m_wPagesText.SetText(string.Format("%1/%2", 1, pageCount));
		}
		else
		{
			m_wPagesText.SetText(string.Format("%1/%2", 1, 1));
			m_wPrevPage.SetVisible(false);
			m_wNextPage.SetVisible(false);
		}
		
		SCR_UISoundEntity.SoundEvent("SOUND_INV_OPEN");
	}
	
	//------------------------------------------------------------------------------------------------
	void SetTraderConfig(ResourceName traderConfig)
	{
		m_TraderConfigResource = traderConfig;
		
		Init();
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
	
	//------------------------------------------------------------------------------------------------
	void SetSelectedItem(EL_TraderItemInfo item)
	{
		//! TODO
	}
	
	//------------------------------------------------------------------------------------------------
	void CloseTraderMenu()
	{
		GetGame().GetMenuManager().CloseMenu(this);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnNextPageButtonClick()
	{
		//! TODO
	}
	
	//------------------------------------------------------------------------------------------------
	void OnPreviousPageButtonClick()
	{
		//! TODO
	}
};