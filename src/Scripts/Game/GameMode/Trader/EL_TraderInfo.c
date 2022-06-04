//! Config template for a trader item
[BaseContainerProps(), SCR_BaseContainerCustomTitleResourceName("m_ResourceName", true)]
class EL_TraderItemInfo
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, "", "et")]
	protected ResourceName m_ResourceName;
	[Attribute("", desc: "Price to be displayed in UI.")]
	protected int m_sPrice;

	ResourceName GetResourceName()
	{
		return m_ResourceName;
	}

	int GetPrice()
	{
		return m_sPrice;
	}
};

[BaseContainerProps(configRoot: true)]
class EL_TraderInfoList
{
	[Attribute(desc: "Displayed trader name.")]
	private string m_TraderName;
	[Attribute(desc: "Trader item list of the items this trader will sell to players.")]
	private ref array<ref EL_TraderItemInfo> m_TraderItemSellList;
	[Attribute(desc: "Trader item list of the items this trader will buy from players.")]
	private ref array<ref EL_TraderItemInfo> m_TraderItemBuyList;

	string GetTraderName()
	{
		return m_TraderName;
	}
	
	void GetTraderItemSellList(out notnull array<ref EL_TraderItemInfo> traderItemSellList)
	{
		traderItemSellList = m_TraderItemSellList;
	}
	
	void GetTraderItemBuyList(out notnull array<ref EL_TraderItemInfo> traderItemBuyList)
	{
		traderItemBuyList = m_TraderItemBuyList;
	}

	void ~EL_TraderInfoList()
	{
		m_TraderItemSellList = null;
		m_TraderItemBuyList = null;
	}
};