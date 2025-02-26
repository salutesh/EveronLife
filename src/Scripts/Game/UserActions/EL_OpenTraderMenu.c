class EL_OpenTraderMenu : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		
	}

	//! Method called when the action is interrupted/canceled.
	//! \param pUserEntity The entity that was performing this action prior to interruption
	//------------------------------------------------------------------------------------------------
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{

	}

	//! Method called from scripted interaction handler when an action is started (progress bar appeared)
	//! \param pUserEntity The entity that started performing this action
	//------------------------------------------------------------------------------------------------
	override void OnActionStart(IEntity pUserEntity)
	{

	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return CanBePerformedScript(user);
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (user == GetOwner())
			return false;

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// Find local player controller
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;

		// Find Everon Life network component to send RPC to server
		EL_NetworkComponent networkComponent = EL_NetworkComponent.Cast(playerController.FindComponent(EL_NetworkComponent));
		if (!networkComponent)
			return;
		
		EL_TraderManagerComponent traderManager = EL_TraderManagerComponent.Cast(pOwnerEntity.FindComponent(EL_TraderManagerComponent));
		if (!traderManager)
			return;
		
		networkComponent.OpenTraderMenu(traderManager.GetResourceName());
	}
};
