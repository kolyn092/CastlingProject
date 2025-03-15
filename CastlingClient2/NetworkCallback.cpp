#include "NetworkCallback.h"
#include "Packet.h"
#include "CGameProcess.h"
#include "GameDefine.h"

GAME Game;
UserData myData;
UserData enemyData;

Piece pieceData;
Piece MoveStartPiece;
Piece MoveEndPiece;
int createPosX;
int createPosY;
bool isCoin;
bool checkSuccess;
eMatchUser castleUpgradeUser;
eMatchUser checkTurn = eMatchUser::MAX;
int gameResult = 0;
std::chrono::milliseconds timeData;

bool resetEvent = true;
bool setEvent = true;

// 서버
HANDLE sendCompleteEvent;
HANDLE sendStartEvent;
HANDLE sendAnimationEvent;
PROTOCOL sendProtocol = PROTOCOL::UNKNOWN;

void SendFunc(PROTOCOL protocol)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	WaitForSingleObject(sendCompleteEvent, INFINITE);
	sendProtocol = protocol;
	SetEvent(sendStartEvent);
}

int recvProc(SOCK* socket)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();
	while (1)
	{
		if (!socket->PacketRecv())
		{
			return 1;
		}

		Packet p(socket->GetPacket(), socket->GetPacketSize());
		PROTOCOL protocol = p.GetProtocol();

		int data;

		switch (protocol)
		{
			break;
			case PROTOCOL::QUICK_MATCH_ATTEND:
				p.Unpack(data);
				if (data == RESULT::SUCCESS)
				{
					gameProcess->m_GameState = eGameState::MATCHING;
				}
				else
				{
					// 매칭 실패했을 때
				}
				break;
			case PROTOCOL::QUICK_MATCH_CANCEL:
			{
				gameProcess->m_GameState = eGameState::TITLE;
			}
			break;
			case PROTOCOL::QUICK_MATCH_MAKING:
				SendFunc(PROTOCOL::MATCH_USER_DATA);
				SendFunc(PROTOCOL::MATCH_GAME_DATA);
				gameProcess->m_GameState = eGameState::LEADERSELECT;
				break;
			case PROTOCOL::MATCH_SELECT_LEADER:
				p.Unpack(data);
				break;
			case PROTOCOL::MATCH_GAME_DATA:
				p.Unpack(Game);
				if (myData.OwnPlayer != eMatchUser::MAX)
				{
					myData = Game.Data[static_cast<int>(myData.OwnPlayer)];
					enemyData = Game.Data[static_cast<int>(!myData.OwnPlayer)];
					checkTurn = Game.turn;
					timeData = Game.LeftTime;
					gameProcess->m_pressTurnBtn = false;
				}
				for (int x = 0; x < gameProcess->m_ObjManager->m_ClientMap->m_Map.WIDTH; x++)
				{
					for (int y = 0; y < gameProcess->m_ObjManager->m_ClientMap->m_Map.HEIGHT; y++)
					{
						gameProcess->m_ObjManager->m_ClientMap->m_Map.map[x][y] = Game.Board.map[x][y];
					}
				}
				if (gameProcess->m_GameState == eGameState::WAITING)
				{
					gameProcess->m_GameState = eGameState::INGAME;
				}
				break;
			case PROTOCOL::MATCH_USER_DATA:
				p.Unpack(myData);
				gameProcess->m_CamInitalize = false;
				break;
			case PROTOCOL::MATCH_SELECT_UNIT:
				break;
			case PROTOCOL::MATCH_START:
				gameProcess->m_GameState = eGameState::INGAME;
				SendFunc(PROTOCOL::ACTION_TURN_READY);
				break;
			case PROTOCOL::MATCH_TIME_DATA:
				p.Unpack(data);
				Game.LeftTime = std::chrono::milliseconds(data);
				break;
			case PROTOCOL::ACTION_TURN_REQ:
				break;
			case PROTOCOL::ACTION_TURN_READY:
				WaitForSingleObject(sendAnimationEvent, INFINITE);
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				SendFunc(PROTOCOL::ACTION_TURN_READY);
				break;
			case PROTOCOL::ACTION_TURN_OVER:
				break;
			case PROTOCOL::ACTION_UNIT_CREATE:
				p.Unpack(pieceData, createPosX, createPosY, checkSuccess);
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_CREATE;
				break;
			case PROTOCOL::ACTION_CASTLE_UPGRADE:
				p.Unpack(castleUpgradeUser);
				gameProcess->m_InGameState = eInGameState::ACTION_CASTLE_UPGRADE;
				break;
			case PROTOCOL::ACTION_UNIT_MOVE:
				p.Unpack(gameProcess->m_MapStartPosX, gameProcess->m_MapStartPosY, MoveStartPiece,
					gameProcess->m_MapEndPosX, gameProcess->m_MapEndPosY, MoveEndPiece);
				if (gameProcess->m_MapStartPosX != gameProcess->m_MapEndPosX
					|| gameProcess->m_MapStartPosY != gameProcess->m_MapEndPosY)
				{
					SoundManager* soundManager = SoundManager::GetIns();
					soundManager->StopSoundChannel(SFX);
					switch (MoveStartPiece.type)
					{
						case Piece::Type::BOW:
							soundManager->PlaySound_Enum(eSoundType::Sound_Eff_Bow_Attack);
							break;
						case Piece::Type::KNIGHT:
							soundManager->PlaySound_Enum(eSoundType::Sound_Eff_Knight_Attack);
							break;
						case Piece::Type::SHIELD:
							soundManager->PlaySound_Enum(eSoundType::Sound_Eff_Shield_Attack);
							break;
						case Piece::Type::SWORD:
							soundManager->PlaySound_Enum(eSoundType::Sound_Eff_Sword_Attack);
							break;
						default:
							break;
					}
				}
				gameProcess->m_InGameState = eInGameState::ACTION_UNIT_ANIMATION_ATTACK;
				break;
			case PROTOCOL::ACTION_GIVE_UP:
				gameProcess->m_GameState = eGameState::RESULT;
				break;
			case PROTOCOL::GAME_RESULT:
				p.Unpack(gameResult);
				gameProcess->m_GameState = eGameState::RESULT;
				break;
			case PROTOCOL::UNKNOWN:
				[[fallthrough]];
			default:
				break;
		}
	}

	return 0;
}

int sendProc(SOCK* socket)
{
	CGameProcess* gameProcess = CGameProcess::GetIns();

	Packet p;
	int data = 0;
	DWORD result = 0;
	while (1)
	{
		result = WaitForSingleObject(sendStartEvent, 990);
		if (result == WAIT_TIMEOUT)
		{
			switch (gameProcess->m_GameState)
			{
				case eGameState::TITLE:
				{

				}
				break;
				case eGameState::MATCHING:
				{

				}
				break;
				case eGameState::LEADERSELECT:
				{
					if (Game.LeftTime < std::chrono::milliseconds(0))
					{
						if (myData.leader.type == Leader::Type::NONE || myData.leader.type == Leader::Type::MAX)
						{
							p.Pack(PROTOCOL::MATCH_SELECT_LEADER, static_cast<int>(Leader::Type::MAX));
							socket->Send(p.Get(), p.size());
							gameProcess->m_GameState = eGameState::WAITING;
						}
					}
					else
					{
						p.Pack(PROTOCOL::MATCH_GAME_DATA);
						socket->Send(p.Get(), p.size());
					}
				}
				break;
				case eGameState::INGAME:
				{
					if (Game.LeftTime < std::chrono::milliseconds(0))
					{
						gameProcess->m_GameState = eGameState::WAITING;

						if (Game.turn == myData.OwnPlayer)
						{
							gameProcess->m_ObjManager->ResetPickingAll();
							p.Pack(PROTOCOL::ACTION_TURN_REQ);
							socket->Send(p.Get(), p.size());
						}
					}
					else
					{
						p.Pack(PROTOCOL::MATCH_GAME_DATA);
						socket->Send(p.Get(), p.size());
					}
				}
				break;
				case eGameState::RESULT:
				{

				}
				break;
				default:
					break;
			}
		}
		else if (result == WAIT_OBJECT_0)
		{
			switch (sendProtocol)
			{
				case PROTOCOL::UNKNOWN:
					return 0;
					break;
				case PROTOCOL::QUICK_MATCH_ATTEND:
					p.Pack(PROTOCOL::QUICK_MATCH_ATTEND);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::QUICK_MATCH_CANCEL:
					p.Pack(PROTOCOL::QUICK_MATCH_CANCEL);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::QUICK_MATCH_MAKING:
					break;
				case PROTOCOL::MATCH_SELECT_LEADER:
					data = static_cast<int>(myData.leader.type);
					p.Pack(PROTOCOL::MATCH_SELECT_LEADER, data);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::MATCH_GAME_DATA:
					p.Pack(PROTOCOL::MATCH_GAME_DATA);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::MATCH_USER_DATA:
					p.Pack(PROTOCOL::MATCH_USER_DATA);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::MATCH_SELECT_UNIT:
					break;
				case PROTOCOL::MATCH_START:
					break;
				case PROTOCOL::ACTION_TURN_REQ:
					p.Pack(PROTOCOL::ACTION_TURN_REQ);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::ACTION_TURN_READY:
					p.Pack(PROTOCOL::ACTION_TURN_READY);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::ACTION_TURN_OVER:
					break;
				case PROTOCOL::ACTION_UNIT_CREATE:
					p.Pack(PROTOCOL::ACTION_UNIT_CREATE, pieceData, createPosX, createPosY, isCoin);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::ACTION_CASTLE_UPGRADE:
					p.Pack(PROTOCOL::ACTION_CASTLE_UPGRADE, isCoin);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::ACTION_UNIT_MOVE:
					p.Pack(PROTOCOL::ACTION_UNIT_MOVE,
						gameProcess->m_MapStartPosX, gameProcess->m_MapStartPosY,
						gameProcess->m_MapEndPosX, gameProcess->m_MapEndPosY);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::ACTION_GIVE_UP:
					p.Pack(PROTOCOL::ACTION_GIVE_UP);
					socket->Send(p.Get(), p.size());
					break;
				case PROTOCOL::GAME_RESULT:
					break;
			}
			SetEvent(sendCompleteEvent);
		}
	}

	return 0;
}
