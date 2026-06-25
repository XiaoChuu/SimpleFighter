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
	//入力レイアウト補助クラス
	class InputLayout
	{
		//-----------------------------------------------------------------------
	public:
		ID3D10InputLayout * layout;//入力レイアウト
		string					tecName;
		//		DWORD					tecNum;//対象テクニック番号
		//-----------------------------------------------------------------------
	public:
		using SP = shared_ptr<InputLayout>;
		//-----------------------------------------------------------------------
	private:
		InputLayout();
		bool Initialize(
			const  string  tecName_,
			D3D10_INPUT_ELEMENT_DESC  elem_[],
			UINT  nElem_);
		//-----------------------------------------------------------------------
	public:
		~InputLayout();
		//-----------------------------------------------------------------------
	public:
		static SP Create(
			const  string  tecName_,
			D3D10_INPUT_ELEMENT_DESC  elem_[],
			UINT  nElem_);
		//-----------------------------------------------------------------------
	public:
		bool IASet();
	};
}
