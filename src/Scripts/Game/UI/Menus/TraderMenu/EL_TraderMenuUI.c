enum EL_TraderMenuUITab
{
	TAB_BUY = 0,
	TAB_SELL = 1,
	TAB_BUYBACK = 2
}


modded enum EMenuAction
{
	ACTION_TRADE_SELL,
	ACTION_TRADE_BUY,
	ACTION_QUANTITY_ADD,
	ACTION_QUANTITY_REMOVE
};

#define EL_TRADER_SYSTEM_DEBUG;

class EL_TraderMenuUI: SCR_InventoryMenuUI
{
	//------------------------------------------------------------------------------------------------
	//! MEMBER VARIABLES
	//------------------------------------------------------------------------------------------------
	//! COMPONENTS
	protected SCR_PagingButtonComponent m_NextPageButton;
	protected SCR_PagingButtonComponent m_PrevPageButton;
	protected SCR_TabViewComponent m_TabViewComponent;
	
	protected EL_TraderMenuUI_ItemElement m_pFocusedItemElement;
	protected EL_TraderMenuUI_ItemElement m_pSelectedItemElement;
	//protected EL_TraderMenuUI_ItemElementComponent m_SelectedItemComponent;
	//protected EL_TraderMenuUI_ItemElementQuantityComponent m_SelectedItemQuantityComponent;
	
	//! MISC
	protected ResourceName m_TraderConfigResource;
	//protected EL_TraderItemInfo m_SelectedItem;
	protected ref EL_TraderInfoList m_TraderConfigData;
	protected ref array<ref EL_TraderItemInfo> m_aTraderItemBuyList = {};
	protected ref array<ref EL_TraderItemInfo> m_aTraderItemSellList = {};
	protected ref array<ref EL_TraderMenuUI_ItemElement> m_aTraderPurchasableItemUIElements = {};
	protected ref array<ref EL_TraderMenuUI_ItemElement> m_aTraderSellableItemUIElements = {};
	protected const int MAX_ITEMS_PER_PAGE = 32;
	protected int m_CurrentPage = 1;
	protected int m_LastPageItemIndex = 0;

	protected SCR_TabViewContent m_TabBuy;
	protected SCR_TabViewContent m_TabSell;
	protected SCR_TabViewContent m_TabBuyBack;
	
	//! WIDGETS
	protected Widget m_wLayoutRoot;
	protected TextWidget m_wTraderName;
	protected OverlayWidget m_wContentWidget;
	protected VerticalLayoutWidget m_wTabContent;
	protected ButtonWidget m_wNextPage;
	protected ButtonWidget m_wPrevPage;
	protected RichTextWidget m_wPagesText;
	
	//------------------------------------------------------------------------------------------------
	//! MEMBER METHODS
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	protected void InitTraderMenu()
	{
		DebugPrint("::InitTraderMenu - Start");
				
		if (m_TraderConfigResource == string.Empty)
		{
			Print(ToString() + "::GetTraderInfoFromConfig - ERROR - Can't get m_TraderConfigResource!", LogLevel.ERROR);
			return;
		}

		//Parse & register tarder configurations
		Resource container = BaseContainerTools.LoadContainer(m_TraderConfigResource);
		if (!container || !container.IsValid())
		{
			Print(ToString() + "::GetTraderInfoFromConfig - ERROR - Can't get Resource!", LogLevel.ERROR);
			return;
		}

		m_TraderConfigData = EL_TraderInfoList.Cast(BaseContainerTools.CreateInstanceFromContainer(container.GetResource().ToBaseContainer()));
		if (!m_TraderConfigData)
		{
			Print(ToString() + "::GetTraderInfoFromConfig - ERROR - Can't get EL_TraderInfoList!", LogLevel.ERROR);
			return;
		}
		
		if (!m_aTraderItemBuyList)
			m_aTraderItemBuyList = new array<ref EL_TraderItemInfo>;
		
		if (!m_aTraderItemSellList)
			m_aTraderItemSellList = new array<ref EL_TraderItemInfo>;
		
		m_TraderConfigData.GetTraderItemBuyList(m_aTraderItemBuyList);
		m_TraderConfigData.GetTraderItemSellList(m_aTraderItemSellList);

		if (!m_TraderConfigData)
		{
			Print(ToString() + "::InitTraderMenu - ERROR - Can't get trader config!", LogLevel.ERROR);
			return;
		}

		m_wTraderName.SetText(m_TraderConfigData.GetTraderName());
		
		CreateItemUIElements();
		
		UpdateView();
		
		DebugPrint("::InitTraderMenu - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateView()
	{
		DebugPrint("::UpdateView - Start");
			
		GetSellablePlayerItems();
		UpdatePagesText();
		
		//! Recreate all existing storages and update there view (UI elements).
		ShowStoragesList();
        ShowAllStoragesInList();
		
		int index;
		ButtonWidget focusedWidget;
		if (m_TabViewComponent.m_iSelectedTab == EL_TraderMenuUITab.TAB_BUY)
		{
			focusedWidget = ButtonWidget.Cast(m_aTraderPurchasableItemUIElements[0].GetRootWidget());
		}
		else if (m_TabViewComponent.m_iSelectedTab == EL_TraderMenuUITab.TAB_SELL)
		{
			focusedWidget = ButtonWidget.Cast(m_aTraderPurchasableItemUIElements[0].GetRootWidget());
		}
		
		GetGame().GetWorkspace().SetFocusedWidget(focusedWidget);
		
		DebugPrint("::UpdateView - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void GetSellablePlayerItems()
	{
		DebugPrint("::GetSellablePlayerItems - Start");

		DebugPrint("::GetSellablePlayerItems - Sellable items list: " + m_aTraderItemSellList.ToString());
		
		SCR_PrefabNamePredicate	pSearchPredicate = new SCR_PrefabNamePredicate();
		foreach (EL_TraderItemInfo itemInfo: m_aTraderItemSellList)
		{
			DebugPrint("::GetSellablePlayerItems - EL_TraderItemInfo: " + itemInfo);
			
			array<IEntity> foundItems = {};
			ResourceName itemResourceName = itemInfo.GetResourceName();			
			pSearchPredicate.prefabName = itemResourceName;
			m_InventoryManager.FindItems(foundItems, pSearchPredicate, EStoragePurpose.PURPOSE_DEPOSIT);

			EL_TraderMenuUI_ItemElement itemElement = GetSellableItemElementByResourceName(itemResourceName);
			if (!itemElement)
				return;

			DebugPrint("::GetSellablePlayerItems - Found items for: " + itemResourceName + " | Count: " + foundItems.Count());

			if (foundItems.Count() == 0)
			{
				itemElement.SetVisible(false);
			}
			else
			{
				itemElement.SetQuantityMax(foundItems.Count());
				itemElement.SetQuantity(foundItems.Count());
			}
		}

		DebugPrint("::GetSellablePlayerItems - End");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Gets set from menu action
	void SetTraderConfig(ResourceName traderConfig)
	{
		DebugPrint("::SetTraderConfig - Start");
		
		m_TraderConfigResource = traderConfig;
		
		InitTraderMenu();
		
		DebugPrint("::SetTraderConfig - End");
	}
	
	//------------------------------------------------------------------------------------------------
	void SetFocusedItemElement(EL_TraderMenuUI_ItemElement itemElement)
	{
		DebugPrint("::SetFocusedItemElement - Start");
		DebugPrint("::SetFocusedItemElement - Set element to: " + itemElement.ToString());
		
		m_pFocusedItemElement = itemElement;
		
		NavigationBarUpdate();
		
		DebugPrint("::SetFocusedItemElement - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CreateItemUIElements()
	{
		DebugPrint("::CreateItemUIElements - Start");
		
		if (!m_TabViewComponent)
			return;
		
		if (!m_aTraderItemBuyList || m_aTraderItemBuyList.Count() == 0)
			return;

		CreateItems(m_aTraderItemBuyList, m_TabBuy.m_wTab, EL_TraderMenuUITab.TAB_BUY);
		
		if (!m_aTraderItemSellList || m_aTraderItemSellList.Count() == 0)
			return;
			
		CreateItems(m_aTraderItemSellList, m_TabSell.m_wTab, EL_TraderMenuUITab.TAB_SELL);
		
		DebugPrint("::CreateItemUIElements - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CreateItems(array<ref EL_TraderItemInfo> items, Widget tabRoot, EL_TraderMenuUITab tab, int itemsCount = -1)
	{
		DebugPrint("::CreateItems - Start");
		
		VerticalLayoutWidget list = VerticalLayoutWidget.Cast(tabRoot.FindAnyWidget("ItemList"));
		if (!list)
			return;

		if (!m_aTraderSellableItemUIElements)
			m_aTraderSellableItemUIElements = new array<ref EL_TraderMenuUI_ItemElement>;
		
		if (!m_aTraderPurchasableItemUIElements)
			m_aTraderPurchasableItemUIElements = new array<ref EL_TraderMenuUI_ItemElement>;

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
			EL_TraderMenuUI_ItemElement itemElement = new EL_TraderMenuUI_ItemElement(this, list, items[i]);
			
			if (tab == EL_TraderMenuUITab.TAB_BUY)
			{
				m_aTraderPurchasableItemUIElements.Insert(itemElement);
			}
			else if (tab == EL_TraderMenuUITab.TAB_SELL)
			{
				m_aTraderSellableItemUIElements.Insert(itemElement);
			}
		}

		m_LastPageItemIndex = itemsToAdd;
		
		DebugPrint("::CreateItems - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdatePagesText()
	{
		DebugPrint("::UpdatePagesText - Start");
		
		if (!m_wPagesText)
			return;

		int pageCount = m_aTraderPurchasableItemUIElements.Count() / MAX_ITEMS_PER_PAGE;
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
		
		DebugPrint("::UpdatePagesText - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void HideInventoryElements()
	{
		DebugPrint("::HideInventoryElements - Start");
		
		m_wPlayerRender.SetVisible(false);
		m_wPlayerRenderSmall.SetVisible(false);
		m_wLootStorage.SetVisible(false);
		m_wQuickSlotStorage.SetVisible(false);
		
		DebugPrint("::HideInventoryElements - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Action_TrySellItem()
	{
		DebugPrint("::Action_TrySellItem - Start");
		
		SimpleFSM(EMenuAction.ACTION_TRADE_SELL);
		
		DebugPrint("::Action_TrySellItem - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Action_QuantityAdd()
	{
		DebugPrint("::Action_AddQuantity - Start");
		
		SimpleFSM(EMenuAction.ACTION_QUANTITY_ADD);
		
		DebugPrint("::Action_AddQuantity - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Action_QuantityRemove()
	{
		DebugPrint("::Action_QuantityRemove - Start");
		
		SimpleFSM(EMenuAction.ACTION_QUANTITY_REMOVE);
		
		DebugPrint("::Action_QuantityRemove - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnNextPageButtonClick()
	{
		DebugPrint("::OnNextPageButtonClick - Start");
		
		//! TODO
		
		DebugPrint("::OnNextPageButtonClick - End");
	}

	//------------------------------------------------------------------------------------------------
	protected void OnPreviousPageButtonClick()
	{
		DebugPrint("::OnPreviousPageButtonClick - Start");
		
		//! TODO
		
		DebugPrint("::OnPreviousPageButtonClick - End");
	}
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetSelectedItemResourceName()
	{
		if (m_pSelectedSlotUI && m_pSelectedSlotUI != null)
		{
			InventoryItemComponent pComp = m_pSelectedSlotUI.GetInventoryItemComponent();
			if (pComp)
			{
				IEntity previewEntity = pComp.GetOwner();
				if (previewEntity)
					return previewEntity.GetPrefabData().GetPrefabName();
			}
		}
		
		return string.Empty;
	}
	
	//------------------------------------------------------------------------------------------------
	EL_TraderMenuUI_ItemElement GetSellableItemElementByResourceName(ResourceName resourceName)
	{
		foreach(EL_TraderMenuUI_ItemElement itemElement: m_aTraderSellableItemUIElements)
		{
			if (itemElement.GetItemResourceName() == resourceName)
				return itemElement;
		}

		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	//! BASE CLASS OVERRIDES
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	protected override void InitializeCharacterInformation()
	{
		DebugPrint("::InitializeCharacterInformation - Start");
		
		super.InitializeCharacterInformation();
		
		DebugPrint("::InitializeCharacterInformation - End");
	}
	
	//------------------------------------------------------------------------------------------------
	override void RegisterUIStorage(SCR_InventorySlotUI pStorageUI)
	{
		DebugPrint("::RegisterUIStorage - Start");
		
		super.RegisterUIStorage(pStorageUI);
		
		DebugPrint("::RegisterUIStorage - End");
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetActiveStorage(SCR_InventoryStorageBaseUI storageUI)
	{
		DebugPrint("::SetActiveStorage - Start");
		
		super.SetActiveStorage(storageUI);
		
		DebugPrint("::SetActiveStorage - End");
	}
	
	//------------------------------------------------------------------------------------------------
	override void UnregisterUIStorage( SCR_InventorySlotUI pStorageUI )
	{
		DebugPrint("::UnregisterUIStorage - Start");
		
		super.UnregisterUIStorage(pStorageUI);
		
		DebugPrint("::UnregisterUIStorage - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void OnMenuOpen()
	{
		DebugPrint("::OnMenuOpen - Start");
		
		super.OnMenuOpen();
		
		HideInventoryElements(); //! Hide vanilla inventory UI elements we not need and we dont want to display
		
		if (!m_wLayoutRoot)
			m_wLayoutRoot = GetRootWidget();
		
		m_wTraderName = TextWidget.Cast(m_wLayoutRoot.FindAnyWidget("Title"));
		
		m_wPagesText = RichTextWidget.Cast(m_wLayoutRoot.FindAnyWidget("PagesText"));
		m_wTabContent = VerticalLayoutWidget.Cast(m_wLayoutRoot.FindAnyWidget("TabViewRoot"));
		if (!m_wTabContent)
		{
			Print(ToString() + "::InitTraderMenu - ERROR - Can't get m_wTabContent widget!", LogLevel.ERROR);
			return;
		}

		m_TabViewComponent = SCR_TabViewComponent.Cast(m_wTabContent.FindHandler(SCR_TabViewComponent));
		if (!m_TabViewComponent)
		{
			Print(ToString() + "::InitTraderMenu - ERROR - Can't get SCR_TabViewComponent!", LogLevel.ERROR);
			return;
		}
		
		m_TabBuy = m_TabViewComponent.GetTabComponentByIndex(0);
		if (!m_TabBuy)
		{
			Print(ToString() + "::InitTraderMenu - ERROR - Can't get buy tab from SCR_TabViewComponent!", LogLevel.ERROR);
			return;
		}
		
		m_TabSell = m_TabViewComponent.GetTabComponentByIndex(1);
		if (!m_TabSell)
		{
			Print(ToString() + "::InitTraderMenu - ERROR - Can't get sell tab from SCR_TabViewComponent!", LogLevel.ERROR);
			return;
		}
		
		m_TabBuyBack = m_TabViewComponent.GetTabComponentByIndex(2);
		if (!m_TabBuyBack)
		{
			Print(ToString() + "::InitTraderMenu - ERROR - Can't get buyback tab from SCR_TabViewComponent!", LogLevel.ERROR);
			return;
		}
		
		m_TabViewComponent.m_OnContentShow.Insert(UpdateView);
		
		m_wNextPage = ButtonWidget.Cast(m_wLayoutRoot.FindAnyWidget("ButtonRight"));
		m_NextPageButton = SCR_PagingButtonComponent.Cast(m_wNextPage.FindHandler(SCR_PagingButtonComponent));
		if (!m_NextPageButton)
		{
			Print(ToString() + "::InitTraderMenu - ERROR - Can't get SCR_PagingButtonComponent!", LogLevel.ERROR);
			return;
		}

		m_NextPageButton.m_OnClicked.Insert(OnNextPageButtonClick);

		m_wPrevPage = ButtonWidget.Cast(m_wLayoutRoot.FindAnyWidget("ButtonLeft"));
		m_PrevPageButton = SCR_PagingButtonComponent.Cast(m_wPrevPage.FindHandler(SCR_PagingButtonComponent));
		if (!m_PrevPageButton)
		{
			Print(ToString() + "::InitTraderMenu - ERROR - Can't get SCR_PagingButtonComponent!", LogLevel.ERROR);
			return;
		}

		m_PrevPageButton.m_OnClicked.Insert(OnPreviousPageButtonClick);
		
		DebugPrint("::OnMenuOpen - End");
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		super.OnMenuUpdate(tDelta);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MoveItem(SCR_InventoryStorageBaseUI pStorageBaseUI = null)
	{
		DebugPrint("::MoveItem - Start");
		
		if (!m_pSelectedSlotUI)
			return;
		
		InventoryItemComponent pComp = m_pSelectedSlotUI.GetInventoryItemComponent();
		if (!pComp)
			return;
		
		IEntity pItem = pComp.GetOwner();
		if (!m_InventoryManager.CanMoveItem(pItem))
			return;
		
		m_pCallBack.m_pItem = pItem;
		m_pCallBack.m_pMenu = this;
		m_pCallBack.m_pStorageToFocus = m_pSelectedSlotUI.GetStorageUI();

		m_pCallBack.m_pStorageFrom = m_pSelectedSlotUI.GetStorageUI();
		if (pStorageBaseUI)
		{
			m_pCallBack.m_pStorageTo = pStorageBaseUI;
		}
		else
		{
			m_pCallBack.m_pStorageTo = m_pActiveHoveredStorageUI;
		}

		BaseInventoryStorageComponent pStorageTo = m_pActiveHoveredStorageUI.GetCurrentNavigationStorage();
		
		if (pStorageTo && EquipedWeaponStorageComponent.Cast(pStorageTo))
		{
			m_InventoryManager.EquipWeapon(pItem, m_pCallBack, m_pCallBack.m_pStorageFrom == m_pStorageLootUI);
			SCR_UISoundEntity.SoundEvent("SOUND_INV_VINICITY_EQUIP_CLICK");
			return;
		}
		else if (pStorageTo && CharacterInventoryStorageComponent.Cast(pStorageTo))
		{
			m_InventoryManager.EquipAny(pStorageTo, pItem, 0, m_pCallBack);
		}
		else
		{
			m_InventoryManager.InsertItem(pItem, m_pActiveHoveredStorageUI.GetCurrentNavigationStorage(), m_pCallBack.m_pStorageFrom.GetStorage(), m_pCallBack);
			SCR_UISoundEntity.SoundEvent("SOUND_INV_CONTAINER_DIFR_DROP");
		}
		
		DebugPrint("::MoveItem - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void Action_CloseInventory()
	{
		DebugPrint("::Action_CloseInventory - Start");
		
		super.Action_CloseInventory();
		
		GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.EL_TraderMenu);
		
		DebugPrint("::Action_CloseInventory - End");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns whether interaction with inventory items is possible
	override bool GetCanInteract()
	{
		if (GetInspectionScreen())
			return false;

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! when clicked on item / storage slot using controller or mouse
	protected override void Action_SelectItem()
	{
		// Ignore if we cannot interact
		if (!GetCanInteract())
			return;
		
		SimpleFSM(EMenuAction.ACTION_SELECT);
	}
	
	//------------------------------------------------------------------------------------------------
	override void NavigationBarUpdate()
	{
		DebugPrint("::NavigationBarUpdate - Start");
		DebugPrint("::NavigationBarUpdate - m_pFocusedSlotUI: " + m_pFocusedSlotUI.ToString());
		DebugPrint("::NavigationBarUpdate - m_pFocusedItemElement: " + m_pFocusedItemElement.ToString());
		
		if (!m_pNavigationBar)
			return;

		if (IsUsingGamepad())
		{
			NavigationBarUpdateGamepad();
			return;
		}

		m_pNavigationBar.SetAllButtonEnabled(false);
		m_pNavigationBar.SetButtonEnabled("ButtonClose", true);

		if (m_pFocusedSlotUI)
		{
			DebugPrint("::NavigationBarUpdate - Element is Inventory Slot - Enable Buttons");
			
			if (m_pFocusedSlotUI.GetStorageUI() == m_pQuickSlotStorage)
				return;
	
			m_pNavigationBar.SetButtonEnabled("ButtonSelect", true);
			m_pNavigationBar.SetButtonEnabled("ButtonSell", true);
					
			if (m_pFocusedSlotUI.GetStorageUI() != m_pStorageLootUI)
				m_pNavigationBar.SetButtonActionName("ButtonSell", "Trade_Sell");
	
			m_pNavigationBar.SetButtonEnabled("ButtonStepBack", true);
			m_pNavigationBar.SetButtonEnabled("ButtonInspect", true);
	
			HandleSlottedItemFunction();
		}
		else if (m_pFocusedItemElement)
		{
			DebugPrint("::NavigationBarUpdate - Element is Trader item element - Enable Buttons");
			
			m_pNavigationBar.SetButtonEnabled("ButtonSelect", true);
			m_pNavigationBar.SetButtonActionName("ButtonQuantityAdd", "Quantity_Add");
			m_pNavigationBar.SetButtonEnabled("ButtonQuantityAdd", true);
			m_pNavigationBar.SetButtonActionName("ButtonQuantityRemove", "Quantity_Remove");
			m_pNavigationBar.SetButtonEnabled("ButtonQuantityRemove", true);
			
			if (m_TabViewComponent.m_iSelectedTab == EL_TraderMenuUITab.TAB_SELL)
			{
				m_pNavigationBar.SetButtonEnabled("ButtonSell", true);
			}
			else if (m_TabViewComponent.m_iSelectedTab == EL_TraderMenuUITab.TAB_BUY)
			{
				m_pNavigationBar.SetButtonEnabled("ButtonBuy", true);
			}
		}
		
		DebugPrint("::NavigationBarUpdate - End");
	}
	
	//------------------------------------------------------------------------------------------------
	override void NavigationBarUpdateGamepad()
	{
		DebugPrint("::NavigationBarUpdateGamepad - Start");
		DebugPrint("::NavigationBarUpdateGamepad - m_pFocusedSlotUI: " + m_pFocusedSlotUI.ToString());
		DebugPrint("::NavigationBarUpdateGamepad - m_pFocusedItemElement: " + m_pFocusedItemElement.ToString());
		
		m_pNavigationBar.SetAllButtonEnabled(false);
		m_pNavigationBar.SetButtonEnabled("ButtonSelect", true);

		if (m_bStorageSwitchMode)
		{
			m_CloseButton.SetLabel("#AR-Inventory_Close");
			bool shouldShowMove = (m_pSelectedSlotUI != null);
			
			if (m_pActiveStorageUI)
				shouldShowMove &= m_pActiveStorageUI.IsStorageHighlighted();
			
			m_pNavigationBar.SetButtonEnabled("ButtonMove", shouldShowMove);
			m_pNavigationBar.SetButtonEnabled("ButtonSelect", !m_pSelectedSlotUI);
		}
		else
		{
			m_pNavigationBar.SetButtonEnabled("ButtonMove", m_pSelectedSlotUI != null);
			m_pNavigationBar.SetButtonEnabled("ButtonSwap", m_pSelectedSlotUI != null);
		}

		if (!m_bStorageSwitchMode && m_pActiveStorageUI != m_pStorageLootUI && m_pActiveStorageUI != m_pStorageListUI)
		{
			m_pNavigationBar.SetButtonEnabled("ButtonQuickSlotAssign", true);
		}

		if (m_pActiveStorageUI == m_pQuickSlotStorage)
		{
			bool itmToAssign = m_pItemToAssign != null;
			m_pNavigationBar.SetAllButtonEnabled(false);
			m_pNavigationBar.SetButtonEnabled("ButtonClose", true);
			
			if (m_bStorageSwitchMode)
				m_pNavigationBar.SetButtonEnabled("ButtonSelect", true);

			m_pNavigationBar.SetButtonEnabled("ButtonQuickSlotAssign", itmToAssign);
			m_pNavigationBar.SetButtonEnabled("ButtonQuickSlotUnassign", !itmToAssign && m_pFocusedSlotUI != null);
		}

		if (m_pFocusedSlotUI)
		{	
			DebugPrint("::NavigationBarUpdateGamepad - m_pFocusedSlotUI - ENABLE BUTTONS");
			
			if (m_CloseButton)
				m_CloseButton.SetLabel("#AR-Menu_Back");
			
			m_pNavigationBar.SetButtonEnabled("ButtonSell", m_pSelectedSlotUI == null);
			
			if (m_pFocusedSlotUI.GetStorageUI() != m_pStorageLootUI)
				m_pNavigationBar.SetButtonActionName("ButtonSell", "Trade_Sell");
	
			if (m_pActiveStorageUI != m_pQuickSlotStorage)
				HandleSlottedItemFunction();
		}
		else if (m_pFocusedItemElement)
		{
			DebugPrint("::NavigationBarUpdateGamepad - m_pFocusedItemElement - ENABLE BUTTONS");
			m_pNavigationBar.SetButtonEnabled("ButtonQuantityAdd", true);
			m_pNavigationBar.SetButtonActionName("ButtonQuantityAdd", "Quantity_Add");
			m_pNavigationBar.SetButtonEnabled("ButtonQuantityRemove", true);
			m_pNavigationBar.SetButtonActionName("ButtonQuantityRemove", "Quantity_Remove");
			
			if (m_TabViewComponent.m_iSelectedTab == EL_TraderMenuUITab.TAB_SELL)
			{
				m_pNavigationBar.SetButtonEnabled("ButtonSell", true);
			}
			else if (m_TabViewComponent.m_iSelectedTab == EL_TraderMenuUITab.TAB_BUY)
			{
				m_pNavigationBar.SetButtonEnabled("ButtonBuy", true);
			}
		}
		
		DebugPrint("::NavigationBarUpdateGamepad - End");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Handles events called from the SimpleFSM method.
	//------------------------------------------------------------------------------------------------
	override void OnAction(SCR_NavigationButtonComponent comp, string action, SCR_InventoryStorageBaseUI pParentStorage = null, int traverseStorageIndex = -1)
	{
		DebugPrint("::OnAction - Start");
		
		DebugPrint("::OnAction - SCR_NavigationButtonComponent: " + comp.ToString());
		DebugPrint("::OnAction - Action: " + action);
		DebugPrint("::OnAction - SCR_InventoryStorageBaseUI: " + pParentStorage.ToString());
		DebugPrint("::OnAction - Traverse Storage Index: " + traverseStorageIndex.ToString());
		
		switch (action)
		{
			case "Inventory":
			{
				Action_CloseInventory();
			}
			break;

			case "Inventory_UnassignFromQuickSlot":
			{
				Action_QuickSlotUnassign();
			} 
			break;

			case "Inventory_AssignToQuickSlot":
			{
				Action_QuickSlotAssign();
			} 
			break;

			case "Inventory_Equip":
			{
				Action_EquipItem();
			} 
			break;

			case "Inventory_Select":
			{
				if (m_bStorageSwitchMode && IsUsingGamepad())
				{
					SetStorageSwitchMode(false);
					return;
				}
				if (m_pFocusedSlotUI && !m_pFocusedSlotUI.IsSlotSelected() || m_pFocusedItemElement && !m_pFocusedItemElement.IsElementSelected())
					Action_SelectItem();
				else
					Action_DeselectItem();
			} 
			break;

			case "Inventory_Deselect":
			{
				Action_DeselectItem();
			} 
			break;

			case "Inventory_OpenStorage":
			{
				Action_UnfoldItem();
			} 
			break;

			case "Inventory_Back":
			{
				Action_StepBack(pParentStorage, traverseStorageIndex);
			} 
			break;

			case "Inventory_StepBack":
			{
				Action_StepBack(pParentStorage, traverseStorageIndex);
			} 
			break;

			case "Inventory_Inspect":
			{
				Action_Inspect();
			} 
			break;

			case "Inventory_Use":
			{
				Action_UseItem();
			} 
			break;

			case "Inventory_Move":
			{
				if (m_bStorageSwitchMode)
				{
					Action_Drop();
					return;
				}
				Action_MoveItemToStorage();
			} 
			break;

			case "Inventory_Swap":
			{
				if (m_bStorageSwitchMode)
				{
					ToggleStorageSwitchMode();
					FocusOnSlotInStorage(m_pActiveStorageUI);
					return;
				}
				Action_SwapItems(m_pSelectedSlotUI, m_pFocusedSlotUI);
			} 
			break;
			
			case "Trade_Sell":
			{
				Action_TrySellItem();
			} 
			break;
			
			case "Quantity_Add":
			{
				Action_QuantityAdd();
			}
			break;
			
			case "Quantity_Remove":
			{
				Action_QuantityRemove();
			}
			break;
		}
		
		DebugPrint("::OnAction - End");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Gets called by Trader_Sell action
	//! Take a look at the SimpleFSM and OnAnction functions for more information
	//! Action input is defined in "Configs/System/chimeraInputCommon.conf"
	//------------------------------------------------------------------------------------------------
	protected void TrySellItem()
	{
		DebugPrint("::TrySellItem - Start");
		
		IEntity pItem;
		ResourceName itemResourceName;
		if (m_pSelectedSlotUI)
		{
			pItem = m_pSelectedSlotUI.GetInventoryItemComponent().GetOwner();
			if (!pItem)
			{
				Print(ToString() + "::TrySellItem - ERROR - Can't get IEntity from selected UI element.", LogLevel.ERROR);
				return;
			}
			
			itemResourceName = pItem.GetPrefabData().GetPrefabName();
		}
		else if (m_pSelectedItemElement)
		{
			itemResourceName = m_pSelectedItemElement.GetItemResourceName();
		}

		if (itemResourceName == string.Empty)
		{
			Print(ToString() + "::TrySellItem - ERROR - No valid element selected!", LogLevel.ERROR);
			return;
		}
		
		array<IEntity> foundItems = new array<IEntity>;
		DebugPrint("::TrySellItem - Resource name: " + itemResourceName);
		
		SCR_PrefabNamePredicate	pSearchPredicate = new SCR_PrefabNamePredicate();
		pSearchPredicate.prefabName = itemResourceName;
		m_InventoryManager.FindItems(foundItems, pSearchPredicate, EStoragePurpose.PURPOSE_DEPOSIT);

		if (foundItems.Count() == 0)
			return;

		EL_TraderMenuUI_ItemElement itemElement = GetSellableItemElementByResourceName(itemResourceName);
		if (!itemElement)
			return;

		int quantityToSell = itemElement.GetQuantity();
		//! Can't sell more items then owned in storages
		if (foundItems.Count() < quantityToSell || quantityToSell == 0)
			return;

		DebugPrint("::TrySellItem - quantityToSell: " + quantityToSell);

		//m_pCallBack.m_pItem = pItem;
		//m_pCallBack.m_pMenu = this;
		//m_pCallBack.m_pStorageToFocus = m_pSelectedSlotUI.GetStorageUI();
		//m_pCallBack.m_sItemToFocus = m_pSelectedSlotUI.GetItemName();
		
		//BaseInventoryStorageComponent pStorageFrom = m_pSelectedSlotUI.GetStorageUI().GetStorage();
		
		//if (!m_InventoryManager.CanMoveItem(pItem))
			//return;
	 
		//m_pCallBack.m_pStorageFrom = m_pSelectedSlotUI.GetStorageUI();
		//m_pCallBack.m_pStorageTo = null;
				
		if(!m_InventoryManager.TryRemoveItemFromInventory(pItem, null, null))
		{
			Print(ToString() + "TrySellItem - ERROR - Can't remove slot from storgare!");
		}
		
		for (int i = 0; i < quantityToSell; i++)
		{
			IEntity item = foundItems[i];
			RplComponent rplComp = RplComponent.Cast(item.FindComponent(RplComponent));
			if (rplComp)
			{
				RplComponent.DeleteRplEntity(item, false);
			}
			else
			{
				delete item;
			}
		}
		
		UpdateView();

		DebugPrint("::TrySellItem - End");
	}
	
	//------------------------------------------------------------------------------------------------
	//! This method handles all the action calls executed by action inputs that are bound to
	//! this menu by the defined action context in the "Configs/System/chimeraInputCommon.conf" file.
	//! The used action inputs are set in the SCR_NavigationBarUI component that is set on the used menu layout.
	//! Action context: InventoryContext
	//! Layout path: UI/Layouts/Menus/Trader/EL_TraderMenu.layout
	//------------------------------------------------------------------------------------------------
	protected override void SimpleFSM(EMenuAction EAction = EMenuAction.ACTION_SELECT)
	{
		DebugPrint("::SimpleFSM - Start");
		
		switch (EAction)
		{
			case EMenuAction.ACTION_MOVEINSIDE:
			{
				DebugPrint("::SimpleFSM - ACTION_MOVEINSIDE");
				Action_MoveItemToStorage(m_pActiveStorageUI);
				
				if (IsUsingGamepad())
					SetStorageSwitchMode(true);
				
				NavigationBarUpdate();
				
				if (m_pSelectedSlotUI)
				{
					m_pSelectedSlotUI.SetSelected(false);
					m_pSelectedSlotUI = null;
				}
				
				FocusOnSlotInStorage(m_pActiveStorageUI);
				ResetHighlightsOnAvailableStorages();
			} 
			break;
			
			case EMenuAction.ACTION_SELECT:
			{
				DebugPrint("::SimpleFSM - ACTION_SELECT");
				DebugPrint("::SimpleFSM - m_pFocusedSlotUI: " + m_pFocusedSlotUI.ToString());
				DebugPrint("::SimpleFSM - m_pFocusedItemElement: " + m_pFocusedItemElement.ToString());
				
				if (m_pFocusedSlotUI)
				{
					DebugPrint("::SimpleFSM - Selected - m_pFocusedSlotUI");
					if (m_pSelectedSlotUI)
						m_pSelectedSlotUI.SetSelected(false);
	
					if (IsUsingGamepad())
					{
						m_pSelectedSlotUI = m_pFocusedSlotUI;
						m_pSelectedSlotUI.SetSelected(true);
						HighlightAvailableStorages(m_pSelectedSlotUI);
					}
	
					NavigationBarUpdate();
					if (IsUsingGamepad())
						SetStorageSwitchMode(true);
				}
				else if (m_pFocusedItemElement)
				{
					DebugPrint("::SimpleFSM - Selected - m_pFocusedItemElement");
					if (m_pSelectedItemElement)
						m_pSelectedItemElement.SetSelected(false);
					
					m_pSelectedItemElement = m_pFocusedItemElement;
					m_pSelectedItemElement.SetSelected(true);
					
					NavigationBarUpdate();
				}
			} 
			break;
			
			case EMenuAction.ACTION_DESELECT:
			{
				DebugPrint("::SimpleFSM - ACTION_DESELECT");
				if (m_pSelectedSlotUI)
				{
					ResetHighlightsOnAvailableStorages();
					m_pSelectedSlotUI.SetSelected(false);
					m_pSelectedSlotUI = null;
				}
				else if (m_pSelectedItemElement)
				{
					m_pSelectedItemElement.SetSelected(false);
					m_pSelectedItemElement = null;
				}

				NavigationBarUpdate();
			}
			break;
			
			case EMenuAction.ACTION_DRAGGED:
			{
				DebugPrint("::SimpleFSM - ACTION_DRAGGED");
				
				m_EStateMenuItem = EStateMenuItem.STATE_MOVING_ITEM_STARTED;
				SCR_UISoundEntity.SoundEvent("SOUND_INV_CONTAINER_DRAG");
			} 
			break;
			
			case EMenuAction.ACTION_DROPPED:
			{
				DebugPrint("::SimpleFSM - ACTION_DROPPED");
				
				Action_Drop();
				
				if (m_pActiveHoveredStorageUI)
					m_pActiveHoveredStorageUI.ShowContainerBorder(false);
				
				m_EStateMenuItem = EStateMenuItem.STATE_IDLE;
				return;
			} 
			break;
			
			case EMenuAction.ACTION_UNFOLD:
			{
				DebugPrint("::SimpleFSM - ACTION_UNFOLD");
				
				if (m_pFocusedSlotUI.GetStorageUI() == m_pStorageListUI) //if it is slot in the "storage list ui"
				{
					if (BaseInventoryStorageComponent.Cast(m_pFocusedSlotUI.GetAsStorage()))	// and if it is a storage
					{
						//ShowStorage( m_pFocusedSlotUI.GetAsStorage() ); 		//show the content of the actualy selected
						ToggleStorageContainerVisibility(m_pFocusedSlotUI);
						m_EStateMenuStorage = EStateMenuStorage.STATE_OPENED;
					}
					else
					{
						//CloseOpenedStorage();	// if it is not storage, show nothing
						//ToggleStorageContainerVisibility( m_pFocusedSlotUI.GetAsStorage() );
						m_EStateMenuStorage = EStateMenuStorage.STATE_IDLE;
					}
				}
				else
				{
					TraverseActualSlot();
					NavigationBarUpdate();
					m_EStateMenuStorage = EStateMenuStorage.STATE_OPENED;
				}
			} 
			break;
			
			case EMenuAction.ACTION_TRADE_SELL:
			{
				DebugPrint("::SimpleFSM - ACTION_TRADE_SELL");
				
				if (m_pFocusedSlotUI)
				{
					DebugPrint("::SimpleFSM - Selected - ACTION_TRADE_SELL - m_pFocusedSlotUI");
					
					m_pSelectedSlotUI = m_pFocusedSlotUI;
					TrySellItem();
					SCR_UISoundEntity.SoundEvent("SOUND_INV_PICKUP_CLICK");
				}
				else if (m_pFocusedItemElement)
				{
					DebugPrint("::SimpleFSM - Selected - ACTION_TRADE_SELL - m_pFocusedItemElement");
					
					m_pFocusedItemElement = m_pSelectedItemElement;
					TrySellItem();
					SCR_UISoundEntity.SoundEvent("SOUND_INV_PICKUP_CLICK");
				}
				
				ResetHighlightsOnAvailableStorages();
			} 
			break;
			
			case EMenuAction.ACTION_TRADE_BUY:
			{
				DebugPrint("::SimpleFSM - ACTION_TRADE_BUY");
				
				if (m_pFocusedSlotUI)
				{
					m_pSelectedSlotUI = m_pFocusedSlotUI;
					
					SCR_UISoundEntity.SoundEvent("SOUND_INV_PICKUP_CLICK");
				}
				ResetHighlightsOnAvailableStorages();
			} 
			break;
			
			case EMenuAction.ACTION_QUANTITY_REMOVE:
			{
				DebugPrint("::SimpleFSM - ACTION_QUANTITY_REMOVE");
				
				if (m_pFocusedItemElement)
				{
					SCR_UISoundEntity.SoundEvent("SOUND_INV_PICKUP_CLICK");
				}
			} 
			break;
			
			case EMenuAction.ACTION_QUANTITY_ADD:
			{
				DebugPrint("::SimpleFSM - ACTION_QUANTITY_ADD");
				
				if (m_pFocusedItemElement)
				{
					SCR_UISoundEntity.SoundEvent("SOUND_INV_PICKUP_CLICK");
				}
			} 
			break;
		}

		if (!IsUsingGamepad())
		{
			m_pSelectedSlotUI = m_pFocusedSlotUI;
			m_pSelectedItemElement = m_pFocusedItemElement;
		}

		HideItemInfo();
		
		DebugPrint("::SimpleFSM - End");
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandleSlottedItemFunction()
	{
		string sAction = "Inventory_Select";

		switch ( m_pFocusedSlotUI.GetSlotedItemFunction() )
		{
			case ESlotFunction.TYPE_GADGET:
			{
				// m_pNavigationBar.SetButtonEnabled( "ButtonEquip", true );
			} break;

			case ESlotFunction.TYPE_WEAPON:
			{
				m_pNavigationBar.SetButtonEnabled( "ButtonEquip", true );
				IEntity item = m_pFocusedSlotUI.GetInventoryItemComponent().GetOwner();
				if (item)
				{
					WeaponComponent weaponComp = WeaponComponent.Cast(item.FindComponent(WeaponComponent));
					if (weaponComp &&
						weaponComp.GetWeaponType() != EWeaponType.WT_FRAGGRENADE &&
						weaponComp.GetWeaponType() != EWeaponType.WT_SMOKEGRENADE)
					{
						m_pNavigationBar.SetButtonEnabled( "ButtonOpenStorage", true );		// look into it
					}
				}
			} break;

			case ESlotFunction.TYPE_MAGAZINE:
			{
				// TODO: show the Reload action
				//m_pNavigationBar.SetButtonEnabled( "ButtonUse", true );
			} break;

			case ESlotFunction.TYPE_CONSUMABLE:
			{
				// TODO: show the Consume action
				m_pNavigationBar.SetButtonEnabled( "ButtonUse", true );
			} break;

			case ESlotFunction.TYPE_STORAGE:
			{
				if( m_EStateMenuItem == EStateMenuItem.STATE_MOVING_ITEM_STARTED && m_pFocusedSlotUI != m_pSelectedSlotUI )
				{
					sAction = "Inventory_Move";
					//m_pNavigationBar.SetButtonEnabled( "ButtonSelect", false );
					//m_pNavigationBar.SetButtonEnabled( "ButtonMove", true );
				}
				// Enable in case the storage is not "togglable" - can be only shown and only opening another storage will close it
				/*else if ( m_EStateMenuStorage == EStateMenuStorage.STATE_OPENED && m_pFocusedSlotUI == m_pSelectedSlotUI && m_pFocusedSlotUI.Type() != SCR_InventorySlotStorageEmbeddedUI)
				{
					m_pNavigationBar.SetButtonEnabled( "ButtonSelect", false );
				}*/
				else if ( m_pFocusedSlotUI.Type() == SCR_InventorySlotStorageEmbeddedUI )
				{
					m_pNavigationBar.SetButtonEnabled( "ButtonOpenStorage", true );
				}
			} break;

			case ESlotFunction.TYPE_HEALTH:
			{
				// TODO: show the Heal action
				m_pNavigationBar.SetButtonEnabled( "ButtonUse", true );
			} break;
		}

		HandleSelectButtonState( sAction );
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void HandleSelectButtonState( string sAction = "Inventory_Select" )
	{
		//TODO: this can be done better

		if ( sAction == "Inventory_Move" )
			m_pNavigationBar.SetButtonActionName( "ButtonSelect", sAction );
		else
		{
			if ( !m_pFocusedSlotUI.IsSlotSelected() )
				m_pNavigationBar.SetButtonActionName( "ButtonSelect", sAction );
			else
				m_pNavigationBar.SetButtonActionName( "ButtonSelect", "Inventory_Deselect" );
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void SetStorageSwitchMode(bool enabled)
	{
		DebugPrint("::SetStorageSwitchMode - Start");
		
		m_bStorageSwitchMode = enabled;
		foreach (SCR_InventoryStorageBaseUI storage : m_aStorages)
		{
			if (storage)
				storage.ActivateStorageButton(m_bStorageSwitchMode);
		}

		//m_pStorageLootUI.ActivateStorageButton(m_bStorageSwitchMode);
		m_pStorageListUI.ActivateStorageButton(m_bStorageSwitchMode);
		//m_pQuickSlotStorage.ActivateStorageButton(m_bStorageSwitchMode);

		if (m_bStorageSwitchMode)
		{
			Widget btnToFocus = m_pStorageListUI.GetButtonWidget();
			if (m_pActiveStorageUI)
				btnToFocus = m_pActiveStorageUI.GetButtonWidget();
			GetGame().GetWorkspace().SetFocusedWidget(btnToFocus);
		}
		else
		{
			if (m_pActiveStorageUI)
			{
				m_pActiveStorageUI.ShowContainerBorder(false);
				FocusOnSlotInStorage(m_pActiveStorageUI);
			}
		}

		NavigationBarUpdate();
		
		DebugPrint("::SetStorageSwitchMode - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void ToggleStorageSwitchMode()
	{
		DebugPrint("::ToggleStorageSwitchMode - Start");
		
		super.ToggleStorageSwitchMode();
		
		DebugPrint("::ToggleStorageSwitchMode - End");
	}
	
	//------------------------------------------------------------------------------------------------
	// ! Default view of the menu inventory
	override void ShowDefault()
	{
		DebugPrint("::ShowDefault - Start");
		
		ShowEquipedWeaponStorage();
		if (!m_pSelectedSlotStorageUI)
		{
			SimpleFSM(EMenuAction.ACTION_BACK);
			return;
		}
		m_pSelectedSlotStorageUI.SetSelected(false);

		if (m_pSelectedSlotUI)
		{
			m_pSelectedSlotUI.SetSelected(false);
			m_pSelectedSlotUI = null;
		}
		if (m_pSelectedSlotStorageUI)
			m_pSelectedSlotStorageUI = null;

		FilterOutStorages(false);
		
		DebugPrint("::ShowDefault - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected override int CloseOpenedStorage()
	{
		DebugPrint("::CloseOpenedStorage - Start");
		
		return super.CloseOpenedStorage();
		
		DebugPrint("::CloseOpenedStorage - End");
	}

	//------------------------------------------------------------------------------------------------
	protected override int CloseStorage( SCR_InventoryStorageBaseUI pStorageUI )
	{
		DebugPrint("::CloseStorage - Start");
		
		return super.CloseStorage(pStorageUI);
		
		DebugPrint("::CloseStorage - End");
	}

	//------------------------------------------------------------------------------------------------
	protected override int CloseStorage( notnull Widget w )
	{
		DebugPrint("::CloseStorage - Start");
		
		return super.CloseStorage(w);
		
		DebugPrint("::CloseStorage - End");
	}

	//------------------------------------------------------------------------------------------------
	// ! Hides/Unhides ( not closes ) the storage
	protected override void ToggleStorageContainerVisibility( notnull SCR_InventorySlotUI pSlot )
	{
		DebugPrint("::ToggleStorageContainerVisibility - Start");
		
		super.ToggleStorageContainerVisibility(pSlot);
		
		DebugPrint("::ToggleStorageContainerVisibility - End");
	}

	//------------------------------------------------------------------------------------------------
	// ! Shows the content of the 1st available storage
	override void ShowStorage()
	{
		DebugPrint("::ShowStorage - Start");
		
		super.ShowStorage();
		
		DebugPrint("::ShowStorage - End");
	}

	//------------------------------------------------------------------------------------------------
	//!
	override void ShowStorage( BaseInventoryStorageComponent storage, ELoadoutArea area = ELoadoutArea.ELA_None )
	{
		DebugPrint("::ShowStorage - Start");
		
		super.ShowStorage(storage, area);
		
		DebugPrint("::ShowStorage - End");
	}

	//------------------------------------------------------------------------------------------------
	override void ShowVicinity()
	{
		DebugPrint("::ShowVicinity - Start");
		
		super.ShowVicinity();
		
		DebugPrint("::ShowVicinity - End");
	}

	//------------------------------------------------------------------------------------------------
	override void RefreshLootUIListener()
	{
		DebugPrint("::RefreshLootUIListener - Start");
		
		super.RefreshLootUIListener();
		
		DebugPrint("::RefreshLootUIListener - End");
	}

	//------------------------------------------------------------------------------------------------
	override void ShowStoragesList()
	{
		DebugPrint("::ShowStoragesList - Start");
		
		super.ShowStoragesList();
		
		DebugPrint("::ShowStoragesList - End");
	}

	//------------------------------------------------------------------------------------------------
	override void ShowAllStoragesInList()
	{
		DebugPrint("::ShowAllStoragesInList - Start");
		
		super.ShowAllStoragesInList();
		
		DebugPrint("::ShowAllStoragesInList - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void SortSlotsByLoadoutArea( out array<SCR_InventorySlotUI> aSlots )
	{
		DebugPrint("::SortSlotsByLoadoutArea - Start");
		
		super.SortSlotsByLoadoutArea(aSlots);
		
		DebugPrint("::SortSlotsByLoadoutArea - End");
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void ShowGadgetStorage()
	{
		DebugPrint("::ShowGadgetStorage - Start");
		
		super.ShowGadgetStorage();
		
		DebugPrint("::ShowGadgetStorage - End");
	}

	//------------------------------------------------------------------------------------------------
	protected override void ShowEquipedWeaponStorage()
	{
		DebugPrint("::ShowEquipedWeaponStorage - Start");
		
		super.ShowEquipedWeaponStorage();
		
		DebugPrint("::ShowEquipedWeaponStorage - End");
	}

	//------------------------------------------------------------------------------------------------
	override void ShowItemInfo( string sName = "", string sDescr = "", float sWeight = 0.0 )
	{
		DebugPrint("::ShowItemInfo - Start");
		
		super.ShowItemInfo(sName, sDescr, sWeight);
		
		DebugPrint("::ShowItemInfo - End");
	}

	//------------------------------------------------------------------------------------------------
	override void HideItemInfo()
	{
		DebugPrint("::HideItemInfo - Start");
		
		super.HideItemInfo();
		
		DebugPrint("::HideItemInfo - End");
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetSlotFocused(SCR_InventorySlotUI pFocusedSlot, SCR_InventoryStorageBaseUI pFromStorageUI, bool bFocused)
	{
		DebugPrint("::SetSlotFocused - Start");
		
		super.SetSlotFocused(pFocusedSlot, pFromStorageUI, bFocused);
		
		DebugPrint("::SetSlotFocused - End");
	}

	//------------------------------------------------------------------------------------------------
	protected override void SetFocusedSlotEffects()
	{
		DebugPrint("::SetFocusedSlotEffects - Start");
		
		super.SetFocusedSlotEffects();
		
		DebugPrint("::SetFocusedSlotEffects - End");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnContainerFocused(SCR_InventoryStorageBaseUI pContainer)
	{
		DebugPrint("::OnContainerFocused - Start");
		
		super.OnContainerFocused(pContainer);
		
		DebugPrint("::OnContainerFocused - End");
	}

	//------------------------------------------------------------------------------------------------
	override void OnContainerFocusLeft(SCR_InventoryStorageBaseUI pContainer)
	{
		DebugPrint("::OnContainerFocusLeft - Start");
		
		super.OnContainerFocusLeft(pContainer);
		
		DebugPrint("::OnContainerFocusLeft - End");
	}
	
	//------------------------------------------------------------------------------------------------
	override void Action_QuickSlotAssign();
	//------------------------------------------------------------------------------------------------
	override void Action_QuickSlotUnassign();
	//------------------------------------------------------------------------------------------------
	protected override void RemoveItemFromQuickSlotDrop();
	//------------------------------------------------------------------------------------------------
	protected override void SetItemToQuickSlot(int iSlotIndex, SCR_InventorySlotUI slot = null);
	//------------------------------------------------------------------------------------------------
	protected override void SetItemToQuickSlotDrop();
	//------------------------------------------------------------------------------------------------
	override void OnItemRemoved(IEntity pItem, BaseInventoryStorageComponent pStorageOwner);
	//------------------------------------------------------------------------------------------------
	override void OnItemAdded(IEntity pItem, BaseInventoryStorageComponent pStorageOwner);
	
	//------------------------------------------------------------------------------------------------
	void EL_TraderMenuUI();
	//------------------------------------------------------------------------------------------------
	void ~EL_TraderMenuUI();
	
	//------------------------------------------------------------------------------------------------
	void DebugPrint(string text)
	{
	#ifdef EL_TRADER_SYSTEM_DEBUG
		Print(ToString() + text);
	#endif
	}
};