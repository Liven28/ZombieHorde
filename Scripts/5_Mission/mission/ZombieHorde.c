modded class MissionServer
{
	float activationDistanceRatio = 2;
	float desactivationDistanceRatio = 2.3;
	
	float mediumRadiusRatio = 0.5;
	float hightRadiusRation = 0.2;
	
	static int despawnDistance = 600;
	static int despawnDistanceDeadBodies = 80;
	static int deadBodyDespawnDelay = 30000;
	static int despawnDelayCheckDistance = 5000;
	
	static int zombieCurrentNumber;	
	int numberOfInfectedZones;
	string zombieTypeToSpawn;
	vector zombiePositionToSpawn;
	autoptr TBoolArray hordeSpawned = {false,false,false,};
	float checkStaticHordeDistanceCount;
	int checkStaticHordeDistanceDelay = 1;
	int currentPlayerCheckedStaticHorde;
	float playersUpdateCount;
	int playersUpdateDelay;
	int numHordeToCheck;
	static ref array<Man> players;
	static int playerCount;
	PlayerBase thePlayer;
	vector posPlayer;
	PlayerBase thePlayer2;
	vector posPlayer2; 
	

	autoptr TFloatArray infectedZoneCoordX = {500,5000,10000};
	autoptr TFloatArray infectedZoneCoordZ = {500,5000,10000};
	autoptr TIntArray infectedZoneRadius = {100, 150, 200};	
	
	ref TStringArray infectedTypeLow = {
		"ZmbF_MilkMaidOld_Beige",
		"ZmbF_MilkMaidOld_Black",
		"ZmbF_MilkMaidOld_Green",
		"ZmbF_MilkMaidOld_Grey",

		"ZmbM_FarmerFat_Beige",
		"ZmbM_FarmerFat_Blue",
		"ZmbM_FarmerFat_Brown",
		"ZmbM_FarmerFat_Green",

		"ZmbM_JournalistSkinny",

		"ZmbF_JournalistNormal_Blue",
		"ZmbF_JournalistNormal_Green",
		"ZmbF_JournalistNormal_Red",
		"ZmbF_JournalistNormal_White",

		"ZmbM_JoggerSkinny_Blue",
		"ZmbM_JoggerSkinny_Green",

		"ZmbF_JoggerSkinny_Blue",
		"ZmbF_JoggerSkinny_Brown",
		"ZmbF_JoggerSkinny_Green",
	};
	ref TStringArray infectedTypeMedium = {
		"ZmbM_ParamedicNormal_Blue",
		"ZmbM_ParamedicNormal_Red",

		"ZmbF_ParamedicNormal_Blue",
		"ZmbF_ParamedicNormal_Red",
	};
	ref TStringArray infectedTypeHight = {
		"ZmbM_SoldierHelmet",
		"ZmbM_SoldierVest",
		"ZmbM_SoldierVestHelmet",
	};
		
	autoptr TIntArray infectedHightNumber = { 10, 5 , 30 };
	autoptr TIntArray infectedMediumNumber = { 5, 10, 15 };
	autoptr TIntArray infectedLowNumber = {20, 20, 40};

	
	override void OnInit()
	{
		super.OnInit();
		//JSON();
		numberOfInfectedZones = infectedZoneCoordX.Count();
	}
	
	override void TickScheduler(float timeSplice)
	{
		super.TickScheduler(timeslice);
		playersUpdateCount += timeslice * 10000;
		if (playersUpdateCount >= playersUpdateDelay)
		{
			playersUpdateCount = 0;
			GetGame().GetPlayers(players);
			playerCount = players.Count();	
		}
		
		checkStaticHordeDistanceCount += timeslice * 1000 * playerCount;
		if (checkStaticHordeDistanceCount > checkStaticHordeDistanceDelay)
		{
			checkStaticHordeDistanceCount = 0;				

			currentPlayerCheckedStaticHorde++;
			if (currentPlayerCheckedStaticHorde >= playerCount)
			{
				currentPlayerCheckedStaticHorde = 0; 
				numHordeToCheck++;
				if (numHordeToCheck > numberOfInfectedZones) 
					numHordeToCheck = 1; 
			}
			
			thePlayer = PlayerBase.Cast(players.Get(currentPlayerCheckedStaticHorde));		
				
			if (thePlayer)
			{
				posPlayer = thePlayer.GetPosition();
				CheckInfectedZone(posPlayer);
			}			
		}
	}
		
	void CheckInfectedZone(vector thePosPlayer)
	{
		for (int i = 0; i < numberOfInfectedZones; ++i)
		{
			vector coordCheck = {infectedZoneCoordX[i],0,infectedZoneCoordZ[i]};
			float distX = Math.AbsFloat(coordCheck[0] - thePosPlayer[0]);
			float distZ = Math.AbsFloat(coordCheck[2] - thePosPlayer[2]);
			if (hordeSpawned[i] == false)
			{
				if (distX < infectedZoneRadius[i] * activationDistanceRatio && distZ < infectedZoneRadius[i] * activationDistanceRatio)
				{					
					SpawnGroupe (coordCheck, infectedTypeHight, infectedHightNumber[i], infectedZoneRadius[i], hightRadiusRation);
					SpawnGroupe (coordCheck, infectedTypeMedium, infectedMediumNumber[i], infectedZoneRadius[i], mediumRadiusRatio);
					SpawnGroupe (coordCheck, infectedTypeLow, infectedLowNumber[i], infectedZoneRadius[i], 1);
					hordeSpawned[i] = true;
					RPC_MESSAGE("Static Horde Spawned : " + (infectedHightNumber[i] + infectedMediumNumber[i] + infectedLowNumber[i]).ToString());
					RPC_MESSAGE("zombieCurrentNumber : " + zombieCurrentNumber.ToString());
					break;
				}	
			}
			else if (hordeSpawned[i] == true)
			{								
				bool readyToRespawn = true;
				for (int j = 0; j < playerCount; ++j)
				{
					thePlayer2 = PlayerBase.Cast(players.Get(j));
					if (thePlayer2)
					{
						posPlayer2 = thePlayer2.GetPosition();
						vector coordCheck2 = {infectedZoneCoordX[i],0,infectedZoneCoordZ[i]};
						float distX2 = Math.AbsFloat(coordCheck2[0] - posPlayer2[0]);
						float distZ2 = Math.AbsFloat(coordCheck2[2] - posPlayer2[2]);						
											
						if (distX2 < infectedZoneRadius[i] * desactivationDistanceRatio && distZ2 < infectedZoneRadius[i] * desactivationDistanceRatio)
						{
							readyToRespawn = false;
							break;
						}
					}
				}
				if (readyToRespawn == true)
				{						
					hordeSpawned[i] = false;
					RPC_MESSAGE("Static Horde Ready to Spawn again");
				}				
			}
		}
	}

	void SpawnGroupe(vector theInfectedZoneCoord, TStringArray in_infectedType, int in2_infectedNumber, int in2_infectedZoneRadius, float in_InteriorRadiusRation)
	{
		for (int k = 0; k < in2_infectedNumber; k++)
		{
			zombieTypeToSpawn = ChooseZombiesType(in_infectedType);
			zombiePositionToSpawn = ChoosePositionZombie(theInfectedZoneCoord, in2_infectedZoneRadius * in_InteriorRadiusRation);			
			SpawnZombie(zombieTypeToSpawn, zombiePositionToSpawn);
		}
	}

	

	vector ChoosePositionZombie(vector the_Zone_Position, int the_spawned_Size)
	{
		vector thePosition = the_Zone_Position;
		float decalX = Math.RandomFloatInclusive(-the_spawned_Size, the_spawned_Size);
		float decalZ = Math.RandomFloatInclusive(-the_spawned_Size, the_spawned_Size);
		thePosition[0] = the_Zone_Position[0] + decalX;		
		thePosition[2] = the_Zone_Position[2] + decalZ;
		return thePosition;
	}	


	string ChooseZombiesType(TStringArray in2_infectedType)
	{
		int rand = Math.RandomInt(0, in2_infectedType.Count() - 1)	
		return in2_infectedType[rand];
	}	
	
	
	void SpawnZombie(string theZombieTypeToSpawn, vector theZombiePositionToSpawn)
	{
		EntityAI zombie = GetGame().CreateObject(zombieTypeToSpawn, theZombiePositionToSpawn, false, true);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(PvzRemoveZombie.CheckRemoveZombie, despawnDelayCheckDistance + Math.RandomInt(0, 5000), false, zombie);
		zombieCurrentNumber++;
	}

	static void RPC_MESSAGE(string theMessage)
	{
		//if (showRpcMessages == true)
		{
			Param1<string> Msgparam;
			PlayerBase player;
			PlayerIdentity playerIdentity;	
				
			players = new array<Man>;
			GetGame().GetPlayers(players);
			
			for ( int i = 0; i < players.Count(); ++i )
			{
				player = PlayerBase.Cast(players.Get(i));				
				//playerCheckAdmin = player;
				//CheckAdmin();
				//if (isAdmin == true)
				{
					playerIdentity = player.GetIdentity();
					Msgparam = new Param1<string>( theMessage);
					GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, playerIdentity);
				}
			}
		}
	}

}


class PvzRemoveZombie
{	
	static void CheckRemoveZombie(EntityAI zomb)
	{
		if (zomb != null)
		{
			bool zombRemovable = true;
			bool deadBodyByDelay = false;
			vector posZombie = zomb.GetPosition();

			for (int k = 0; k < MissionServer.playerCount; ++k)
			{
				PlayerBase thePlayer = PlayerBase.Cast(MissionServer.players.Get(k));
				if (thePlayer)
				{
					vector posPlay = thePlayer.GetPosition();					

					float distX = Math.AbsFloat(posZombie[0] - posPlay[0]);
					float distZ = Math.AbsFloat(posZombie[2] - posPlay[2]);
					
					if (zomb.IsAlive() == true)
					{
						if (distX < MissionServer.despawnDistance && distZ < MissionServer.despawnDistance)
						{
							zombRemovable = false;							
						}	
					}
					else
					{
						int checkNum = Math.Min (MissionServer.despawnDistance, MissionServer.despawnDistanceDeadBodies); 
						if (distX < checkNum && distZ < checkNum)
						{
							zombRemovable = false;		
							if (MissionServer.deadBodyDespawnDelay != 0)
							{
								GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteZombieBody, MissionServer.deadBodyDespawnDelay, false, zomb);		
								deadBodyByDelay	= true;							
							}
						}							
					}
				}						
			}

			if (zombRemovable == true)
			{			
				zomb.SetHealth("", "", 0);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteZombieBody, 5000, false, zomb);
			}
			else if (deadBodyByDelay == false)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckRemoveZombie, MissionServer.despawnDelayCheckDistance, false, zomb);
			}
		}
	}

	static void DeleteZombieBody(EntityAI zomb2)
	{
		if (zomb2 != null)
		{
			GetGame().ObjectDelete(zomb2);	
			MissionServer.zombieCurrentNumber--;
		}
	}
}

/*

class EvocatusRadiationZones
{
    static const string m_EvocatusRadiationZonesJSON = "$profile:Sync/config/radiationzones.json";
    
    private ref map<string, float> positions_radius;
    
    // read from json
    void Load()    
    {
        if(FileExist(m_EvocatusRadiationZonesJSON))
        {
            JsonFileLoader<EvocatusRadiationZones>.JsonLoadFile(m_EvocatusRadiationZonesJSON, this);
        }
        else
        {
            Print("JSON config file: " + m_EvocatusRadiationZonesJSON + " not found" );
        }
    }
    
    ref map<string, float> Get()
    {
        return positions_radius;
    }
    
    // check if player is in a zone
    
    // update HUD on enter/exit zone
}

for(int k = 0; k < m_RadiationZonesPositions.Count(); k++)
{
	string _key = m_RadiationZonesPositions.GetKey(k);
	RadiationZone_epicentrum = _key.ToVector();
	RadiationZone_radius = m_RadiationZonesPositions.Get(_key);
	
	if(vector.Distance(player.GetPosition(), RadiationZone_epicentrum) <= RadiationZone_radius)
		IsInRadiationZone = true;
}
_key.ToVector();

*/