#pragma	comment(lib,"winmm")	//	マルチメディア拡張機能を使用するために必要
#include "MyPG.h"

#include "resource.h"

LRESULT CALLBACK WndProc(HWND wnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_);
HWND MyPG_CreateWindow(HINSTANCE inst_, const TCHAR cName_[], const TCHAR tName_[], RECT *wSize_, int showCmd_);

//-----------------------------------------------------------------------------
//ウィンメイン
//機能概要：ウィンドウズプログラムのメイン関数
//-----------------------------------------------------------------------------
int __stdcall WinMain(	HINSTANCE inst_,	//
						HINSTANCE,			//
						LPSTR,				//
						int showCmd)		//
{
	ge = new MyPG::MyGameEngine( );

	HWND	wnd;								//	ウインドウハンドル
	RECT	ws = { 0, 0, (LONG)(ge->screenWidth  * ge->viewScaleW), (LONG)(ge->screenHeight * ge->viewScaleH) };
	wnd = MyPG_CreateWindow(inst_, ge->windowClassName, ge->windowTitle, &ws, showCmd);
	if(wnd == nullptr){ return 0;}

//	ゲーム環境の初期化
	ge->B_Initialize(wnd);

//	メッセージループ
	MSG		msg = {};
	while (msg.message != WM_QUIT)
	{
		//メッセージが来ているか
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			//メッセージに応じた処理を行う
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (GetActiveWindow() == wnd)
		{
			ge->B_Step(wnd);		//	ゲーム処理
		}
	}

	//	ゲーム環境の破棄
	delete ge;
	return 0;
}

//-----------------------------------------------------------------------------
//ウィンドウ生成
//機能概要：ウィンドウを作るための各種設定を行い、ウィンドウを生成する
//-----------------------------------------------------------------------------
HWND MyPG_CreateWindow(HINSTANCE inst_, const TCHAR cName_[], const TCHAR tName_[], RECT* wSize_, int showCmd_)
{
	WNDCLASSEX wcex;						//	ウインドウ作成に使用
	HWND wnd;								//	ウインドウハンドル

//	データ初期化
	wcex.style			= (CS_HREDRAW | CS_VREDRAW);
	wcex.hIcon = (HICON)LoadImage(inst_, MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 0, 0,  LR_DEFAULTSIZE | LR_SHARED);

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hIconSm = (HICON)LoadImage(inst_, MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 0, 0,  LR_DEFAULTSIZE | LR_SHARED);
	wcex.hInstance		= inst_;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.lpszMenuName	= nullptr;
	wcex.lpszClassName	= cName_;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
//	Windowsに登録
	if(!RegisterClassEx(&wcex))
	{
		MessageBox(nullptr, "ウインドウ登録に失敗", nullptr, MB_OK);
		return	nullptr;
	}
//	ウインドウ作成
	DWORD dws = ge->screenMode ? WS_POPUP : (WS_CAPTION | WS_SYSMENU);
	AdjustWindowRectEx(wSize_, dws, false, WS_EX_APPWINDOW);

	wnd = CreateWindowEx(	WS_EX_APPWINDOW, 
							cName_,
							tName_,
							dws, 0, 0,
							(wSize_->right  - wSize_->left),
							(wSize_->bottom - wSize_->top),
							nullptr, nullptr, inst_, nullptr);
	if(!wnd)
	{
		MessageBox(nullptr, "ウインドウ生成に失敗", nullptr, MB_OK);
		return	nullptr;
	}

//	ウインドウの状態を更新する
	ShowWindow(wnd, showCmd_);
	UpdateWindow(wnd);

	return wnd;	//成功
}

//-----------------------------------------------------------------------------
//ウィンドウプロシジャ
//機能概要：メッセージに対する処理を行う
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(	HWND	wnd_,		//	ウィンドウハンドル
							UINT	msg_,		//	メッセージ
							WPARAM	wParam_,	//	パラメータ
							LPARAM	lParam_)	//	パラメータ
{
	LRESULT ret = (LRESULT)0;
//	Windowsから送信されたメッセージを調べる
	switch(msg_){

	case WM_KEYDOWN:
		if(wParam_ == VK_BACK){
			static bool dm = true;
			ge->DebugMode( dm = !dm );
		}
		break;

	//	ウインドウが生成された
		case WM_CREATE:
			break;

	//	このウインドウがアクティブになった
		case WM_ACTIVATEAPP:
			break;

	//	×ボタンが押された
		case WM_CLOSE:
			ge->quitRequest = true;
			break;

	//	ウインドウが破棄された
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

	//	その他のメッセージはWindowsに処理を任せる
		default:
			ret = DefWindowProc(wnd_, msg_, wParam_, lParam_);
	}
	return ret;
}

