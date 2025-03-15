#include "SoundManager.h"


SoundManager* SoundManager::m_pSoundInstance = nullptr;

SoundManager::SoundManager()
	: Result(), SoundSystem(nullptr), BGMSound(), SFXSound(), UISound(),
	MasterGroup(nullptr), BgmChannel(nullptr), SfxChannel(nullptr), UIChannel(nullptr),
	m_nowDelayTime(0.f)
{

}


SoundManager::~SoundManager()
{

}

bool SoundManager::Initialize()
{
	Result = FMOD_System_Create(&SoundSystem);
	if (Result != FMOD_OK)
	{
		return false;
	}

	Result = FMOD_System_Init(SoundSystem, 100, FMOD_INIT_NORMAL, nullptr);
	if (Result != FMOD_OK)
	{
		return false;
	}

	if (LoadSoundResources() != true)
	{
		return false;
	}

	FMOD_System_CreateChannelGroup(SoundSystem, "MasterGroup", &MasterGroup);
	FMOD_Channel_SetChannelGroup(BgmChannel, MasterGroup);
	FMOD_Channel_SetChannelGroup(SfxChannel, MasterGroup);
	FMOD_Channel_SetChannelGroup(UIChannel, MasterGroup);

	m_pSoundInstance = this;
	return true;
}

bool SoundManager::LoadSoundResources()
{
	/// 반복할 음악은 FMOD_LOOP_NORMAL
	/// 한번만 출력할 음악은 FMOD_DEFAULT

	/// 배경음 CreateStream
	/// 메인화면 배경음
	FMOD_System_CreateStream(SoundSystem, "../Data/Sound/Title.wav", FMOD_LOOP_NORMAL, 0, &BGMSound[TITLEBGM]);

	/// 인게임 배경음 1
	FMOD_System_CreateStream(SoundSystem, "../Data/Sound/IngameBGM.wav", FMOD_LOOP_NORMAL, 0, &BGMSound[INGAMEBGM]);

	/// 효과음
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Soldier_Create.wav", FMOD_DEFAULT, 0, &SFXSound[SOUND_EFF_CREATION]);
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Soldier_Select.wav", FMOD_DEFAULT, 0, &SFXSound[SOUND_EFF_SELECTION]);
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Soldier_Move.wav", FMOD_DEFAULT, 0, &SFXSound[SOUND_EFF_MOVE]);
	///FMOD_System_CreateSound(SoundSystem, "../Data/Sound/PAttack2.mp3", FMOD_DEFAULT, 0, &SFXSound[SOUND_EFF_DIE]);
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Worker_Work.mp3", FMOD_DEFAULT, 0, &SFXSound[SOUND_EFF_WORKER_WORK]);
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Sword_Attack.wav", FMOD_DEFAULT, 0, &SFXSound[SOUND_EFF_SWORD_ATTACK]);
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Archer_Attack.wav", FMOD_DEFAULT, 0, &SFXSound[SOUND_EFF_BOW_ATTACK]);
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Shield_Attack.wav", FMOD_DEFAULT, 0, &SFXSound[SOUND_EFF_SHIELD_ATTACK]);
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Knight_Attack.mp3", FMOD_DEFAULT, 0, &SFXSound[SOUND_EFF_KNIGHT_ATTACK]);

	/// UI 효과음
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Button.wav", FMOD_DEFAULT, 0, &UISound[UI_BTN]);
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Error.wav", FMOD_DEFAULT, 0, &UISound[UI_CANCELBTN]);
	///FMOD_System_CreateSound(SoundSystem, "../Data/Sound/PAttacked.mp3", FMOD_DEFAULT, 0, &UISound[UI_VS]);
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Castling_Win.wav", FMOD_DEFAULT, 0, &UISound[UI_WIN]);
	FMOD_System_CreateSound(SoundSystem, "../Data/Sound/Castling_Lose.wav", FMOD_DEFAULT, 0, &UISound[UI_LOSE]);

	return true;
}

void SoundManager::PlaySound_T(int Track, int sound)
{
	switch (Track)
	{
	case BGM:
		switch (sound)
		{
		case TITLEBGM:
			FMOD_System_PlaySound(SoundSystem, BGMSound[TITLEBGM], 0, false, &BgmChannel);
			break;
		case INGAMEBGM:
			FMOD_System_PlaySound(SoundSystem, BGMSound[INGAMEBGM], 0, false, &BgmChannel);
			break;
		default:
			break;
		}
		break;
	case SFX:
		//switch (sound)
		//{
			//case BGSFX_01:
			//	FMOD_System_PlaySound(SoundSystem, SFXSound[BGSFX_01], 0, false, &BackGroundSoundChannel);
			//	break;

			//case SFX_01:
			//	FMOD_System_PlaySound(SoundSystem, SFXSound[SFX_01], 0, false, &CharacterSoundChannel);
			//	break;

			//case EFF_01:
			//	FMOD_System_PlaySound(SoundSystem, SFXSound[EFF_01], 0, false, &EffectSoundChannel);
			//	break;

			//case UISound_01:
			//	FMOD_System_PlaySound(SoundSystem, SFXSound[UISound_01], 0, false, &UISoundChannel);
			//	break;
		//default:
		//	break;
		//}
		break;
	default:
		break;
	}
}

void SoundManager::PlaySound_T(const std::string& sound_name)
{
	if (sound_name == "TITLEBGM")
	{
		FMOD_Channel_SetVolume(BgmChannel, 1.f);
		Result = FMOD_System_PlaySound(SoundSystem, BGMSound[TITLEBGM], 0, false, &BgmChannel);
	}
}

void SoundManager::PlaySound_Enum(eSoundType soundType)
{
	switch (soundType)
	{
	case eSoundType::TitleBGM:
	{
		///FMOD_Channel_SetVolume(BgmChannel, 0.5f);
		Result = FMOD_System_PlaySound(SoundSystem, BGMSound[TITLEBGM], MasterGroup, false, &BgmChannel);
	}
	break;
	case eSoundType::InGameBGM:
	{
		Result = FMOD_System_PlaySound(SoundSystem, BGMSound[INGAMEBGM], MasterGroup, false, &BgmChannel);
	}
	break;
	case eSoundType::Sound_Eff_Creation:
	{
		Result = FMOD_System_PlaySound(SoundSystem, SFXSound[SOUND_EFF_CREATION], MasterGroup, false, &SfxChannel);
	}
	break;
	case eSoundType::Sound_Eff_Selection:
	{
		Result = FMOD_System_PlaySound(SoundSystem, SFXSound[SOUND_EFF_SELECTION], MasterGroup, false, &SfxChannel);
	}
	break;
	case eSoundType::Sound_Eff_Move:
	{
		Result = FMOD_System_PlaySound(SoundSystem, SFXSound[SOUND_EFF_MOVE], MasterGroup, false, &SfxChannel);
	}
	break;
	case eSoundType::Sound_Eff_Die:
	{
		Result = FMOD_System_PlaySound(SoundSystem, SFXSound[SOUND_EFF_DIE], MasterGroup, false, &SfxChannel);
	}
	break;
	case eSoundType::Sound_Eff_Worker_Work:
	{
		Result = FMOD_System_PlaySound(SoundSystem, SFXSound[SOUND_EFF_WORKER_WORK], MasterGroup, false, &SfxChannel);
	}
	break;
	case eSoundType::Sound_Eff_Sword_Attack:
	{
		Result = FMOD_System_PlaySound(SoundSystem, SFXSound[SOUND_EFF_SWORD_ATTACK], MasterGroup, false, &SfxChannel);
	}
	break;
	case eSoundType::Sound_Eff_Bow_Attack:
	{
		Result = FMOD_System_PlaySound(SoundSystem, SFXSound[SOUND_EFF_BOW_ATTACK], MasterGroup, false, &SfxChannel);
	}
	break;
	case eSoundType::Sound_Eff_Shield_Attack:
	{
		Result = FMOD_System_PlaySound(SoundSystem, SFXSound[SOUND_EFF_SHIELD_ATTACK], MasterGroup, false, &SfxChannel);
	}
	break;
	case eSoundType::Sound_Eff_Knight_Attack:
	{
		Result = FMOD_System_PlaySound(SoundSystem, SFXSound[SOUND_EFF_KNIGHT_ATTACK], MasterGroup, false, &SfxChannel);
	}
	break;
	case eSoundType::UI_Btn:
	{
		FMOD_Channel_SetVolume(UIChannel, 0.5f);
		Result = FMOD_System_PlaySound(SoundSystem, UISound[UI_BTN], MasterGroup, false, &UIChannel);
	}
	break;
	case eSoundType::UI_CancelBtn:
	{
		FMOD_Channel_SetVolume(UIChannel, 0.5f);
		Result = FMOD_System_PlaySound(SoundSystem, UISound[UI_CANCELBTN], MasterGroup, false, &UIChannel);
	}
	break;
	case eSoundType::UI_Vs:
	{
		Result = FMOD_System_PlaySound(SoundSystem, UISound[UI_VS], MasterGroup, false, &UIChannel);
	}
	break;
	case eSoundType::UI_Win:
	{
		FMOD_Channel_SetVolume(UIChannel, 0.3f);
		Result = FMOD_System_PlaySound(SoundSystem, UISound[UI_WIN], MasterGroup, false, &BgmChannel);
	}
	break;
	case eSoundType::UI_Lose:
	{
		FMOD_Channel_SetVolume(UIChannel, 0.3f);
		Result = FMOD_System_PlaySound(SoundSystem, UISound[UI_LOSE], MasterGroup, false, &BgmChannel);
	}
	break;
	default:
		break;
	}
}

// 사운드 조절은 0.0 부터 1.0 까지
void SoundManager::SetSoundChannelVolume(int sound, float volume)
{
	switch (sound)
	{
	case BGM:
	{
		FMOD_Channel_SetVolume(BgmChannel, volume);
	}
	break;

	case SFX:
	{
		FMOD_Channel_SetVolume(SfxChannel, volume);
		FMOD_Channel_SetVolume(UIChannel, volume);
	}
	break;

	default:
		break;
	}
}

void SoundManager::StopSoundChannel(int sound)
{
	switch (sound)
	{
	case BGM:
	{
		FMOD_Channel_Stop(BgmChannel);
	}
	break;

	case SFX:
	{
		FMOD_Channel_Stop(SfxChannel);
		FMOD_Channel_Stop(UIChannel);
	}
	break;

	default:
		break;
	}

}


///////////////////////////////////////

void SoundManager::Update()
{
	FMOD_System_Update(SoundSystem);
	m_nowDelayTime++;
	if (m_nowDelayTime > g_DelaySoundTime)
	{
		m_nowDelayTime = 0;
	}
}


void SoundManager::CloseSoundSystem()
{
	for (int i = 0; i < AUDIONUM; i++)
	{
		FMOD_Sound_Release(BGMSound[i]);
	}
	for (int i = 0; i < AUDIONUM; i++)
	{
		FMOD_Sound_Release(SFXSound[i]);
	}
	for (int i = 0; i < AUDIONUM; i++)
	{
		FMOD_Sound_Release(UISound[i]);
	}
	FMOD_ChannelGroup_Release(MasterGroup);
	FMOD_System_Close(SoundSystem);
	FMOD_System_Release(SoundSystem);
}



//사용하지 않음
//bool SoundManager::ERRCHECK(FMOD_RESULT Result)
//{
//	if (Result != FMOD_OK)
//		return false;
//	return true;
//}


void SoundManager::Release()
{
	CloseSoundSystem();
	if (m_pSoundInstance)
	{
		delete m_pSoundInstance;
	}
}