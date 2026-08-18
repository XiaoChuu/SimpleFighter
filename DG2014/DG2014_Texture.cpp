#include "DG2014_Ver3_6.h"

namespace DG
{
	unordered_map<string, Texture::WP>	 Texture::rep;

	Texture::Texture( )
	:tex_srv(nullptr)
	{
	}
	//-----------------------------------------------------------------------
	bool  Texture::Initialize(const  string&  fpath_)
	{
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return false; }

		//テクスチャー作成
		HRESULT rtv  =  D3DX10CreateShaderResourceViewFromFile( 
			&dgi->Device( ), fpath_.c_str( ), NULL, NULL, &tex_srv, NULL );
		if(FAILED(rtv)){
			MessageBox(nullptr, "ファイルが開けません。", fpath_.c_str( ), MB_OK);
			return false;
		}
		//テクスチャの情報を得る
		ID3D10Resource* wtr;
		tex_srv->GetResource(&wtr);
		ID3D10Texture2D*  wt = (ID3D10Texture2D*)wtr;
		wt->GetDesc(&desc);
		ML::SafeRelease(&wtr);
		return true;
	}
	//-----------------------------------------------------------------------
	Texture::~Texture( )
	{
		ML::SafeRelease(&tex_srv);
	}
	//-----------------------------------------------------------------------
	//意図的に破棄する
	void Texture::Destroy(Texture::SP& sp_)
	{
		sp_ = nullptr;
	}
	//-----------------------------------------------------------------------
	//生成する
	Texture::SP Texture::Create(const string&  fpath_)
	{
		auto it = rep.find( fpath_ );
		//既に読込済みのデータがある場合
		if(it != rep.end( )){
			if(Texture::SP sp = (*it).second.lock( )){
				return sp;
			}
		}
		//新規読込が必要な場合
		if( Texture::SP sp = CreateRep(fpath_) ){
			rep[fpath_] = sp;
			return sp;
		}
		return nullptr;
	}
	//重複する画像を別途読み込む
	Texture::SP Texture::CreateRep(const string&  fpath_)
	{
		if( Texture::SP  ob = Texture::SP(new Texture( )) ){
			if( ob->Initialize(fpath_) ){	return ob;	}
		}
		return nullptr;
	}
	//-----------------------------------------------------------------------
	//ピクセルシェーダに登録する
	void  Texture::PSSet(UINT  slotNum_)
	{
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return; }
		ID3D10EffectShaderResourceVariable*  srv = nullptr;
		char texNum[5] = "tex0";
		texNum[3] += slotNum_;
		srv = dgi->Effect( ).GetVariableByName(texNum)->AsShaderResource();
		if(srv){
			srv->SetResource(tex_srv);
		}
	}
	void  Texture::Empty_PSSet(UINT  slotNum_)
	{
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return; }
		ID3D10EffectShaderResourceVariable*  srv = nullptr;
		char texNum[5] = "tex0";
		texNum[3] += slotNum_;
		srv = dgi->Effect( ).GetVariableByName(texNum)->AsShaderResource();
		if(srv){
			srv->SetResource(nullptr);
		}
	}
	//-----------------------------------------------------------------------
	const  D3D10_TEXTURE2D_DESC&  Texture::Desc( )
	{
		return desc;
	}

}
