class EL_TraderMenuUI: SCR_InventoryMenuUI
{
	protected RichTextWidget m_wTraderName;
	protected ButtonWidget m_wButtonShowSellSection;
	protected ButtonWidget m_wButtonShowBuySection;
	protected ResourceName m_TraderConfig;
	protected ref array<ref EL_TraderItemInfo> m_aTraderItemBuyList;
	protected ref array<ref EL_TraderItemInfo> m_aTraderItemSellList;
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		super.OnMenuUpdate(tDelta);
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		m_wTraderName = RichTextWidget.Cast(m_widget.FindAnyWidget("TraderName"));
		m_wButtonShowSellSection = ButtonWidget.Cast(m_widget.FindAnyWidget("TabSellButton"));
		if(m_wButtonShowSellSection) {
			m_wButtonShowSellSection.AddHandler(new EL_ShowSellSectionButton(this));
		}
		
		m_wButtonShowSellSection = ButtonWidget.Cast(m_widget.FindAnyWidget("TabSellButton"));
		if(m_wButtonShowSellSection) {
			m_wButtonShowSellSection.AddHandler(new EL_ShowBuySectionButton(this));
		}
		
		HideInventoryElements();
	}
	
	//------------------------------------------------------------------------------------------------
	void HideInventoryElements()
	{
		m_wPlayerRender.SetVisible(false);
		m_wPlayerRenderSmall.SetVisible(false);
		m_wStorageList.SetVisible(false);
		m_wLootStorage.SetVisible(false);
		m_wQuickSlotStorage.SetVisible(false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool Init()
	{
		return super.Init();
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void Action_CloseInventory()
	{
		GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.ELTraderMenu);
		
		super.Action_CloseInventory();
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void RefreshPlayerWidget()
	{
		//! Do nothing - YIKES
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnAction(SCR_NavigationButtonComponent comp, string action, SCR_InventoryStorageBaseUI pParentStorage = null, int traverseStorageIndex = -1)
	{
		//! Do nothing - YIKES
	}
	
	//------------------------------------------------------------------------------------------------
	void SetTraderConfig(ResourceName traderConfig)
	{
		m_TraderConfig = traderConfig
	}
	
	//------------------------------------------------------------------------------------------------
	void ShowSellSection()
	{
		Print(ToString() + "::ShowSellSection - Start");
		
		Print(ToString() + "::ShowSellSection - End");
	}
	
	//------------------------------------------------------------------------------------------------
	void ShowBuySection()
	{
		Print(ToString() + "::ShowBuySection - Start");
		
		Print(ToString() + "::ShowBuySection - End");
	}
	
	//------------------------------------------------------------------------------------------------
	void GetTraderInfoFromConfig()
	{
		//Parse & register tarder configurations	
		m_aTraderItemBuyList = new array<ref EL_TraderItemInfo>;
		m_aTraderItemSellList = new array<ref EL_TraderItemInfo>;
		
		Resource container = BaseContainerTools.LoadContainer(m_TraderConfig);
		if (container && container.IsValid()) {
			EL_TraderInfoList list = EL_TraderInfoList.Cast(BaseContainerTools.CreateInstanceFromContainer(container.GetResource().ToBaseContainer()));
			list.GetTraderItemBuyList(m_aTraderItemBuyList);
			list.GetTraderItemSellList(m_aTraderItemSellList);
		}
	}
};

class EL_ShowSellSectionButton : BasicButtonComponent
{
	protected EL_TraderMenuUI m_TraderMenu;
	protected Widget m_widget;
	
	//------------------------------------------------------------------------------------------------	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		m_TraderMenu.ShowSellSection();
		return true;
	}
		
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_widget = w;
	}
			
	//------------------------------------------------------------------------------------------------
	void EL_ShowSellSectionButton(EL_TraderMenuUI menu)
	{
		m_TraderMenu = menu;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~EL_ShowSellSectionButton()
	{
	}
};

class EL_ShowBuySectionButton : BasicButtonComponent
{
	protected EL_TraderMenuUI m_TraderMenu;
	protected Widget m_widget;
	
	//------------------------------------------------------------------------------------------------	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		m_TraderMenu.ShowBuySection();
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_widget = w;
	}
			
	//------------------------------------------------------------------------------------------------
	void EL_ShowBuySectionButton(EL_TraderMenuUI menu)
	{
		m_TraderMenu = menu;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~EL_ShowBuySectionButton()
	{
	}
};