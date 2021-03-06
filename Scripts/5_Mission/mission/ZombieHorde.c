modded class MissionServer
{
	int spawnDistance = 100;
	float reactivationDistanceRatio = 1.5;
	float despawnDistanceRatio = 1.5;
	
	float mediumRadiusRatio = 0.5;
	float hightRadiusRation = 0.2;
	
	static int despawnDistance;
	static int despawnDelayCheckDistanceZombToPlayer = 10000;
	int checkStaticHordeDistanceDelay = 100;

	static int zombieCurrentNumber;	
	int numberOfInfectedZones;
	string zombieTypeToSpawn;
	vector zombiePositionToSpawn;
	float checkStaticHordeDistanceCount;
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
	
	autoptr TBoolArray hordeSpawned = {};
	

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
			vector coordCheck = {infectedZoneCoordX[i],infectedZoneCoordY[i],infectedZoneCoordZ[i]};
			float distX = Math.AbsFloat(coordCheck[0] - thePosPlayer[0]);
			float distZ = Math.AbsFloat(coordCheck[2] - thePosPlayer[2]);
			
			if (hordeSpawned[i] == false)
			{
				//RPC_MESSAGE("distX : " + distX.ToString());
				//RPC_MESSAGE("distZ : " + distZ.ToString());
				//RPC_MESSAGE("Distance : " + (infectedZoneRadius[i] + spawnDistance).ToString());
				//RPC_MESSAGE("infectedZoneRadius : " + (infectedZoneRadius[i]).ToString());
				//RPC_MESSAGE("spawnDistance : " + (spawnDistance).ToString());

				if (distX < infectedZoneRadius[i] + spawnDistance && distZ < infectedZoneRadius[i] + spawnDistance)
				{					
					SpawnGroupe (coordCheck, infectedTypeHight, infectedHightNumber[i], infectedZoneRadius[i], hightRadiusRation);
					SpawnGroupe (coordCheck, infectedTypeMedium, infectedMediumNumber[i], infectedZoneRadius[i], mediumRadiusRatio);
					SpawnGroupe (coordCheck, infectedTypeLow, infectedLowNumber[i], infectedZoneRadius[i], 1);
					hordeSpawned[i] = true;
					RPC_MESSAGE("Static Horde Spawned : " + (infectedHightNumber[i] + infectedMediumNumber[i] + infectedLowNumber[i]).ToString() + " (num" + i.ToString() + ")");
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
											
						if (distX2 < infectedZoneRadius[i] + (spawnDistance * reactivationDistanceRatio) && distZ2 < infectedZoneRadius[i] + (spawnDistance * reactivationDistanceRatio))
						{
							readyToRespawn = false;
							break;
						}
					}
				}
				if (readyToRespawn == true)
				{						
					hordeSpawned[i] = false;
					RPC_MESSAGE("Static Horde Ready to Spawn again" + " (num" + i.ToString() + ")");
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
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(PvzRemoveZombie.CheckRemoveZombie, despawnDelayCheckDistanceZombToPlayer + Math.RandomInt(0, 5000), false, zombie);
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

	autoptr TVectorArray infectedZoneCoord;

	autoptr TFloatArray infectedZoneCoordX;
	autoptr TFloatArray infectedZoneCoordY;
	autoptr TFloatArray infectedZoneCoordZ;
	autoptr TIntArray infectedZoneRadius;	
	
	ref TStringArray infectedTypeLow;
	ref TStringArray infectedTypeMedium;
	ref TStringArray infectedTypeHight;
	
	autoptr TIntArray infectedHightNumber;
	autoptr TIntArray infectedMediumNumber;
	autoptr TIntArray infectedLowNumber;
	
	ref HordeGetJsonData hordeGetJsonData;

	override void OnInit()
	{
		super.OnInit();
		//Print("TestLiven infectedZoneCoordX");		
		hordeGetJsonData = new HordeGetJsonData();	
		hordeGetJsonData.Load("$profile:/ZombieHordeProfile/HordeCoordinatesX.json");
		infectedZoneCoordX = hordeGetJsonData.GetDataFloat();
		delete hordeGetJsonData;
		
		//Print("TestLiven infectedZoneCoodY");
		hordeGetJsonData = new HordeGetJsonData();	
		hordeGetJsonData.Load("$profile:/ZombieHordeProfile/HordeCoordinatesY.json");
		infectedZoneCoordY = hordeGetJsonData.GetDataFloat();
		delete hordeGetJsonData;
		
		//Print("TestLiven infectedZoneCoordZ");
		hordeGetJsonData = new HordeGetJsonData();	
		hordeGetJsonData.Load("$profile:/ZombieHordeProfile/HordeCoordinatesZ.json");
		infectedZoneCoordZ = hordeGetJsonData.GetDataFloat();
		delete hordeGetJsonData;
		
		//Print("TestLiven infectedZoneRadius");
		hordeGetJsonData = new HordeGetJsonData();
		hordeGetJsonData.Load("$profile:/ZombieHordeProfile/HordeSize.json");
		infectedZoneRadius = hordeGetJsonData.GetDataInt();
		delete hordeGetJsonData;
		
		int maxDist = 0;
		for (int i = 0; i < infectedZoneRadius.Count(); i++)
		{
			if (infectedZoneRadius[i] > maxDist)
				maxDist = infectedZoneRadius[i];
		}
		
		despawnDistance = (maxDist + spawnDistance) * despawnDistanceRatio;
		//Print("TestLiven despawnDistance : " + despawnDistance.ToString());

		//Print("TestLiven infectedHightNumber");
		hordeGetJsonData = new HordeGetJsonData();
		hordeGetJsonData.Load("$profile:/ZombieHordeProfile/ZombiesNumberHight.json");
		infectedHightNumber = hordeGetJsonData.GetDataInt();
		delete hordeGetJsonData;
		
		//Print("TestLiven infectedMediumNumber");
		hordeGetJsonData = new HordeGetJsonData();
		hordeGetJsonData.Load("$profile:/ZombieHordeProfile/ZombiesNumberMedium.json");
		infectedMediumNumber = hordeGetJsonData.GetDataInt();
		delete hordeGetJsonData;
		
		//Print("TestLiven infectedLowNumber");
		hordeGetJsonData = new HordeGetJsonData();
		hordeGetJsonData.Load("$profile:/ZombieHordeProfile/ZombiesNumberLow.json");
		infectedLowNumber = hordeGetJsonData.GetDataInt();
		delete hordeGetJsonData;
		
		//Print("TestLiven infectedTypeHight");
		hordeGetJsonData = new HordeGetJsonData();
		hordeGetJsonData.Load("$profile:/ZombieHordeProfile/ZombieTypeHight.json");
		infectedTypeHight = hordeGetJsonData.GetDataString();
		delete hordeGetJsonData;
		
		//Print("TestLiven infectedTypeMedium");
		hordeGetJsonData = new HordeGetJsonData();
		hordeGetJsonData.Load("$profile:/ZombieHordeProfile/ZombieTypeMedium.json");
		infectedTypeMedium = hordeGetJsonData.GetDataString();
		delete hordeGetJsonData;
		
		//Print("TestLiven infectedTypeLow");
		hordeGetJsonData = new HordeGetJsonData();
		hordeGetJsonData.Load("$profile:/ZombieHordeProfile/ZombieTypeLow.json");
		infectedTypeLow = hordeGetJsonData.GetDataString();			
		delete hordeGetJsonData;
		
		hordeSpawned = {};
		for (int j = 0; j < infectedZoneCoordX.Count(); j++)
		{
			hordeSpawned.Insert(false);
		}
		
		numberOfInfectedZones = infectedZoneCoordX.Count();
	}
	
}



class HordeGetJsonData
{
	private ref map<string, int> dataJson;
	private static autoptr TIntArray m_messages_int = {};
	private static autoptr TFloatArray m_messages_float = {};
	private static autoptr TStringArray m_messages_string = {};
	bool init;

	void Load(string fileToLoad)
	{
		if (FileExist(fileToLoad))
		{
			JsonFileLoader<HordeGetJsonData>.JsonLoadFile(fileToLoad, this);
			
			m_messages_int = {};
			m_messages_float = {};
			m_messages_string = {};
			for (int i = 0; i < dataJson.Count(); i++)
			{
				m_messages_int.Insert(0);
				m_messages_float.Insert(0);
				m_messages_string.Insert("");
			}
			
			
			init = false;
		}
		else {Print("TestLiven JSON config file: " + fileToLoad + " not found");}
	}
	
	TFloatArray GetDataFloat()
	{		
		if (dataJson.Count() > 0 && init == false)
		{			
			init = true;
			for (int i = 0; i < dataJson.Count(); i++)
			{
				string _key = dataJson.GetKey(i);
				m_messages_float[i] = dataJson.Get(_key);
				//Print("TestLiven : " + m_messages_float[i].ToString());
			}
		}
		return m_messages_float;
	}
		
	TIntArray GetDataInt()
	{		
		if (dataJson.Count() > 0 && init == false)
		{			
			init = true;
			for (int i = 0; i < dataJson.Count(); i++)
			{
				string _key = dataJson.GetKey(i);
				m_messages_int[i] = dataJson.Get(_key);
				//Print("TestLiven : " + m_messages_int[i].ToString());
			}
		}
		return m_messages_int;
	}
	
	TStringArray GetDataString()
	{		
		if (dataJson.Count() > 0 && init == false)
		{			
			init = true;
			for (int i = 0; i < dataJson.Count(); i++)
			{		
				m_messages_string[i] = dataJson.GetKey(i);
				//Print("TestLiven : " + m_messages_string[i]);
			}
		}
		return m_messages_string;
	}
}



class PvzRemoveZombie
{	
	static void CheckRemoveZombie(EntityAI zomb)
	{
		if (zomb != null)
		{
			bool killZombie = true;
			bool deadBodyByDelay = false;
			vector posZombie = zomb.GetPosition();
			bool updateDone = false;

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
							killZombie = false;							
						}	
					}
					else
					{
						killZombie = false;							
						GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateZombieCurrentNumber, 120000, false, zomb);
						updateDone = true;
					}
				}						
			}

			if (killZombie == true)
			{			
				GetGame().ObjectDelete(zomb); 
				UpdateZombieCurrentNumber();				
				//zomb.SetHealth("", "", 0);
				//GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateZombieCurrentNumber, 15000, false, zomb);
			}
			else if (updateDone == false)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckRemoveZombie, MissionServer.despawnDelayCheckDistanceZombToPlayer, false, zomb);
			}
		}
	}

	static void UpdateZombieCurrentNumber()
	{
		MissionServer.zombieCurrentNumber--;
	}
}

