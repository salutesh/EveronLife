class EL_TraderMenuUI_ItemElement: ScriptedWidgetComponent
{
	protected ResourceName m_ItemPreviewManagerPrefab = "{9F18C476AB860F3B}Prefabs/World/Game/ItemPreviewManager.et";
	protected EL_TraderMenuUI m_TraderMenu;
	protected EL_TraderItemInfo m_ItemInfo;
	protected ItemPreviewManagerEntity m_PreviewManager;
	protected ResourceName m_ItemResourceName;
	protected InventoryItemComponent m_pItemComp;
	
	protected Widget m_wRoot;
	protected ButtonWidget m_wElementButton;
	protected RichTextWidget m_wItemName;
	protected RenderTargetWidget m_wPreviewImage;
	protected ItemPreviewWidget m_wItemPreview;
	protected ButtonWidget m_wAddQuantityButton;
	protected ButtonWidget m_wRemoveQuantityButton;
	protected RichTextWidget m_wQuantityText;
	protected Widget m_wElementBorder;
	
	protected UIInfo m_ItemUIInfo;
	protected SCR_InventoryItemInfoUI m_pItemInfo;
	protected const string ITEM_INFO = "{AE8B7B0A97BB0BA8}UI/layouts/Menus/Inventory/InventoryItemInfo.layout";
	protected bool m_bItemInfoVisable = false;
	
	protected SCR_PagingButtonComponent m_QuantityAddButton;
	protected SCR_PagingButtonComponent m_QuantityRemoveButton;
	protected EL_TraderMenuUI_ItemElementComponent m_ItemElementComponent;
	
	protected int m_Quantity = 0;
	protected int m_QuantityMax = 99;
	protected bool m_bSHIFT = false;
	protected const int SHIFT_QUANTITY = 10;
	protected bool m_bSelected = false;
	
	//------------------------------------------------------------------------------------------------
	//! MEMBER METHODS
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	void EL_TraderMenuUI_ItemElement(EL_TraderMenuUI traderMenu, VerticalLayoutWidget list, EL_TraderItemInfo itemInfo)
	{
		DebugPrint("::EL_TraderMenuUI_ItemElement - Start");
		
		m_TraderMenu = traderMenu;
		m_ItemInfo = itemInfo;
		
		m_wRoot = GetGame().GetWorkspace().CreateWidgets("{EA9D038CEDA914A1}UI/Layouts/Menus/Trader/EL_TraderItemElementNew.layout", list);
		m_wRoot.AddHandler(this);
		
		DebugPrint("::EL_TraderMenuUI_ItemElement - End");
	}
	
	//------------------------------------------------------------------------------------------------
	void ~EL_TraderMenuUI_ItemElement()
	{
		if (m_pItemInfo && m_bItemInfoVisable)
			HideItemInfo();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SetElement()
	{
		DebugPrint("::SetElement - Start");
		
		if (!m_ItemInfo) 
			return;
		
		Resource res = Resource.Load(m_ItemInfo.GetResourceName());
		if (!res.IsValid())
			return;
		
		IEntity previewEntity = GetGame().SpawnEntityPrefabLocal(res);
		SetPreviewedItem(previewEntity);
		
		m_pItemComp = InventoryItemComponent.Cast(previewEntity.FindComponent(InventoryItemComponent));
		if (!m_pItemComp)
		{
			Print(ToString() + "SetElement - ERROR - Can't get InventoryItemComponent from preview entity!", LogLevel.ERROR);
			return;
		}
		
		m_ItemUIInfo = m_pItemComp.GetAttributes().GetUIInfo();
		if (!m_ItemUIInfo)
		{
			Print(ToString() + "SetElement - ERROR - Can't get UIInfo from preview entity InventoryItemComponent!", LogLevel.ERROR);
			return;
		}
		
		m_wItemName.SetText(m_ItemUIInfo.GetName());
		m_wQuantityText.SetText(m_Quantity.ToString() + "/" + m_QuantityMax.ToString());
		
		DebugPrint("::SetElement - End");
	}
	
	//------------------------------------------------------------------------------------------------
	Widget GetRootWidget()
	{
		return m_wRoot;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SetPreviewedItem(notnull IEntity previewItem)
	{
		m_PreviewManager = GetGame().GetItemPreviewManager();
		m_ItemResourceName = previewItem.GetPrefabData().GetPrefabName();
		
		if (!m_PreviewManager)
		{
			Resource res = Resource.Load(m_ItemPreviewManagerPrefab);
			if (!res.IsValid())
				return;
			
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
			m_wItemPreview.SetResolutionScale(1, 1);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ShowItemInfo(string sName = "", string sDescr = "", float sWeight = 0.0)
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
	protected void HideItemInfo()
	{
		if (!m_pItemInfo)
			return;
		
		m_pItemInfo.Hide();
		m_bItemInfoVisable = false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnItemButtonFocus()
	{
		m_TraderMenu.SetFocusedItemElement(this);
		m_wElementBorder.SetVisible(true);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnItemButtonFocusLost()
	{
		m_TraderMenu.SetFocusedItemElement(null);
		m_wElementBorder.SetVisible(false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnAddQuantityButtonClicked()
	{
		if (m_Quantity < m_QuantityMax && !m_bSHIFT)
		{
			m_Quantity++;
		}
		else if (m_Quantity + SHIFT_QUANTITY < m_QuantityMax && m_bSHIFT)
		{
			m_Quantity += 10;
		}
		
		m_wQuantityText.SetText(m_Quantity.ToString() + "/" + m_QuantityMax.ToString());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnRemoveQuantityButtonClicked()
	{
		if (m_Quantity > 0 && !m_bSHIFT)
		{
			m_Quantity--;
		}
		else if (m_Quantity - SHIFT_QUANTITY < m_QuantityMax && m_bSHIFT)
		{
			m_Quantity -= 10;
		}
		
		m_wQuantityText.SetText(m_Quantity.ToString() + "/" + m_QuantityMax.ToString());
	}
	
	//------------------------------------------------------------------------------------------------
	void SetQuantityMax(int max)
	{
		m_QuantityMax = max;
		m_wQuantityText.SetText(m_Quantity.ToString() + "/" + m_QuantityMax.ToString());
	}
	
	//------------------------------------------------------------------------------------------------
	void SetQuantity(int quantity)
	{
		m_Quantity = quantity;
		
		m_wQuantityText.SetText(m_Quantity.ToString() + "/" + m_QuantityMax.ToString());
	}
	
	//------------------------------------------------------------------------------------------------
	int GetQuantity()
	{
		return m_Quantity;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetElementSize(float x, float y)
	{
		ImageWidget imageBackground = ImageWidget.Cast(m_wRoot.FindAnyWidget("Background"));
		
	}
	
	//------------------------------------------------------------------------------------------------
	void SetVisible(bool state)
	{
		m_wRoot.SetVisible(state);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetSelected(bool state)
	{
		m_bSelected = state;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsElementSelected() 
	{ 
		return m_bSelected;
	}
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetItemResourceName()
	{
		return m_ItemResourceName;
	}
	
	//------------------------------------------------------------------------------------------------
	EL_TraderMenuUI_ItemElementComponent GetItemElementComponent()
	{
		return m_ItemElementComponent;
	}
	
	//------------------------------------------------------------------------------------------------	
	InventoryItemComponent GetInventoryItemComponent()
	{
		return m_pItemComp;
	}
	
	//------------------------------------------------------------------------------------------------
	//! BASE CLASS OVERRIDES
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------
	protected override void HandlerAttached(Widget w)
	{
		DebugPrint("::HandlerAttached - Start");
		
		m_wElementButton = ButtonWidget.Cast(m_wRoot);
		m_wItemName = RichTextWidget.Cast(m_wRoot.FindAnyWidget("ItemName"));
		m_wPreviewImage = ItemPreviewWidget.Cast(m_wRoot.FindAnyWidget("ItemPreview"));
		m_wItemPreview = ItemPreviewWidget.Cast(m_wPreviewImage);
		m_wElementBorder = m_wRoot.FindAnyWidget("ElementBorder");
		
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

		m_ItemElementComponent = EL_TraderMenuUI_ItemElementComponent.Cast(m_wRoot.FindHandler(EL_TraderMenuUI_ItemElementComponent));
		if (!m_ItemElementComponent)
			return;
		
		m_ItemElementComponent.SetTraderMenu(m_TraderMenu);
		
		SetElement();
		
		DebugPrint("::HandlerAttached - End");
	}	
	
	//------------------------------------------------------------------------------------------------
	protected override bool OnMouseEnter(Widget w, int x, int y)
	{
		DebugPrint("::OnMouseEnter - Start");
		DebugPrint("::OnMouseEnter - Widget: " + w.GetName());
		
		super.OnMouseEnter(w, x, y);
		
		if (w == m_wRoot)
		{
			m_wElementButton.SetColor(Color.FromRGBA(194,99,20,150));
			OnItemButtonFocus();
			if (m_ItemUIInfo)
				ShowItemInfo(m_ItemUIInfo.GetName(), m_ItemUIInfo.GetDescription());
		}
		
		DebugPrint("::OnMouseEnter - End");
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		DebugPrint("::OnMouseLeave - Start");
		DebugPrint("::OnMouseLeave - Widget: " + w.GetName());
		
		super.OnMouseLeave(w, enterW, x, y);
		
		if (w == m_wRoot)
		{
			m_wElementButton.SetColor(Color.FromRGBA(0,0,0,150));
			OnItemButtonFocusLost();
			if (m_pItemInfo)
				HideItemInfo();
		}
		
		DebugPrint("::OnMouseLeave - End");
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	/*override bool OnFocus(Widget w, int x, int y)
	{
		DebugPrint("::OnFocus - Start");
		DebugPrint("::OnFocus - Widget: " + w.GetName());
		
		if (w == m_wRoot)
			OnItemButtonFocus();
		
		DebugPrint("::OnFocus - End");
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnFocusLost(Widget w, int x, int y)
	{
		DebugPrint("::OnFocusLost - Start");
		DebugPrint("::OnFocusLost - Widget: " + w.GetName());		
		
		if (w == m_wRoot)
			OnItemButtonFocusLost();	
			
		DebugPrint("::OnFocusLost - End");
		
		return false;
	}*/
	
	//------------------------------------------------------------------------------------------------
	void DebugPrint(string text)
	{
	#ifdef EL_TRADER_SYSTEM_DEBUG
		Print(ToString() + text);
	#endif
	}
};
