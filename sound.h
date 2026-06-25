#pragma once
//----------------------------------------------
//音楽再生ライブラリ
// DirectSound DirectShow 汎用関数
//      k.koyanagi
//----------------------------------------------
// 2022.07.21 バグ修正版
//----------------------------------------------

#include <assert.h> 
#include <string>
#include <dshow.h>
#include <dsound.h>

/*
how to use

プロジェクトに両ファイルを追加

利用したいCPPで
#include "sound.h"
を追加

------------bgm------------------
対応形式はDirectShowに準ずる


起動時に
bgm::Init();

終了時に
bgm::Del();

ファイルの読み込み
利用前に一回だけ
bgm::LoadFile("testbgm","./sound/test.mp3");

再生
bgm::Play("testbgm");

毎フレーム行うこと
bgm::EndCheck();
------------------------------

------------se------------------
対応形式はDirectSoundに準ずる


起動時に
se::Init();

終了時に
se::Del();

ファイルの読み込み
利用前に一回だけ
se::LoadFile("testse","./sound/test.wav");

再生
se::Play("testbgm");

毎フレーム行うこと
se::EndCheck();
------------------------------





*/






namespace bgm{
	//BGM再生関数
	void Init();
	void Del();
	void LoadFile(const std::string& resname, const std::string& filename);
	bool Play(const std::string& resname);
	void RateControl(const std::string& resname, float rate);
	void EndCheck();
	void Stop(const std::string& resname);
	void AllStop();
	LONGLONG GetCurrentPos(const std::string& resname);
	void SetStartPos(const std::string& resname);
	void VolumeControl(const std::string& resname, int volume);
	void Pause(const std::string& resname);
	LONGLONG GetEndPosition(const std::string& resname);
}
namespace se{
	//SE再生関数
	void Init(HWND);
	bool Create(HWND);
	void CreateSecondaryBuffer();
	bool CreatePrimaryBuffer();
	bool Del();
	void Play(const std::string& resname);
	void PlayLoop(const std::string& resname);
	bool Del();
	void LoadFile(const std::string& resname, const std::string& filename);
	void AllStop();
	void Stop(const std::string& resname);
	void EndCheck();
	void SetFrequency(const std::string& resname, int Fre);
	void SetVolume(const std::string& resname, int Vol);
}


