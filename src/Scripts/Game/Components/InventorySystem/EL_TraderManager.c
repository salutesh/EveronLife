class EL_TraderManagerComponentClass: GameComponentClass
{
};


class EL_TraderManagerComponent : GameComponent
{
	/*[Attribute(defvalue: "", uiwidget: UIWidgets.ResourceNamePicker, desc: "Item to Receive", category: "Trade" )]
	ResourceName m_ItemToReceive;
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourceNamePicker, desc: "Item to Give", category: "Trade" )]
	ResourceName m_ItemToGive;*/
	[Attribute(defvalue: "{B443D5321B09D546}Configs/Traders/EL_AppleTraderConfig.conf", desc: "Trader configuration file.", params: "conf", category: "Trader Configuration")]
	protected ResourceName m_TraderConfig;
	
	ResourceName GetResourceName()
	{
		return m_TraderConfig;
	}
}