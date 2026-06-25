#include "DG2014_Ver3_6.h"


namespace DG
{
	//----------------------------------------------------------------------
	VertexBuffer::VertexBuffer( )
	{
		memory=nullptr;
	}
	//----------------------------------------------------------------------
	VertexBuffer::~VertexBuffer( )
	{
		ML::SafeDeleteArray(&memory);
		ML::SafeRelease(&buf);
	}
	//----------------------------------------------------------------------
	bool VertexBuffer::Initialize(D3D10_BUFFER_DESC&  desc_,  BYTE da_[], UINT str_, UINT n_)
	{
		auto  dgi = DG::DGObject::GetInst( );
		if(dgi == 0){	return false; }

		//バーテックスバッファー作成
		D3D10_SUBRESOURCE_DATA  initData;
		initData.pSysMem = da_;
		HRESULT rtv = dgi->Device( ).CreateBuffer( &desc_, &initData, &buf);
		if(FAILED(rtv)){	return false;	}
		stride = str_;
		num = n_;
		memory  =  new BYTE[str_ * n_];
		if(!memory){			return false;	}
		memcpy(memory,  da_,  str_ * n_);
		return true;
	}
	//----------------------------------------------------------------------
	//	生成する
	VertexBuffer::SP VertexBuffer::Create(BYTE da_[], UINT str_, UINT n_)
	{
		if( VertexBuffer::SP ob = VertexBuffer::SP(new VertexBuffer( )) ){
			D3D10_BUFFER_DESC  bd;
			bd.Usage = D3D10_USAGE_DEFAULT;
			bd.ByteWidth = str_ * n_;
			bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			if( ob->Initialize(bd, da_, str_, n_) ){
				return ob;
			}
		}
		return nullptr;
	}
	//----------------------------------------------------------------------
	//	生成する
	VertexBuffer::SP VertexBuffer::CreateCPUAccessMode(BYTE da_[], UINT str_, UINT n_)
	{
		if( VertexBuffer::SP ob = VertexBuffer::SP(new VertexBuffer( )) ){
			D3D10_BUFFER_DESC  bd;
			bd.Usage = D3D10_USAGE_DYNAMIC;
			bd.ByteWidth = str_ * n_;
			bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0;
			if( ob->Initialize(bd, da_, str_, n_) ){
				return ob;
			}
		}
		return nullptr;
	}
	//----------------------------------------------------------------------
	void  VertexBuffer::Update( )
	{
		void*  pv;
		buf->Map(D3D10_MAP_WRITE_DISCARD,  0,  &pv);
		if(pv){
			memcpy(pv,  memory,  stride  *  num);
		}
		buf->Unmap( );
	}
}
