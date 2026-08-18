#include "DG2014_Ver3_6.h"

namespace DG
{
	unordered_map<string, Image::WP>	 Image::rep;
	bool  Image::dt;

	Image::Image( )
	:tex( ), angle(0), center(0,0)
	{
	}
	//-----------------------------------------------------------------------
	bool Image::ReLoad(const string&  fpath_)
	{
		return Initialize(fpath_);
	}
	//-----------------------------------------------------------------------
	bool  Image::Initialize(const  string&  fpath_)
	{
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return false; }
		//
		tex = Texture::Create(fpath_);
		if(!tex){		return false;	}
		sw = 1.0f / tex->Desc( ).Width;
		sh = 1.0f / tex->Desc( ).Height;
		return true;
	}
	//-----------------------------------------------------------------------
	Image::~Image( )
	{
		tex.reset( );
	}
	//-----------------------------------------------------------------------
	//意図的に破棄する
	void Image::Destroy(Image::SP& sp_)
	{
		sp_ = nullptr;
	}
	//-----------------------------------------------------------------------
	//生成する
	Image::SP Image::Create(const string&  fpath_)
	{
		auto it = rep.find( fpath_ );
		//既に読込済みのデータがある場合
		if(it != rep.end( )){
			if(Image::SP sp = (*it).second.lock( )){
				return sp;
			}
		}
		//新規読込が必要な場合
		if( Image::SP sp = CreateRep(fpath_) ){
			rep[fpath_] = sp;
			return sp;
		}
		return nullptr;
	}
	//重複する画像を別途読み込む
	Image::SP Image::CreateRep(const string&  fpath_)
	{
		if( Image::SP  ob = Image::SP(new Image( )) ){
			if( ob->Initialize(fpath_) ){	return ob;	}
		}
		return nullptr;
	}
	//-----------------------------------------------------------------------
	////バッファの内容を描画する
	//void  Image::Draw( )
	//{
	//	auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return; }
	//	dgi->Flush2D( );
	//}
	//バッファに登録する
	void  Image::Draw(const ML::Box2D&  d_, const ML::Box2D&  s_, const ML::Color&  c_)
	{
		dt = true;
		//SRCをテクスチャUV座標に変換
		float  u = s_.x * sw,  v = s_.y * sh,  w = s_.w * sw,  h = s_.h * sh;

		float  dx = (float)d_.x, dy = (float)d_.y, dw = (float)d_.w, dh = (float)d_.h;
		float  dwh = dw * 0.5f, dhh = dh * 0.5f;
		ML::Vec2 scale(dw, dh);									//	スケーリング係数（倍率を指定）
		ML::Vec2 drawPos(dx + dwh, -(dy + dhh));				//	描画先座標
		ML::Vec2 rotCentr(center.x - dwh, -(center.y - dhh));	//	回転軸座標

		D3DX10_SPRITE  dd;
		dd.ColorModulate = c_.D3D( );
		dd.TexCoord = ML::Vec2(u, v);
		dd.TexSize  = ML::Vec2(w, h);
		dd.pTexture = this->tex->tex_srv;
		dd.TextureIndex = 0;
		D3DXMatrixTransformation2D(
				&dd.matWorld, nullptr, 0.0f, &scale,
				&rotCentr, -angle, &drawPos);

		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return; }
		dgi->PushBack_D3DX10_SPRITE( dd );
	}
	//回転
	void  Image::Rotation(float  a_, const ML::Vec2&  c_)
	{
		angle = a_;
		center = c_;
	}
	//サイズ取得
	POINT  Image::Size( )
	{
		POINT  r = {(LONG)this->tex->Desc( ).Width, (LONG)this->tex->Desc( ).Height};
		return  r;
	}
	//-----------------------------------------------------------------------
}
