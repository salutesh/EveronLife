//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/EveronLife", description: "Handles client > server communication in Everon Life. Should be attached to PlayerController.", color: "0 0 255 255")]
class EL_NetworkComponentClass: ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Used to identify various notifications for client
enum EL_EClientNotificationID
{
	VEHICLE_SPAWNED,
	COOLDOWN,
	ZONE_BLOCKED
};

class EL_NetworkComponent : ScriptComponent
{
	// Member variables
	protected SCR_PlayerController m_PlayerController;
	protected RplComponent m_RplComponent;

	//------------------------------------------------------------------------------------------------
	static EL_NetworkComponent GetELNetworkComponent(int playerID)
	{
		PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerID);

		if (!playerController)
			return null;

		EL_NetworkComponent networkComponent = EL_NetworkComponent.Cast(playerController.FindComponent(EL_NetworkComponent));

		return networkComponent;
	}

	//------------------------------------------------------------------------------------------------
	void OpenTraderMenu(ResourceName resourceName)
	{
		Rpc(RpcDo_ELOnOpenTraderMenu, resourceName);
	}

	//------------------------------------------------------------------------------------------------
	//! Checks if the session is run as client
	protected bool IsProxy()
	{
		return (m_RplComponent && m_RplComponent.IsProxy());
	}

	//***********//
	//RPC METHODS//
	//***********//
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ELOnOpenTraderMenu(ResourceName resourceName)
	{
		EL_TraderMenuUI traderMenu = EL_TraderMenuUI.Cast(GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.ELTraderMenu, 0, true, true));
		if (!traderMenu)
			return;
		
		traderMenu.SetTraderConfig(resourceName);
	}

	//------------------------------------------------------------------------------------------------
	// Init
	override void EOnInit(IEntity owner)
	{
		m_PlayerController = SCR_PlayerController.Cast(PlayerController.Cast(owner));

		if (!m_PlayerController) {
			Print("EL_NetworkComponent must be attached to PlayerController!", LogLevel.ERROR);
			return;
		}

		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (!m_RplComponent) {
			Print("Could not find RplComponent on PlayerController!", LogLevel.ERROR);
			return;
		}
	}

	//------------------------------------------------------------------------------------------------
	// PostInit
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}

	//------------------------------------------------------------------------------------------------
	// Constructor
	void EL_NetworkComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{

	}

	//------------------------------------------------------------------------------------------------
	// Destructor
	void ~EL_NetworkComponent()
	{

	}
};