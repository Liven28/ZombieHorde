class CfgPatches
{
	class LIVEN_ZombieHorde
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{

		};
	};
};

class CfgMods
{
	class ZombieHorde
	{
		
		dir = "ZombieHorde";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "ZombieHorde";
		credits = "0";
		author = "0";
		authorID = "0"; 
		version = ""; 
		extra = 0;
		type = "mod";
		dependencies[] = {"Game","World","Mission"};
		
		class defs
		{	
			class gameScriptModule
			{
				value = "";
				files[] = {"LIVEN/@ZombieHorde/Scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"LIVEN/@ZombieHorde/Scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"LIVEN/@ZombieHorde/Scripts/5_Mission"};
			};
		};
	};
};
