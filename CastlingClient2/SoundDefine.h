#pragma once

const float g_DelaySoundTime = 300;
const int g_FootStepWalkFrame = 18;

// Sound
#define AUDIONUM 10

/// BGM
#define BGM								0

#define TITLEBGM						0
#define INGAMEBGM						1

/// SFX
#define SFX								1

#define SOUND_EFF_CREATION				0
#define SOUND_EFF_SELECTION				1
#define SOUND_EFF_MOVE					2
#define SOUND_EFF_DIE					3
#define SOUND_EFF_WORKER_WORK			4
#define SOUND_EFF_SWORD_ATTACK			5
#define SOUND_EFF_BOW_ATTACK			6
#define SOUND_EFF_SHIELD_ATTACK			7
#define SOUND_EFF_KNIGHT_ATTACK			8

#define UI_BTN						0
#define UI_CREATIONBTN					1
#define UI_MENUBTN						2
#define UI_CANCELBTN					3
#define UI_VS							4
#define UI_WIN							5
#define UI_LOSE							6

enum class eSoundType
{
	TitleBGM,
	InGameBGM,
	Sound_Eff_Creation,
	Sound_Eff_Selection,
	Sound_Eff_Move,
	Sound_Eff_Die,
	Sound_Eff_Worker_Work,
	Sound_Eff_Sword_Attack,
	Sound_Eff_Bow_Attack,
	Sound_Eff_Shield_Attack,
	Sound_Eff_Knight_Attack,
	UI_Btn,
	UI_CancelBtn,
	UI_Vs,
	UI_Win,
	UI_Lose,
};