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
	//インデックスバッファ補助クラス
	//-----------------------------------------------------------------------
	class IndexBuffer
	{
	//-----------------------------------------------------------------------
	public:
		ID3D10Buffer*	buf;
		UINT			stride;	//データ1件分のデータ数
		UINT			num;	//データ件数
		void*			memory;	//アクセス用
	//-----------------------------------------------------------------------
	public:
		using SP = shared_ptr<IndexBuffer>;
		void  Update( );
	//-----------------------------------------------------------------------
	private:
		IndexBuffer( );
		bool Initialize(BYTE da_[], UINT str_, UINT n_);
	//-----------------------------------------------------------------------
	public:
		~IndexBuffer( );
	//-----------------------------------------------------------------------
	public:
		static SP Create(BYTE da_[], UINT str_, UINT n_);
	};
}
