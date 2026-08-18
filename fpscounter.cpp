// FPS計測クラス実装ファイル

#include "FPSCounter.h"


void FPSCounter::DrawFps()
{
	//FPSカウント
	FpsCnt++;
	//1秒経過を取得
	if (timeGetTime() >= (unsigned)FpsTime) {
		FpsData = FpsCnt;
		FpsCnt = 0;
		FpsTime = (timeGetTime() + 1000);
	}

	char Buffer[1024];
	sprintf_s(Buffer, "Spe fps = %0.2f : real fps = %3d : deltaTime=%0.4f", _REFRESHRATE, FpsData, deltaTime);
	SetWindowText(GetActiveWindow(), (LPCSTR)Buffer);
}

bool FPSCounter::LoopControl() {
	float _FPS = 1000.0f / static_cast<float>(_REFRESHRATE);					//FPS計算用バッファ
	if (_REFRESHRATE != 0) {
		//				//与えられたFPSからFrameTimeを計算	
		if (timeGetTime() - _Time >= (unsigned)_FPS) {		//FrameTimeが経過するまで待つ
			_Time = timeGetTime();
			return 1;
		}
		return 0;
	}
	else {
		return 1;
	}
}

int FPSCounter::GetFps()
{
	return _REFRESHRATE;
}

// コンストラクタ
FPSCounter::FPSCounter(float fps)
{
	// 精度を上げる
	timeBeginPeriod(1);

	_REFRESHRATE = fps;
	ONEFRAME_TIME = 1.0 / _REFRESHRATE;

	FpsTime = (timeGetTime() + 1000);
	FpsData = 0.0;
	FpsCnt = 1;

	//FPS制御用
	_Time = timeGetTime();	//初期時間格納

	deltaTime = (timeGetTime() - oldTime) / 1000.f;
	oldTime = timeGetTime();
}

// デストラクタ
FPSCounter::~FPSCounter(void)
{
	timeEndPeriod(1);    // タイマーの精度を戻す
}

// FPSを更新
double FPSCounter::UpdateFPS()
{
	deltaTime = (timeGetTime() - oldTime) / 1000.f;
	oldTime = timeGetTime();
	return deltaTime;
}


//
////float		FrameTime;
////const int	REFRESHRATE = 60;
////const float	ONEFRAME_TIME = 1.0f / REFRESHRATE;
////---------------------------------------------------------------------------------------
//// ループ制御
////---------------------------------------------------------------------------------------
//bool MyGameEngine::LoopControl(int FPS) {
//	if (FPS != 0) {
//		_FPS = 1000.0f / static_cast<float>(FPS);		//与えられたFPSからFrameTimeを計算	
//		if (timeGetTime() - _Time > (unsigned)_FPS) {		//FrameTimeが経過するまで待つ
//			_Time = timeGetTime();
//			return 1;
//		}
//		return 0;
//	}
//	else {
//		return 1;
//	}
//}
//
//void MyGameEngine::Draw_ShowFps(int x, int y)
//{
//	static DWORD FpsTime = (timeGetTime() + 1000);
//	static double FpsData = 0.0;
//	static int FpsCnt = 1;
//	//1秒経過によるfpsの計測
//	if (timeGetTime() > (unsigned)FpsTime) {
//		FpsData = FpsCnt;
//		FpsCnt = 0;
//		FpsTime = (timeGetTime() + 1000);
//	}
//	//FPSカウント
//	FpsCnt++;
//	char Buffer[1024];
//	sprintf_s(Buffer, "fps=%3.2f:dt=%0.3f", FpsData, ge->deltaTime);
//	ML::Box2D draw(x, y, 480, 30);
//
//	//		this->debugFont->Draw(draw, Buffer, ML::Color(1, 1, 1, 1));
//	Dbg_ToTitle((LPCSTR)Buffer);
//}


