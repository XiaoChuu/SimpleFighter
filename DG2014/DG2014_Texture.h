#pragma warning(disable:4996)
#pragma once
#pragma warning( disable : 4005 )
#include <d3dx10.h>
#include <d3dCompiler.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma warning( default : 4005 )

#include "../myLib.h"
#include <string>

namespace DG
{
	//-----------------------------------------------------------------------
	//テクスチャ補助クラス
	class Texture
	{
	//-----------------------------------------------------------------------
	public:
		using SP = shared_ptr<Texture>;
		using WP = weak_ptr<Texture>;
	//-----------------------------------------------------------------------
	private:
		Texture( );
	//-----------------------------------------------------------------------
	private:
		bool  Initialize(const  string&  fpath_);
	//-----------------------------------------------------------------------
	public:
		ID3D10ShaderResourceView*	tex_srv;//テクスチャーリソースビュー
		D3D10_TEXTURE2D_DESC		desc;
	//-----------------------------------------------------------------------
	public:
		~Texture( );
	//-----------------------------------------------------------------------
	public:
		//生成する
		static SP Create(const string&  fpath_);
		//重複する画像を別途読み込む
		static SP CreateRep(const string&  fpath_);
		//意図的に破棄する
		static void Destroy(SP& sp_);
	//-----------------------------------------------------------------------
	public:
		//ピクセルシェーダに登録する
		void  PSSet(UINT  slotNum);
		//ピクセルシェーダで利用するテクスチャを無効化させる
		static  void  Empty_PSSet(UINT  slotNum_);	
	//-----------------------------------------------------------------------
	public:
		const  D3D10_TEXTURE2D_DESC&  Desc( );
	//-----------------------------------------------------------------------
	private:
		static unordered_map<string, WP>   rep;
	};
}
