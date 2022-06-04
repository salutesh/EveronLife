class EL_TraderMenuUI_ItemElement: ScriptedWidgetComponent
{
	protected ResourceName m_ItemPreviewManagerPrefab = "{9F18C476AB860F3B}Prefabs/World/Game/ItemPreviewManager.et";
	protected EL_TraderMenuUI m_TraderMenu;
	protected EL_TraderItemInfo m_ItemInfo;
	protected ItemPreviewManagerEntity m_PreviewManager;
	
	protected Widget m_wRoot;
	protected RichTextWidget m_wItemName;
	protected ItemPreviewWidget m_wItemPreview;
	protected ButtonWidget m_wItemButton;
	protected ButtonWidget m_wAddQuantityButton;
	protected ButtonWidget m_wRemoveQuantityButton;
	protected RichTextWidget m_wQuantityText;
	
	protected UIInfo m_ItemUIInfo;
	protected SCR_InventoryItemInfoUI m_pItemInfo;
	protected const string ITEM_INFO = "{AE8B7B0A97BB0BA8}UI/layouts/Menus/Inventory/InventoryItemInfo.layout";
	protected bool m_bItemInfoVisable = false;
	
	protected SCR_PagingButtonComponent m_QuantityAddButton;
	protected SCR_PagingButtonComponent m_QuantityRemoveButton;
	protected SCR_ButtonBaseComponent m_ItemButton;
	
	protected int m_Quantity = 0;
	protected const int MAX_QUANTITY = 99;
	protected bool m_bSHIFT = false;
	protected const int SHIFT_QUANTITY = 10;
	
	//------------------------------------------------------------------------------------------------
	void EL_TraderMenuUI_ItemElement(EL_TraderMenuUI traderMenu, GridLayoutWidget grid, EL_TraderItemInfo itemInfo)
	{
		m_TraderMenu = traderMenu;
		m_ItemInfo = itemInfo;
		m_wRoot = GetGame().GetWorkspace().CreateWidgets("{EA9D038CEDA914A1}UI/Layouts/Menus/Trader/EL_TraderItemElementNew.layout", grid);
		m_wRoot.AddHandler(this);
		m_wRoot.SetEnabled(true);
	}
	
	//------------------------------------------------------------------------------------------------	
	void ~EL_TraderMenuUI_ItemElement()
	{
		if (m_pItemInfo && m_bItemInfoVisable)
			HideItemInfo();
	}
	
	//------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wItemName = RichTextWidget.Cast(m_wRoot.FindAnyWidget("ItemName"));
		m_wItemPreview = ItemPreviewWidget.Cast(m_wRoot.FindAnyWidget("ItemPreview"));
		
		m_wItemButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Button0"));
		m_ItemButton = SCR_ButtonBaseComponent.Cast(m_wItemButton.FindHandler(SCR_ButtonBaseComponent));
		if (!m_ItemButton)
			return;
		
		m_ItemButton.m_OnClicked.Insert(OnItemButtonClicked);

		m_wQuantityText = RichTextWidget.Cast(m_wRoot.FindAnyWidget("PagesText"));
		m_wAddQuantityButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("ButtonAdd")); 
		m_QuantityAddButton = SCR_PagingButtonComponent.Cast(m_wAddQuantityButton.FindHandler(SCR_PagingButtonComponent));
		if (!m_QuantityAddButton)
			return;
		
		m_QuantityAddButton.m_OnClicked.Insert(OnAddQuantityButtonClicked);
		
		m_wRemoveQuantityButton = ButtonWidget.Cast(m_wRoot.FindAnyWidget("ButtonRemove")); 
		m_QuantityRemoveButton = SCR_PagingButtonComponent.Cast(m_wRemoveQuantityButton.FindHandler(SCR_PagingButtonComponent));
		if (!m_QuantityRemoveButton)
			return;
		
		m_QuantityRemoveButton.m_OnClicked.Insert(OnRemoveQuantityButtonClicked);

		SetElement();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetElement()
	{
		if (!m_ItemInfo) 
			return;
		
		Resource res = Resource.Load(m_ItemInfo.GetResourceName());
		if (!res.IsValid())
			return;
		
		IEntity previewEntity = GetGame().SpawnEntityPrefabLocal(res);
		SetPreviewedItem(previewEntity);
		
		InventoryItemComponent invItemComp = InventoryItemComponent.Cast(previewEntity.FindComponent(InventoryItemComponent));
		if (!invItemComp) 
			return;
		
		m_ItemUIInfo = invItemComp.GetAttributes().GetUIInfo();
		if (!m_ItemUIInfo)
			return;
		
		m_wItemName.SetText(m_ItemUIInfo.GetName());
		m_wQuantityText.SetText(string.Format("%1/%2", m_Quantity, MAX_QUANTITY));
	}
	
	//------------------------------------------------------------------------------------------------
	Widget GetRootWidget()
	{
		return m_wRoot;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPreviewedItem(notnull IEntity previewItem)
	{
		m_PreviewManager = GetGame().GetItemPreviewManager();

		if (!m_PreviewManager)
		{
			Resource res = Resource.Load(m_ItemPreviewManagerPrefab);
			if (res.IsValid())
				GetGame().SpawnEntityPrefabLocal(res);
			
			m_PreviewManager = GetGame().GetItemPreviewManager();
			if (!m_PreviewManager)
				return;
		}
		
		if (previewItem && m_wItemPreview)
		{
			m_PreviewManager.SetPreviewItem(m_wItemPreview, previewItem);
			previewItem.ClearFlags(EntityFlags.ACTIVE | EntityFlags.TRACEABLE, true);
			previewItem.SetFlags(EntityFlags.NO_LINK, true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ShowItemInfo(string sName = "", string sDescr = "", float sWeight = 0.0)
	{
		if (!m_pItemInfo)
		{
			Widget infoWidget = GetGame().GetWorkspace().CreateWidgets(ITEM_INFO, null);
			if (!infoWidget)
				return;

			infoWidget.AddHandler(new SCR_InventoryItemInfoUI());
			m_pItemInfo = SCR_InventoryItemInfoUI.Cast(infoWidget.FindHandler(SCR_InventoryItemInfoUI));
		}

		if(!m_pItemInfo)
			return;

		Widget w = WidgetManager.GetWidgetUnderCursor();
		if (!w)
		{
			w = m_wRoot;
		}

		m_pItemInfo.Show(0.6, w, false);
		m_pItemInfo.SetName(sName);
		m_pItemInfo.SetDescription(sDescr);
		m_pItemInfo.SetWeight(sWeight);
		int iMouseX, iMouseY;

		float x, y;
		w.GetScreenPos(x, y);

		float width, height;
		w.GetScreenSize(width, height);

		float screenSizeX, screenSizeY;
		GetGame().GetWorkspace().GetScreenSize(screenSizeX, screenSizeY);

		float infoWidth, infoHeight;
		m_pItemInfo.GetInfoWidget().GetScreenSize(infoWidth, infoHeight);

		iMouseX = x;
		iMouseY = y + height;
		if (x + infoWidth > screenSizeX)
			iMouseX = screenSizeX - infoWidth - width / 2; // offset info if it would go outside of the screen

		m_pItemInfo.Move(GetGame().GetWorkspace().DPIUnscale(iMouseX), GetGame().GetWorkspace().DPIUnscale(iMouseY));
		m_bItemInfoVisable = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void HideItemInfo()
	{
		if (!m_pItemInfo)
			return;
		
		m_pItemInfo.Hide();
		m_bItemInfoVisable = false;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnItemButtonClicked(SCR_ButtonBaseComponent buttonComponent)
	{
		m_TraderMenu.SetSelectedItem(m_ItemInfo);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnAddQuantityButtonClicked()
	{
		if (m_Quantity < MAX_QUANTITY && !m_bSHIFT)
		{
			m_Quantity++;
		}
		else if (m_Quantity + SHIFT_QUANTITY < MAX_QUANTITY && m_bSHIFT)
		{
			m_Quantity += 10;
		}
		
		m_wQuantityText.SetText(string.Format("%1/%2", m_Quantity, MAX_QUANTITY));
	}
	
	//------------------------------------------------------------------------------------------------
	void OnRemoveQuantityButtonClicked()
	{
		if (m_Quantity > 0 && !m_bSHIFT)
		{
			m_Quantity--;
		}
		else if (m_Quantity - SHIFT_QUANTITY < MAX_QUANTITY && m_bSHIFT)
		{
			m_Quantity -= 10;
		}
		
		m_wQuantityText.SetText(string.Format("%1/%2", m_Quantity, MAX_QUANTITY));
	}

	//------------------------------------------------------------------------------------------------
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		super.OnMouseEnter(w, x, y);
		
		if (w == m_wItemButton && m_ItemUIInfo)
			ShowItemInfo(m_ItemUIInfo.GetName(), m_ItemUIInfo.GetDescription());
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w, enterW, x, y);
		
		if (w == m_wItemButton && m_pItemInfo)
			HideItemInfo();
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnKeyPress(Widget w, int x, int y, int key) 
	{
		super.OnKeyPress(w, x, y, key);
		
		Print(ToString() + "::OnKeyPress - Key: " + key);
		
		if (key == KeyCode.KC_LSHIFT)
			m_bSHIFT = true;	
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnKeyUp(Widget w, int x, int y, int key) 
	{
		super.OnKeyUp(w, x, y, key);
		
		Print(ToString() + "::OnKeyUp - Key: " + key);
		
		if (key == KeyCode.KC_LSHIFT)
			m_bSHIFT = false;	
		
		return true;
	}
};
