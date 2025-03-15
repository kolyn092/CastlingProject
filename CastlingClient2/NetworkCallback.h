#pragma once
#include "sock.h"
#include "Net_Predefined.h"
#include "UserData.h"
#include <chrono>

extern HANDLE sendCompleteEvent;
extern HANDLE sendStartEvent;
extern HANDLE sendAnimationEvent;

extern bool resetEvent;
extern bool setEvent;

extern UserData myData;
extern UserData enemyData;

extern Piece pieceData;
extern Piece MoveStartPiece;
extern Piece MoveEndPiece;
extern int createPosX;
extern int createPosY;
extern bool isCoin;
extern bool checkSuccess;
extern eMatchUser castleUpgradeUser;
extern eMatchUser checkTurn;
extern int gameResult;

extern std::chrono::milliseconds timeData;


void SendFunc(PROTOCOL protocol);

int recvProc(SOCK* socket);
int sendProc(SOCK* socket);