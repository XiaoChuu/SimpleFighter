#include "DG2014_Ver3_6.h"

//#include "atlstr.h"


//┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//┃画像制御系機能群																		　┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
namespace DG
{


	//-----------------------------------------------------------------------
	//入力レイアウト補助クラス
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	InputLayout::InputLayout()
	{
	}
	//-----------------------------------------------------------------------
	InputLayout::~InputLayout()
	{
		ML::SafeRelease(&layout);
	}
	//-----------------------------------------------------------------------
	bool InputLayout::Initialize(
		const  string  tecName_,
		D3D10_INPUT_ELEMENT_DESC  elem_[],
		UINT  nElem_)
	{
		auto  dgi = DG::DGObject::GetInst(); if (dgi == 0) { return false; }
		D3D10_PASS_DESC  passDesc;
		this->tecName = tecName_;
		dgi->ETechnique().GetPassByName(this->tecName.c_str())->GetDesc(&passDesc);

		HRESULT rtv;
		//頂点インプットレイアウトを作成
		rtv = dgi->Device().CreateInputLayout(
			elem_, nElem_,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&this->layout);
		if (FAILED(rtv)) { return false; }
		return true;
	}
	//-----------------------------------------------------------------------
	InputLayout::SP InputLayout::Create(
		const  string  tecName_,
		D3D10_INPUT_ELEMENT_DESC  elem_[],
		UINT  nElem_)
	{
		if (InputLayout::SP ob = InputLayout::SP(new InputLayout())) {
			if (ob->Initialize(tecName_, elem_, nElem_)) {
				return ob;
			}
		}
		return nullptr;
	}
	//-----------------------------------------------------------------------
	bool InputLayout::IASet()
	{
		auto  dgi = DG::DGObject::GetInst(); if (dgi == 0) { return false; }

		//レイアウト設定
		dgi->Device().IASetInputLayout(layout);
		return true;
	}

}