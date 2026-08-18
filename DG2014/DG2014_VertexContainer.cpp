#include "DG2014_Ver3_6.h"


namespace DG
{
	//-----------------------------------------------------------------------
	//バーテックスバッファ統括クラス
	//----------------------------------------------------------------------
	VB_Container::VB_Container( )
	{
	}
	//----------------------------------------------------------------------
	VB_Container::~VB_Container( )
	{
	}	
	//----------------------------------------------------------------------
	bool VB_Container::Initialize(D3D10_PRIMITIVE_TOPOLOGY  pt_)
	{
		priTopo = pt_;
		return true;
	}
	//----------------------------------------------------------------------
	//	生成する
	VB_Container::SP  VB_Container::Create(D3D10_PRIMITIVE_TOPOLOGY  pt_)
	{
		if( VB_Container::SP ob = VB_Container::SP(new VB_Container( )) ){
			if( ob->Initialize(pt_) ){
				return ob;
			}
		}
		return nullptr;
	}
	//----------------------------------------------------------------------
	bool  VB_Container::IASet(UINT ofs_)	//ofs_はバッファ毎にstride倍
	{
		auto  dgi = DG::DGObject::GetInst( );
		if(dgi == 0){	return false; }

		//プリミティブ・トポロジーをセット
		dgi->Device( ).IASetPrimitiveTopology( priTopo );
		auto  it = vbs.begin( );
		for(int c=0; it != vbs.end( ); ++c, ++it)
		{
			//バーテックスバッファーをセット
			UINT  ofs = ofs_ * (*it)->stride;
			dgi->Device( ).IASetVertexBuffers( c, 1, &(*it)->buf, &(*it)->stride, &ofs);
		}
		return true;
	}
	//----------------------------------------------------------------------
	//頂点数を返す
	UINT  VB_Container::Num( )
	{
		auto  it = vbs.begin( );
		if(it != vbs.end( )){
			return  (*it)->num;
		}
		return 0;
	}
	//----------------------------------------------------------------------
	//	バーテックス情報を登録する（内部的にバッファ生成）
	bool  VB_Container::PushBack(BYTE da_[], UINT str_, UINT n_)
	{
		DG::VertexBuffer::SP  w = DG::VertexBuffer::Create(da_, str_, n_);
		if(!w){	return false;	}
		vbs.push_back( w );
		return true;
	}
	//----------------------------------------------------------------------
	//	指定番号のバーテックスバッファを返す
	VertexBuffer::SP  VB_Container::GetVB(UINT  n_)
	{
		return  vbs[n_];
	}
}
