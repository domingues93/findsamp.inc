#if defined _inc_findsamp
	#undef _inc_findsamp
#endif

#if defined _findsamp_included
	#endinput
#endif
#define _findsamp_included

#tryinclude <requests>

#if !defined _requests_included
    #error Voce precisa incluir e configurar o plugin requests em seu server.cfg. Acesse https://github.com/Southclaws/pawn-requests
#endif

#if !defined FINDSAMP_SERVER_IP
    #error Voce precisa definir FINDSAMP_SERVER_IP antes de incluir o findsamp.inc
#endif

#if !defined FINDSAMP_SERVER_PORT
    #error Voce precisa definir FINDSAMP_SERVER_PORT antes de incluir o findsamp.inc
#endif


/**     DEFINIÇÕES          */
#define FS_MAX_REQUESTS         1000


/**    VARIÁVEIS            */
static PLAYERID[FS_MAX_REQUESTS], REQUEST_TIME[MAX_PLAYERS];


/**    FORWARDS            */
forward OnResponsePlayerVoted(playerid, bool:voted);


/**    FUNÇÕES            */
stock checkPlayerVoted(playerid)
{
    if(!IsPlayerConnected(playerid) || REQUEST_TIME[playerid] > gettime() )return 0;

    REQUEST_TIME[playerid] = gettime() + 10;

    new RequestsClient:client = RequestsClient("https://servers.portalsamp.com");
    PLAYERID[_:client] = playerid;

    new path[128], nickname[MAX_PLAYER_NAME];

    GetPlayerName(playerid, nickname, sizeof(nickname));

    format(path, sizeof(path), "api/voted/%s:%d?nickname=%s", FINDSAMP_SERVER_IP, FINDSAMP_SERVER_PORT, nickname);
    
    Request(client, path, HTTP_METHOD_GET, "OnFindsampResponse", .headers = RequestHeaders());
    return 1;
}

static stock registerLog(const data[])
{
    new fileName[30];
    format(fileName, sizeof(fileName), "findsamp_%d.html", gettime());
    new File:file = fopen(fileName, io_readwrite);
    fwrite(file, data);
    fclose(file);
    return 1;
}

/**    CALLBACKS            */
forward OnFindsampResponse(Request:id, E_HTTP_STATUS:status, data[], dataLen);
public OnFindsampResponse(Request:id, E_HTTP_STATUS:status, data[], dataLen)
{
    
    new playerid = PLAYERID[_:id];
    PLAYERID[_:id] = INVALID_PLAYER_ID;

    if(dataLen > 10) {
        if(IsPlayerConnected(playerid)){
            SendClientMessage(playerid, -1, "Não foi possivel verificar seu voto, por favor tente novamente mais tarde.");
        }
        registerLog(data);
        print("FS: Não foi possível recuperar o voto do jogador.");
        return 0;
    }

    if(status != HTTP_STATUS_OK) {
        registerLog(data);
        print("FS: não foi possível verificar o voto.");
        return 0;
    }   
    
    if(!IsPlayerConnected(playerid)) {
        print("FS: O jogador não esta conectado.");
        return 0;
    }

    if(_:id >= FS_MAX_REQUESTS){
        print("FS: Limite de requisições ultrapassados.");
        return 0;
    }

    if(funcidx("OnResponsePlayerVoted") == -1) {
        print("FS: Função OnResponsePlayerVoted não esta definida.");
        return 0;
    }
    
    REQUEST_TIME[playerid] = gettime() + 10;

    return CallLocalFunction("OnResponsePlayerVoted", "ii", playerid, bool:strval(data[0]));
}

public OnRequestFailure(Request:id, errorCode, errorMessage[], len)
{
    printf("[FS:ERROR] %s", errorMessage);
    return 1;
}

public OnPlayerConnect(playerid)
{
    REQUEST_TIME[playerid] = 0;

    #if defined FS_OnPlayerConnect
        return FS_OnPlayerConnect(playerid);
    #else
        return 1;
    #endif
}


#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect FS_OnPlayerConnect

#if defined FS_OnPlayerConnect
    forward FS_OnPlayerConnect(playerid);
#endif