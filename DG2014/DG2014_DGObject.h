#pragma warning(disable:4996)
#pragma once
#pragma warning( disable : 4005 )
#include <d3d10.h>
#include <d3dx10.h>
#include <d3dCompiler.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma warning( default : 4005 )

#include "../myLib.h"
#include <string>

#include "DG2014_Effect.h"
#include "DG2014_VertexContainer.h"
#include "DG2014_InputLayout.h"

namespace DG
{
	//-----------------------------------------------------------------------
	class DGObject
	{	
	//-----------------------------------------------------------------------
	private:
		ID3D10Device*					d3dDevice;			//デバイスオブジェクト
		IDXGISwapChain*					swapChain;			//スワップチェイン
		ID3D10RenderTargetView*			renderTargetView;	//レンダーターゲットビュー
		ID3D10Effect*					effect;				//
		ID3D10EffectTechnique*			effectTechnique;	//
		ID3D10Texture2D*				depthBuffer;
		ID3D10DepthStencilView*			depthView;			//深度ステンシルビュー

		ID3DX10Sprite*					sprite;				//2D描画スプライト
//		vector<D3DX10_SPRITE>			spriteBuffer;		
		//各種パラメータ
		EffectState*					effectState;

		DWORD							screenWidth;
		DWORD							screenHeight;
	//-----------------------------------------------------------------------
	private:
		using WP = weak_ptr<DGObject>;
		static DGObject::WP				winst;//１つしか存在できないインスタンスのウィークポインタ
	//-----------------------------------------------------------------------
	private:
		DGObject( );
		bool Initialize(HWND  hw_,  int  w_,  int  h_,  DWORD  ms_, bool  sm_,  int  w2D_,  int  h2D_);	
	//-----------------------------------------------------------------------
	public:
		EffectState&		EffectState( ){	return  *effectState;}
		ID3DX10Sprite&		Sprite( ){	return  *sprite;}
	//-----------------------------------------------------------------------
	public:
		using SP = shared_ptr<DGObject>;
		~DGObject( );
	//-----------------------------------------------------------------------
	public:
		//	DGライブラリを生成する
		static DGObject::SP Create(HWND  hw_,  int  w_,  int  h_,  DWORD  ms_,  bool  sm_,  int  w2D_,  int  h2D_);
		//アクセス
		static DGObject::SP GetInst( );
		ID3D10Device&				Device( );		//デバイスオブジェクト
		IDXGISwapChain&				SChain( );		//スワップチェイン
		ID3D10RenderTargetView&		RTView( );		//レンダーターゲットビュー
		ID3D10EffectTechnique&		ETechnique( );
		ID3D10Effect&				Effect( );
	//-----------------------------------------------------------------------
	public:
	//	画面描画（開始）
		BOOL  Begin(const  ML::Color&  c_ = ML::Color(1,0,0,0));	//	塗りつぶし色
	//	3D描画の許可を得る
		void Begin3D( );
	//	3D描画を終了する
		void End3D( );
	//	2D描画の許可を得る
		void Begin2D( );
	//	2D描画、登録済みの画像を書き込む
		void Flush2D( );
	//	2D描画のを終了する
		void End2D( );
	//	画面描画（終了）
		void  End( );
	//	デバイス
	//-----------------------------------------------------------------------
	public:
	//パラメータの更新状態を調べて適宜定数バッファに登録し直す
		DWORD  ScreenWidth( );
		DWORD  ScreenHeight( );
		//-----------------------------------------------------------------------
	public:
		//2D描画データをバッファへ登録
		void  PushBack_D3DX10_SPRITE(D3DX10_SPRITE  &s_);
	};

}
