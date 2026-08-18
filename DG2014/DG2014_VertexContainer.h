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

#include "DG2014_VertexBuffer.h"

namespace DG
{
	//-----------------------------------------------------------------------
	//バーテックスバッファ統括クラス
	class VB_Container
	{
	//-----------------------------------------------------------------------
	private:
		D3D10_PRIMITIVE_TOPOLOGY			priTopo;
		vector<VertexBuffer::SP>			vbs;
	//-----------------------------------------------------------------------
	public:
		using SP = shared_ptr<VB_Container>;
	//-----------------------------------------------------------------------
	private:
		VB_Container( );
		bool Initialize(D3D10_PRIMITIVE_TOPOLOGY  pt_);
	//-----------------------------------------------------------------------
	public:
		~VB_Container( );
		static SP  Create(D3D10_PRIMITIVE_TOPOLOGY  pt_);
	//-----------------------------------------------------------------------
	public:
		bool PushBack(BYTE da_[], UINT str_, UINT n_);	//	バーテックス情報を登録する（内部的にバッファ生成）
		bool  IASet(UINT  ofs_);	//ofs_はバッファ毎にstride倍
		UINT  Num( );				//頂点数を返す（最初のバッファのデータ数）
		VertexBuffer::SP  GetVB(UINT  n_);//	指定番号のバーテックスバッファを返す
	};
}
