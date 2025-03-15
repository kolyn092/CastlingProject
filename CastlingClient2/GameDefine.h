#pragma once

///
///
///  게임에 필요한 enum 클래스들
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
	ACTION_TURNOVER,				//턴 변경
	ACTION_UNIT_PICKING,			//유닛 피킹
	ACTION_UNIT_CREATE_REQUEST,		//유닛 생성 요청
	ACTION_UNIT_CREATE,				//유닛 생성
	ACTION_CASTLE_UPGRADE,			//성 업그레이드
	ACTION_UNIT_ANIMATION_ATTACK,	//유닛 애니메이션
	ACTION_UNIT_ANIMATION_DIE,		//유닛 애니메이션
	ACTION_UNIT_MOVE,				//유닛 이동
};

enum class ePickingState
{
	STANDBY,	// 피킹이 되지 않은 상태. 피킹을 하는 것을 기다리고 있음
	ON,			// 유닛을 피킹 한 상태. 이동 명령을 기다리고 있다.
	OFF,		// 이동 명령을 내리고, 오브젝트가 이동중인 상태. 피킹이 추가적으로 되면 안된다.
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
