#include "DG2014_Ver3_6.h"

//#include "atlstr.h"


//┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//┃画像制御系機能群																		　┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
namespace DG
{
//----------------------------------------------------------------------
	//インデックスバッファ補助クラス
//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	IndexBuffer::IndexBuffer( )
	{
	}
	//----------------------------------------------------------------------
	IndexBuffer::~IndexBuffer( )
	{
		ML::SafeDeleteArray(&memory);
		ML::SafeRelease(&buf);
	}
	//----------------------------------------------------------------------
	bool IndexBuffer::Initialize(BYTE da_[], UINT str_, UINT n_)
	{
		auto  dgi = DG::DGObject::GetInst( );
		if(dgi == 0){	return false; }
		//インデックスバッファー作成
		D3D10_BUFFER_DESC  bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = str_ * n_;
		bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA  initData;
		initData.pSysMem = da_;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;
		HRESULT rtv = dgi->Device( ).CreateBuffer( &bd, &initData, &buf);
		if(FAILED(rtv)){
			return false;
		}
		stride = str_;
		num = n_;

		memory  =  new BYTE[str_ * n_];
		if(!memory){			return false;	}
		memcpy(memory,  da_,  str_ * n_);
		return true;
	}
	//----------------------------------------------------------------------
	//	生成する
	IndexBuffer::SP IndexBuffer::Create(BYTE da_[], UINT str_, UINT n_)
	{
		if( IndexBuffer::SP ob = IndexBuffer::SP(new IndexBuffer( )) ){
			if( ob->Initialize(da_, str_, n_) ){
				return ob;
			}
		}
		return nullptr;
	}
	//----------------------------------------------------------------------
	void  IndexBuffer::Update( )
	{
		void*  pv;
		buf->Map(D3D10_MAP_WRITE_DISCARD,  0,  &pv);
		if(pv){
			memcpy(pv,  memory,  stride  *  num);
		}
		buf->Unmap( );
	}
}
