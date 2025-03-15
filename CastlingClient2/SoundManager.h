#pragma once
#include "fmod.h"
#include "fmod_errors.h"
#include <string>
#include "SoundDefine.h"
//#pragma  comment( lib, "./FMod/lib/x86/fmod_vc.lib" )


//FMOD_SOUND* sound;
//���� ��ü�� ���� ���ϰ� 1:1 ������ �ȴ�.����� ���� ������ŭ �����Ͽ� ����Ѵ�.
//FMOD_System_CreateSound(g_System, "���ϰ�� �� ���ϸ�", FMOD_DEFAULT, 0, &sound);
//FMOD_DEFAULT�� FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE�� ������ ������, ���带 1���� ����Ѵ�.
//�ٸ� ���δ� FMOD_LOOP_NORMAL�� ������, ���带 �ݺ� ����Ѵ�.

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

	///index�� ã�°�
	void PlaySound_T(int Track, int sound);

	///�̸����� ã�°�
	void PlaySound_T(const std::string& sound_name);

	/// Ÿ������ ã�� �Լ�
	void PlaySound_Enum(eSoundType soundType);

	/// ���� ���� ����
	void SetSoundChannelVolume(int type, float volume);

	/// ���� ����
	void StopSoundChannel(int sound);

	/// ���� ������Ʈ
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