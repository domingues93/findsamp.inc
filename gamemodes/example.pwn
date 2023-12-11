#include <a_samp>
#include <dof2>

#define FINDSAMP_SERVER_IP      "149.56.252.173"
#define FINDSAMP_SERVER_PORT    7777
#include "../findsamp"

main()return;

public OnGameModeInit()
{
    return 1;
}

public OnGameModeExit()
{
    DOF2_Exit();
    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    if(!strcmp(cmdtext, "/recompensa", true))
    {
        if(GetPlayeRewardTime(playerid) > gettime()){
            SendClientMessage(playerid, -1, "Você já resgatou sua recompensa hoje, volte amanhã.");
            return 1;
        }

        if( checkPlayerVoted(playerid) ) {
            SendClientMessage(playerid, -1, "FS: checando...");
        }
        return 1;
    }
    return 0;
}

public OnResponsePlayerVoted(playerid, bool:voted)
{
    if(voted)
    {
        if(GetPlayeRewardTime(playerid) < gettime())
        {
            SetPlayeRewardTime(playerid, 86400); // bloquear por 86400 segundos ( 24hrs )

            GivePlayerMoney(playerid, 100000);
            SendClientMessage(playerid, 0x1E90FFFF, "FINDSAMP: {FFFFFF}Você recebeu {006400}R$100.000{FFFFFF} por votar em nosso servidor.");
        }
        return 1;
    }
    SendClientMessage(playerid, -1, "Você você ainda não voltou em nosso servidor :(.");
    return 0;
}

stock GetPlayeRewardTime(playerid)
{
    if(!IsPlayerConnected(playerid))return 0;

    new playerFilePath[128], nickname[MAX_PLAYER_NAME];
    
    GetPlayerName(playerid, nickname, sizeof(nickname));

    format(playerFilePath, sizeof(playerFilePath), "players/%s.ini", nickname);
    
    if(!DOF2_FileExists(playerFilePath)) {
        DOF2::CreateFile(playerFilePath);
        DOF2::SetInt(playerFilePath, "Findsamp_RewardTime", 0);
        DOF2::SaveFile();
    }

    return DOF2_GetInt(playerFilePath, "Findsamp_RewardTime");
}

stock SetPlayeRewardTime(playerid, NextRewardTime)
{
    if(!IsPlayerConnected(playerid))return 0;

    new playerFilePath[128], nickname[MAX_PLAYER_NAME];
    
    GetPlayerName(playerid, nickname, sizeof(nickname));

    format(playerFilePath, sizeof(playerFilePath), "players/%s.ini", nickname);

    if(!DOF2_FileExists(playerFilePath)) {
        DOF2_CreateFile(playerFilePath);
    }

    DOF2_SetInt(playerFilePath, "Findsamp_RewardTime", (NextRewardTime + gettime()));
    DOF2_SaveFile();
    return 1;
}