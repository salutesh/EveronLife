class EL_TraderMenuUI_ItemElement
{
	protected ResourceName m_ItemPreviewManagerPrefab = "{9F18C476AB860F3B}Prefabs/World/Game/ItemPreviewManager.et";
	
	protected EL_TraderMenuUI m_TraderMenu;
	protected EL_TraderItemInfo m_ItemInfo;
	protected ItemPreviewManagerEntity m_PreviewManager;
	
	protected Widget m_wRoot;
	protected RichTextWidget m_wItemName;
	protected ItemPreviewWidget m_wItemPreview;
	
	//------------------------------------------------------------------------------------------------
	void EL_TraderMenuUI_ItemElement(EL_TraderMenuUI traderMenu, GridLayoutWidget grid, EL_TraderItemInfo itemInfo)
	{
		m_TraderMenu = traderMenu;
		m_ItemInfo = itemInfo;
		m_wRoot = GetGame().GetWorkspace().CreateWidgets("{5D99FC874DCBAF42}UI/Layouts/Menus/Trader/EL_TraderItemElement.layout", grid);
		m_wItemName = RichTextWidget.Cast(m_wRoot.FindAnyWidget("ItemName"));
		m_wItemPreview = ItemPreviewWidget.Cast(m_wRoot.FindAnyWidget("ItemPreview"));
		
		SetElement();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetElement()
	{
		if (!m_ItemInfo) return;
		
		Resource res = Resource.Load(m_ItemInfo.GetResourceName());
		if (!res.IsValid())
			return;
		
		IEntity previewEntity = GetGame().SpawnEntityPrefabLocal(res);
		SetPreviewedItem(previewEntity);
		
		InventoryItemComponent invItemComp = InventoryItemComponent.Cast(previewEntity.FindComponent(InventoryItemComponent));
		if (!invItemComp) 
			return;
		
		UIInfo uiInfoItem = invItemComp.GetAttributes().GetUIInfo();
		if (uiInfoItem) 
			m_wItemName.SetText(uiInfoItem.GetName());
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
}
