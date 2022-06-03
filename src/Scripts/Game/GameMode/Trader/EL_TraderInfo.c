//! Config template for vehicles available for request in Campaign
[BaseContainerProps(), SCR_BaseContainerCustomTitleResourceName("m_Prefab", true)]
class EL_TraderInfo
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, "", "et")]
	protected ResourceName m_Prefab;
	[Attribute("Vehicle", desc: "Display name to be shown in UI.")]
	protected string m_sDisplayName;
	[Attribute("VEHICLE", desc: "Display name to be shown in UI (upper case).")]
	protected string m_sDisplayNameUC;
	[Attribute("", desc: "Description to be shown in UI.")]
	protected string m_sDescription;
	[Attribute("", desc: "Price to be displayed in UI.")]
	protected int m_sPrice;

	ResourceName GetPrefab()
	{
		return m_Prefab;
	}

	string GetDisplayName()
	{
		return m_sDisplayName;
	}

	string GetDisplayNameUpperCase()
	{
		return m_sDisplayNameUC;
	}

	string GetDescription()
	{
		return m_sDescription;
	}

	int GetPrice()
	{
		return m_sPrice;
	}
};

[BaseContainerProps(configRoot: true)]
class EL_TraderInfoList
{
	[Attribute(desc: "Trader item list.")]
	private ref array<ref EL_TraderInfo> m_TraderItemList;

	void GetTraderItemList(out notnull array<ref EL_TraderInfo> traderList)
	{
		traderList = m_TraderItemList;
	}

	void ~EL_TraderInfoList()
	{
		m_TraderItemList = null;
	}
};