#pragma once

///
///
///  ���ӿ� �ʿ��� enum Ŭ������
///
///  [8/11/2020 Kolyn]

enum class eTileType
{
	NONE, TILE1, TILE2,
};

enum class eGameState
{
	NONE,
	TITLE,
	HOWTO,
	CREDIT,
	MATCHING,
	LEADERSELECT,
	WAITING,
	SHOWVS,
	OPTION,
	INGAME,
	RESULT,
};

enum class eInGameState
{
	NONE,
	ACTION_TURNOVER,				//�� ����
	ACTION_UNIT_PICKING,			//���� ��ŷ
	ACTION_UNIT_CREATE_REQUEST,		//���� ���� ��û
	ACTION_UNIT_CREATE,				//���� ����
	ACTION_CASTLE_UPGRADE,			//�� ���׷��̵�
	ACTION_UNIT_ANIMATION_ATTACK,	//���� �ִϸ��̼�
	ACTION_UNIT_ANIMATION_DIE,		//���� �ִϸ��̼�
	ACTION_UNIT_MOVE,				//���� �̵�
};

enum class ePickingState
{
	STANDBY,	// ��ŷ�� ���� ���� ����. ��ŷ�� �ϴ� ���� ��ٸ��� ����
	ON,			// ������ ��ŷ �� ����. �̵� ����� ��ٸ��� �ִ�.
	OFF,		// �̵� ����� ������, ������Ʈ�� �̵����� ����. ��ŷ�� �߰������� �Ǹ� �ȵȴ�.
};

enum class eObjectType
{
	NONE,
	TILE1,
	TILE2,
	SCV,
	SWORD,
	BOW,
	KNIGHT,
	SHIELD,
	CASTLE,
	WHEAT,
	IRON,
	HORSE,
	BACKGROUNDROCK,
};

enum class eAnimationState
{
	NONE,
	ATTACK,
	DIE,
};

enum class eUICategory
{
	NONE, 
	BUTTON,
	LOCKED,
	TIMER,
};

struct MoveRangePos
{
	int x;
	int y;
};

static const int g_MainBtnCnt = 4;
static const int g_LeaderSelectBtnCnt = 4;
static const int g_InGameBtnCnt = 30;
static const int g_InResultBtnCnt = 1;
