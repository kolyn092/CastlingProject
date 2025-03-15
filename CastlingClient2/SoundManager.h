#pragma once
#include "fmod.h"
#include "fmod_errors.h"
#include <string>
#include "SoundDefine.h"
//#pragma  comment( lib, "./FMod/lib/x86/fmod_vc.lib" )


//FMOD_SOUND* sound;
//사운드 객체는 사운드 파일과 1:1 대응이 된다.출력할 사운드 개수만큼 선언하여 사용한다.
//FMOD_System_CreateSound(g_System, "파일경로 및 파일명", FMOD_DEFAULT, 0, &sound);
//FMOD_DEFAULT는 FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE의 설정과 같으며, 사운드를 1번만 출력한다.
//다른 모드로는 FMOD_LOOP_NORMAL가 있으며, 사운드를 반복 출력한다.

class SoundManager
{
public:
	SoundManager();
	~SoundManager();


private:
	static SoundManager* m_pSoundInstance;

public:
	static SoundManager* GetIns()
	{
		if (m_pSoundInstance == nullptr)
		{
			m_pSoundInstance = new SoundManager();
		}
		return m_pSoundInstance;
	}

public:
	float GetDelayTime() { return m_nowDelayTime; }

public:
	bool Initialize();

	bool LoadSoundResources();

	///index로 찾는거
	void PlaySound_T(int Track, int sound);

	///이름으로 찾는거
	void PlaySound_T(const std::string& sound_name);

	/// 타입으로 찾는 함수
	void PlaySound_Enum(eSoundType soundType);

	/// 사운드 볼륨 조절
	void SetSoundChannelVolume(int type, float volume);

	/// 사운드 정지
	void StopSoundChannel(int sound);

	/// 사운드 업데이트
	void Update();

	void CloseSoundSystem();

	void Release();
private:
	FMOD_RESULT Result;

	FMOD_SYSTEM* SoundSystem;
	FMOD_SOUND* BGMSound[AUDIONUM];
	FMOD_SOUND* SFXSound[AUDIONUM];
	FMOD_SOUND* UISound[AUDIONUM];

	FMOD_CHANNELGROUP* MasterGroup;

	FMOD_CHANNEL* BgmChannel;
	FMOD_CHANNEL* SfxChannel;
	FMOD_CHANNEL* UIChannel;


	//bool ERRCHECK(FMOD_RESULT Result);

private:
	float m_nowDelayTime;
};