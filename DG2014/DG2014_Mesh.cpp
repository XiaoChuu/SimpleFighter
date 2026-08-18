#include "DG2014_Ver3_6.h"
#include "../myLib.h"

#pragma warning(disable:4316)
//#include "atlstr.h"

namespace DG
{
	//-------------------------------------------------------------------
	Mesh::Mesh( )
	{
	}
	Mesh::~Mesh( )
	{
	}
	//-----------------------------------------------------------------------
	//意図的に破棄する
	void Mesh::Destroy(Mesh::SP& sp_)
	{
		sp_ = nullptr;
	}
	//-------------------------------------------------------------------
	//	メッシュを生成する
	bool Mesh::Initialize(	Material&				mc_,
							VB_Container::SP&		vbc_,//バーテックスバッファコンテナ
							Texture::SP&			tex_)//テクスチャ			
	{
		this->normalEnable  =  false;
		this->uvEnable  =  false;
		this->tecName = "P0";

		mateConts.push_back(mc_);

		MeshGroup  w;
		w.index  =  nullptr;
		w.mcNumber  =  0;
		meshGroups.push_back(w);
		vbc  =  vbc_;
		//BB・BS生成
		this->InitializeBB_BS( );
		return  true;
	}
	//-------------------------------------------------------------------
	//SOBファイルからメッシュを生成する
	Mesh::SP  Mesh::Create_FromSOBFile(	const string&			fpath_)
	{
		//実際の読み込みは別ファイルで実装
		Mesh::SP  rtv  =  Mesh::Create_FromSOBFileSub(fpath_);
		if(rtv){
			//BB・BS生成
			rtv->InitializeBB_BS( );
		}
		return rtv;
	}
	//-------------------------------------------------------------------
	//ＢＢとＢＳを生成する
	void  Mesh::InitializeBB_BS( )
	{
		//バウンディングボックス・スフィアを生成する
		auto  vb = this->vbc->GetVB(0);
		ML::Vec3  *p = (ML::Vec3*)vb->memory;
		//BB
		//最小・最大点の検出
		ML::Vec3  minP(p[0]), maxP(p[0]);
		for (UINT n = 1; n < vb->num; n++){
			minP.x = min(minP.x, p[n].x);
			minP.y = min(minP.y, p[n].y);
			minP.z = min(minP.z, p[n].z);

			maxP.x = max(maxP.x, p[n].x);
			maxP.y = max(maxP.y, p[n].y);
			maxP.z = max(maxP.z, p[n].z);
		}
		this->bb.Center =  (minP + maxP) / 2;
		this->bb.Extents = (maxP - minP) / 2;
		//BS
		//もっとも遠い点を探す
		ML::Vec3  cen = (minP + maxP) / 2;	//球の中心（BBと同じとする）
		float  len = 0;
		for (UINT n = 1; n < vb->num; n++){
			ML::Vec3  v = p[n] - cen;
			float  l = v.Length();
			len = max(l, len);
		}
	}
	//-------------------------------------------------------------------
	//マテリアルを強制的に書き換える（使用は非推奨）
	void  Mesh::UpDateMaterial(
		const ML::Color&  dif_,
		const ML::Color&  spe_,
		const ML::Color&  amb_,
		const ML::Color&  emi_)
	{
		for (size_t c = 0; c < this->mateConts.size(); ++c) {
			this->mateConts[c].diffuse = dif_;
			this->mateConts[c].speculer = spe_;
			this->mateConts[c].ambient = amb_;
			this->mateConts[c].emissive = emi_;
		}
	}

	//-------------------------------------------------------------------
	//プログラム上でメッシュを作る（普通は使わない）
	Mesh::SP  Mesh::Create(	Material&				mc_,
							VB_Container::SP&		vbc_,//バーテックスバッファコンテナ
							Texture::SP&			tex_)//テクスチャ	
	{
		if( Mesh::SP  sp  =  Mesh::SP( new  Mesh( ) ) )
		{
			if( sp->Initialize(mc_,  vbc_,  tex_) ){
				return sp;
			}
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	//	メッシュを表示する
	void Mesh::Draw( )
	{
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return;}

		//テクニック選択？
		dgi->ETechnique().GetPassByName(this->tecName.c_str())->Apply(0);
		//頂点インプットレイアウトをセット
		this->inputLayout->IASet( );


		for(	auto  it = meshGroups.begin( );
				it != meshGroups.end( );	it++){
			auto&  mt = mateConts[it->mcNumber];
			if(mt.tex){
				//テクスチャの登録
				mt.tex->PSSet(0);
			}else{
			//	//テクスチャを使用しない
				Texture::Empty_PSSet(0);
			}
			//メッシュ定数情報を更新
			dgi->EffectState( ).param.mateDiffuse   =  mt.diffuse;
			dgi->EffectState( ).param.mateSpeculer  =  mt.speculer;
			dgi->EffectState( ).param.mateAmbient   =  mt.ambient;
			dgi->EffectState( ).param.mateEmissive  =  mt.emissive;

			//プリミティブ・トポロジーとバーテックスバッファーをセット
			vbc->IASet(0);
			//プリミティブをレンダリング
			//D3D10_TECHNIQUE_DESC  dc;
			//dgi->ETechnique( ).GetDesc( &dc );
			//for( UINT p = 0; p < dc.Passes; ++p )
			//{
			dgi->EffectState( ).Update_Params( );
			dgi->ETechnique().GetPassByName(this->tecName.c_str())->Apply(0);
			if(it->index){
				dgi->Device( ).IASetIndexBuffer(it->index->buf, DXGI_FORMAT_R32_UINT, 0 );
				dgi->Device( ).DrawIndexed( it->index->num, 0 , 0 );
			}
			else{
				dgi->Device( ).Draw( vbc->Num( ), 0 );
			}
			//}
		}
	}
	//-------------------------------------------------------------------
	//	メッシュとレイの接触判定を行う（レイはメッシュのローカル座標系）
	bool  Mesh::Hit_Ray(const  ML::Vec3&  pos_,  const  ML::Vec3&  dir_,  float&  dist_,  bool  qmode_)
	{
		bool  rtv = false;
		//バーテックスコンテナから頂点座標のバッファを得る
		VertexBuffer::SP  vbPos = this->vbc->GetVB(0);

		//メッシュは内部で分割されている可能性があるのでループで処理
		for (auto  it = meshGroups.begin();
			it != meshGroups.end();	it++){

			if (it->index){
				//インデックスあり（複数回の可能性あり）
				rtv = ML::Collsion::HitRayTriangle(pos_, dir_, qmode_, vbPos, 0,
					it->index, dist_);
				if (rtv  &&  qmode_){ break; }
			}
			else{
				//インデックスなし（たぶん１回）
				rtv = ML::Collsion::HitRayTriangle(pos_, dir_, qmode_, vbPos, 0, dist_);
				if (rtv  &&  qmode_){ break; }
			}
		}
		return  rtv;
	}
	//-------------------------------------------------------------------
	//	バウンディングボックスとレイの接触判定を行う（レイはメッシュのローカル座標系）
	bool  Mesh::Hit_RayBB(const  ML::Vec3&  pos_, const  ML::Vec3&  dir_, float&  dist_)
	{
		if (ML::Collsion::HitRayBB(pos_, dir_, this->bb, dist_)){
			return true;
		}
		return  false;
	}

}
