#pragma once
#pragma comment(lib, "winmm.lib")


#include <windows.h>
#include <mmsystem.h>
#include <list>



using namespace std;



class FPSCounter
{
private:
	float _REFRESHRATE;
	float ONEFRAME_TIME;
	float		oldTime;			//1フレーム前の時間を計測
	//経過時間観測用
	DWORD FpsTime;
	int FpsData;
	int FpsCnt;
	//FPS制御用
	int _Time;	//初期時間格納

public:
	float		deltaTime;			//フレームタイム
	// FPSを更新　毎フレーム呼び出し
	double UpdateFPS();
	// FPS値を取得　毎フレーム呼び出し
	int GetFps();
	//FPS表示
	void DrawFps();
	//指定FPSで動作
	bool LoopControl();

	FPSCounter(float);
	virtual ~FPSCounter(void);

};

