#include "DG2014_Ver3_6.h"

namespace DG
{
	//シェーダで使用する名称一覧
	namespace  EffectNames{
		string  material("MaterialCB");
		string  matrixs("MatrixCB");
		string  fog("FogCB");
		string  lights("LightsCB");
		string  etc("EtcCB");
	}
//	bool  sf_Matrix(const  string&  name_,  const  ML::Mat4x4&  m_);
	bool  sf_Vec4(const  string&  name_,  const  ML::Color&  m_);
	bool  sf_Vec4(const  string&  name_,  const  ML::Vec4&  m_);
	bool  sf_Bool4(const  string&  name_,  BOOL  a_, BOOL  b_, BOOL  c_, BOOL  d_);

	template<class  T>
	bool  sf_CBuffer(const  string&  name_,  T&  data)
	{
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return  false; }
		//定数バッファー作成
		ID3D10Buffer*	buf;
		D3D10_BUFFER_DESC  bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(T);
		bd.BindFlags =  D3D10_BIND_CONSTANT_BUFFER ;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA  initData;
		initData.pSysMem = &data;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;
		HRESULT rtv = dgi->Device( ).CreateBuffer( &bd, &initData, &buf);
		if(FAILED(rtv)){
			return false;
		}
		ID3D10EffectConstantBuffer*  evv;
		if(evv = dgi->Effect( ).GetConstantBufferByName( name_.c_str( ) )->AsConstantBuffer( ))
		{
			rtv  =  evv->SetConstantBuffer(buf);
		}
		ML::SafeRelease(&buf);
		return  S_OK == rtv;
	}
//-----------------------------------------------------------------------------------
	void  EffectState::Update_Params( )
	{
		this->Update_Material( );
		this->Update_Fog( );
		this->Update_Lights( );
		this->Update_Matrixs( );
		this->Update_Etc( );
	}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//未分類
	struct  Etc_CB
	{
		ML::Vec4		eyePos;
	};
	void  EffectState::Update_Etc( )
	{
		static  bool  upd  =  true;
	//更新状態をチェック、更新がなければ定数バッファを更新せず終了する
		if(this->prevParam.eyePos  !=  this->param.eyePos){this->prevParam.eyePos = this->param.eyePos;	upd = true;}
		if(upd == false){	return;}
	//定数バッファの更新
		Etc_CB	cb = {
			this->param.eyePos.Vec4( )
		};
		sf_CBuffer(EffectNames::etc, cb);
		upd  =  false;
	}
//-----------------------------------------------------------------------------------
//Matrix系
	struct  Matrix_CB
	{
		ML::Mat4x4		world;
		ML::Mat4x4		view;
		ML::Mat4x4		projection;
		ML::Mat4x4		w_v;
		ML::Mat4x4		w_v_p;
		ML::Mat4x4		v_p;
	};
	void  EffectState::Update_Matrixs( )
	{
		static  bool  upd  =  true;
	//更新状態をチェック、更新がなければ定数バッファを更新せず終了する
		if(this->prevParam.matWorld  !=  this->param.matWorld){this->prevParam.matWorld = this->param.matWorld;	upd = true;}
		if(this->prevParam.matView  !=  this->param.matView){this->prevParam.matView = this->param.matView;	upd = true;}
		if(this->prevParam.matProjection  !=  this->param.matProjection){this->prevParam.matProjection = this->param.matProjection;	upd = true;}
		if(upd == false){	return;}
	//定数バッファの更新
		ML::Mat4x4  mWV = this->param.matWorld  *  this->param.matView;
		ML::Mat4x4  mWVP = this->param.matWorld  *  this->param.matView  *  this->param.matProjection;
		ML::Mat4x4  mVP = this->param.matView   *  this->param.matProjection;

		Matrix_CB	cb = {
			this->param.matWorld.Transpose( ),
			this->param.matView.Transpose( ),
			this->param.matProjection.Transpose( ),
			mWV.Transpose( ),
			mWVP.Transpose( ),
			mVP.Transpose( )
		};
		sf_CBuffer(EffectNames::matrixs, cb);
		upd  =  false;
	}
//-----------------------------------------------------------------------------------
//Light系
	struct  Light_CB{
		BOOL		enable;
		Light::Kind kind;			//0ディレクショナル　1ポイント	2スポット
		float		attenuation;	//減衰率
		float		range;			//有効距離
		ML::Vec4	direction;		//ライト照射方向
		ML::Vec4	directionInv;	//ライト照射方向(逆ベクトル）
		ML::Vec4	pos;			//ライトの位置（追加！）
		ML::Color	color;			//ライトの色兼強さ
	};
	struct  Lights_CB{
		BOOL		enable;
		BOOL		n0;
		BOOL		n1;
		BOOL		n2;
		ML::Color	ambient;
		Light_CB	light[4];
	};

	Light_CB  LightToLight_CB(DG::Light&  lg_)
	{
		Light_CB  w = {lg_.enable,  lg_.kind,
			lg_.attenuation,
			lg_.range,	
			lg_.direction.Vec4( ),
			-lg_.direction.Vec4( ),		//逆照射ベクトルを設定
			lg_.pos.Vec4( ),			//ライトの位置（追加！）
			lg_.color};
		return  w;
	}
	void  EffectState::Update_Lights( )
	{
		static  bool  upd  =  true;
	//更新状態をチェック、更新がなければ定数バッファを更新せず終了する
		if(this->prevParam.lightsEnable  !=  this->param.lightsEnable){this->prevParam.lightsEnable = this->param.lightsEnable;	upd = true;}
		if(this->prevParam.lightAmbient  !=  this->param.lightAmbient){this->prevParam.lightAmbient = this->param.lightAmbient;	upd = true;}
		if(this->prevParam.light[0]  !=  this->param.light[0]){this->prevParam.light[0] = this->param.light[0];	upd = true;}
		if(this->prevParam.light[1]  !=  this->param.light[1]){this->prevParam.light[1] = this->param.light[1];	upd = true;}
		if(this->prevParam.light[2]  !=  this->param.light[2]){this->prevParam.light[2] = this->param.light[2];	upd = true;}
		if(this->prevParam.light[3]  !=  this->param.light[3]){this->prevParam.light[3] = this->param.light[3];	upd = true;}
		if(upd == false){	return;}
	//定数バッファの更新
		Lights_CB	lcb = {
			this->param.lightsEnable,
			false,
			false,
			false,
			this->param.lightAmbient,
			{	LightToLight_CB(this->param.light[0]), 
				LightToLight_CB(this->param.light[1]),
				LightToLight_CB(this->param.light[2]),
				LightToLight_CB(this->param.light[3])}
		};
		sf_CBuffer(EffectNames::lights, lcb);
		upd  =  false;
	}
//-----------------------------------------------------------------------------------
//Fog系
	struct  Fog_CB{
		BOOL		enable;
		BOOL		mode;
		BOOL		n0;
		BOOL		n1;
		float		nearp;
		float		fore;
		float		p0;
		float		p1;
		ML::Color	color;
	};
	void  EffectState::Update_Fog( )
	{
		static  bool  upd  =  true;
	//更新状態をチェック、更新がなければ定数バッファを更新せず終了する
		if(this->prevParam.fogEnable  !=  this->param.fogEnable){this->prevParam.fogEnable = this->param.fogEnable;	upd = true;}
		if(this->prevParam.fogMode  !=  this->param.fogMode){this->prevParam.fogMode = this->param.fogMode;	upd = true;}
		if(this->prevParam.fogNear  !=  this->param.fogNear){this->prevParam.fogNear = this->param.fogNear;	upd = true;}
		if(this->prevParam.fogFore  !=  this->param.fogFore){this->prevParam.fogFore = this->param.fogFore;	upd = true;}
		if(this->prevParam.fogColor  !=  this->param.fogColor){this->prevParam.fogColor = this->param.fogColor;	upd = true;}
		if(upd == false){	return;}
	//定数バッファの更新
		Fog_CB	fcb = {
			this->param.fogEnable,
			this->param.fogMode,
			false,
			false,
			this->param.fogNear,
			this->param.fogFore,
			0,
			0,
			this->param.fogColor
		};
		sf_CBuffer(EffectNames::fog, fcb);
		upd  =  false;
	}
//-----------------------------------------------------------------------------------
//マテリアル系
	struct  Material_CB
	{
		ML::Color	diffuse;	//マテリアル情報:デフューズ
		ML::Color	speculer;	//マテリアル情報:スペキュラ
		ML::Color	ambient;	//マテリアル情報:アンビエント
		ML::Color	emissive;	//マテリアル情報:エミッシブ
		ML::Color	objectColor;	//モデルの色を強制的に変化させる
	};
	void  EffectState::Update_Material( )
	{
		static  bool  upd  =  true;
	//更新状態をチェック、更新がなければ定数バッファを更新せず終了する
		if(this->prevParam.mateDiffuse  !=  this->param.mateDiffuse){this->prevParam.mateDiffuse = this->param.mateDiffuse;	upd = true;}
		if(this->prevParam.mateSpeculer  !=  this->param.mateSpeculer){this->prevParam.mateSpeculer = this->param.mateSpeculer;	upd = true;}
		if(this->prevParam.mateAmbient  !=  this->param.mateAmbient){this->prevParam.mateAmbient = this->param.mateAmbient;	upd = true;}
		if(this->prevParam.mateEmissive  !=  this->param.mateEmissive){this->prevParam.mateEmissive = this->param.mateEmissive;	upd = true;}
		if(this->prevParam.objectColor  !=  this->param.objectColor){this->prevParam.objectColor = this->param.objectColor;	upd = true;}
		if(upd == false){	return;}
	//定数バッファの更新
		Material_CB	cb = {
			this->param.mateDiffuse,
			this->param.mateSpeculer,
			this->param.mateAmbient,
			this->param.mateEmissive,
			this->param.objectColor
		};
		sf_CBuffer(EffectNames::material, cb);
		upd  =  false;
	}


//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
	EffectParam::EffectParam( )
	{
		ML::Mat4x4  idm;
		idm.Identity( );
		matWorld  =  idm;
		matView  =  idm;
		matProjection  =  idm;

		objectColor  =  ML::Color(1,1,1,1);
		bgColor  =  ML::Color(1,1,1,1);

		mateDiffuse  =  ML::Color(1,1,1,1);
		mateSpeculer  =  ML::Color(1,1,1,1);
		mateAmbient  =  ML::Color(1,1,1,1);
		mateEmissive  =  ML::Color(1,1,1,1);
		
		lightsEnable   =  false;
		lightAmbient   =  ML::Color(0,0,0,0);
		light[0].enable  =  false;
		light[1].enable  =  false;
		light[2].enable  =  false;
		light[3].enable  =  false;

		eyePos = ML::Vec3(0,0,0);

		fogEnable  =  false;
		fogMode    =  true;
		fogNear  = 0;
		fogFore  = 0;
		fogColor  =  ML::Color(1,1,1,1);

		bs_Active  =  nullptr;
		rs_Active  =  nullptr;
		dss_Active  =  nullptr;
	}
	EffectState::EffectState( )
		:param( )
	{
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return;  }
		//アルファブレンディング設定
		D3D10_BLEND_DESC  bsDesc;
		ZeroMemory(&bsDesc, sizeof(D3D10_BLEND_DESC));
		bsDesc.BlendEnable[0] = FALSE;
		bsDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
		dgi->Device( ).CreateBlendState(&bsDesc, &bs_Std);
		//
		bsDesc.BlendEnable[0] = TRUE;
//		bsDesc.AlphaToCoverageEnable  =  TRUE;
		bsDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
		bsDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
		bsDesc.BlendOp = D3D10_BLEND_OP_ADD;
		bsDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
		bsDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
		bsDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
		bsDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
		dgi->Device( ).CreateBlendState(&bsDesc, &bs_Alpha);
		//
		bsDesc.DestBlend = D3D10_BLEND_ONE;
		dgi->Device( ).CreateBlendState(&bsDesc, &bs_Add);
		//
		//bsDesc.BlendOp = D3D10_BLEND_OP_SUBTRACT;
		bsDesc.BlendOp = D3D10_BLEND_OP_REV_SUBTRACT;
		dgi->Device( ).CreateBlendState(&bsDesc, &bs_Dec);
		this->param.bs_Active  =  bs_Std;

		//ラスタライザ設定
		D3D10_RASTERIZER_DESC  raDesc;
		raDesc.FillMode = D3D10_FILL_SOLID;
		raDesc.CullMode = D3D10_CULL_BACK;
		raDesc.FrontCounterClockwise = false;
		raDesc.DepthBias = 0;
		raDesc.DepthBiasClamp = 0;
		raDesc.SlopeScaledDepthBias = 0;
		raDesc.DepthClipEnable = true;
		raDesc.ScissorEnable = false;
		raDesc.MultisampleEnable = true;
		raDesc.AntialiasedLineEnable = false;
		dgi->Device( ).CreateRasterizerState( &raDesc, &this->rs_Def3D );

		raDesc.CullMode = D3D10_CULL_NONE;
		raDesc.DepthClipEnable = true;
		dgi->Device( ).CreateRasterizerState( &raDesc, &this->rs_Def2D );
		this->param.rs_Active  =  this->rs_Def3D;

		//深度　ステンシル
		D3D10_DEPTH_STENCIL_DESC  dsDesc;
		dsDesc.DepthEnable  =  TRUE;
		dsDesc.DepthWriteMask  =  D3D10_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc  =  D3D10_COMPARISON_LESS;
		dsDesc.StencilEnable  =  FALSE;
		dsDesc.StencilReadMask  =  D3D10_DEFAULT_STENCIL_READ_MASK;
		dsDesc.StencilWriteMask  =  D3D10_DEFAULT_STENCIL_WRITE_MASK;
		dsDesc.FrontFace.StencilFailOp       =  D3D10_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp  =  D3D10_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp       =  D3D10_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc         =  D3D10_COMPARISON_ALWAYS;
		dsDesc.BackFace.StencilFailOp       =  D3D10_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp  =  D3D10_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp       =  D3D10_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc         =  D3D10_COMPARISON_ALWAYS;

		dgi->Device( ).CreateDepthStencilState(&dsDesc,  &this->dss_Def3D);
		dsDesc.DepthEnable  =  FALSE;
		dgi->Device( ).CreateDepthStencilState(&dsDesc,  &this->dss_Def3D_Dis);
		dsDesc.DepthEnable  =  TRUE;
		dsDesc.DepthWriteMask  =  D3D10_DEPTH_WRITE_MASK_ZERO;
		dgi->Device( ).CreateDepthStencilState(&dsDesc,  &this->dss_Def3D_WriteDis);

		dsDesc.DepthEnable  =  FALSE;
		dsDesc.DepthWriteMask  =  D3D10_DEPTH_WRITE_MASK_ALL;
		dgi->Device( ).CreateDepthStencilState(&dsDesc,  &this->dss_Def2D);
		this->param.dss_Active  =  this->dss_Def3D;

		this->prevParam  =  this->param;
	}
	EffectState::~EffectState( )
	{
		ML::SafeRelease(&dss_Def3D_WriteDis);
		ML::SafeRelease(&dss_Def3D_Dis);
		ML::SafeRelease(&dss_Def3D);
		ML::SafeRelease(&dss_Def2D);
		ML::SafeRelease(&rs_Def3D);
		ML::SafeRelease(&rs_Def2D);
		ML::SafeRelease(&bs_Std);
		ML::SafeRelease(&bs_Alpha);
		ML::SafeRelease(&bs_Add);
		ML::SafeRelease(&bs_Dec);
	}

	////パラメータを全て再設定する
	//void  EffectState::Set(EffectParam&  p_)
	//{
	//	this->param.matWorld		= p_.matWorld;
	//	this->param.matView			= p_.matView;
	//	this->param.matProjection	= p_.matProjection;

	//	this->param.objectColor		= p_.objectColor;
	//	this->param.bgColor			= p_.bgColor;

	//	this->param.mateDiffuse   = p_.mateDiffuse;
	//	this->param.mateSpeculer  = p_.mateSpeculer;
	//	this->param.mateAmbient   = p_.mateAmbient;
	//	this->param.mateEmissive  = p_.mateEmissive;

	//	this->param.lightsEnable  = p_.lightsEnable;
	//	this->param.lightAmbient  = p_.lightAmbient;
	//	this->param.light[0] = p_.light[0];
	//	this->param.light[1] = p_.light[1];
	//	this->param.light[2] = p_.light[2];
	//	this->param.light[3] = p_.light[3];

	//	this->param.fogNear  = p_.fogNear;
	//	this->param.fogFore  = p_.fogFore;
	//	this->param.fogEnable  = p_.fogEnable;
	//	this->param.fogMode    = p_.fogMode;
	//	this->param.fogColor   = p_.fogColor;

	//	this->BS_Set(p_.bs_Active);
	//	this->RS_Set(p_.rs_Active);
	//	this->DSS_Set(p_.dss_Active);
	//}


	//ビューポート更新
	void  EffectState::ViewPort(const  D3D10_VIEWPORT&  vp_)
	{
		auto  dgi = DG::DGObject::GetInst();	if (dgi == 0) { return; }
		this->viewPort = vp_;
		dgi->Device().RSSetViewports(1, &vp_);
	}	//アルファブレンド
	void  EffectState::BS_Set(ID3D10BlendState*	bs_)
	{
		param.bs_Active = bs_;
		float blendFactor[4] = {0};
		UINT sampleMask   = 0xffffffff;
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return;  }
		dgi->Device( ).OMSetBlendState(param.bs_Active, blendFactor, sampleMask);
	}
	ID3D10BlendState*  EffectState::BS_Get( )
	{
		return  param.bs_Active;	
	}

	//アルファブレンド無効
	void  EffectState::BS_Std( ){		BS_Set(this->bs_Std);	}
	//アルファブレンド有効
	void  EffectState::BS_Alpha( ){		BS_Set(this->bs_Alpha);	}
	//アルファブレンド加算合成
	void  EffectState::BS_AlphaAdd( ){	BS_Set(this->bs_Add);	}
	//アルファブレンド減算合成
	void  EffectState::BS_AlphaDec( ){	BS_Set(this->bs_Dec);	}

	//ラスタライザ
	void  EffectState::RS_Set(ID3D10RasterizerState*  rs_)
	{
		param.rs_Active = rs_;
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return;  }
		dgi->Device( ).RSSetState(param.rs_Active);
	}
	ID3D10RasterizerState*  EffectState::RS_Get( ){
		return  param.rs_Active;	
	}

	//ラスタライザ３Ｄデフォルト
	void  EffectState::RS_Def3D( ){		RS_Set(this->rs_Def3D);}
	//ラスタライザ２Ｄデフォルト
	void  EffectState::RS_Def2D( ){		RS_Set(this->rs_Def2D);}

	//深度・ステンレス
	void  EffectState::DSS_Set(ID3D10DepthStencilState*  dss_)
	{
		param.dss_Active = dss_;
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return;  }
		dgi->Device( ).OMSetDepthStencilState(param.dss_Active, 0);
	}
	ID3D10DepthStencilState*  EffectState::DSS_Get( )
	{
		return  param.dss_Active;	
	}

	//深度ステンシル3Dデフォルト
	void  EffectState::DSS_Def3D( ){		DSS_Set(this->dss_Def3D);}
	void  EffectState::DSS_Def3D_Dis( ){	DSS_Set(this->dss_Def3D_Dis);}
	void  EffectState::DSS_Def3D_WriteDis( ){	DSS_Set(this->dss_Def3D_WriteDis);}
	//深度ステンシル2Dデフォルト
	void  EffectState::DSS_Def2D( ){		DSS_Set(this->dss_Def2D);}

//-----------------------------------------------------------------------------------
	bool  sf_Bool4(const  string&  name_,  BOOL  a_, BOOL  b_, BOOL  c_, BOOL  d_)
	{
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return  false; }
		BOOL  bo[4] = {a_,b_,c_,d_};

		ID3D10EffectVectorVariable*  evv;
		if(evv = dgi->Effect( ).GetVariableByName( name_.c_str( ) )->AsVector( ))
		{
			return  S_OK == evv->SetBoolVector(bo);
		}
		return  false;
	}

//-----------------------------------------------------------------------------------
	bool  sf_Vec4(const  string&  name_,  const  ML::Vec4&  m_)
	{
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return  false; }
		ID3D10EffectVectorVariable*  evv;
		if(evv = dgi->Effect( ).GetVariableByName( name_.c_str( ) )->AsVector( ))
		{
			return  S_OK == evv->SetFloatVector((float*)&m_);
		}
		return  false;
	}
	bool  sf_Vec4(const  string&  name_,  const  ML::Color&  m_)
	{
		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return  false; }
		ID3D10EffectVectorVariable*  evv;
		if(evv = dgi->Effect( ).GetVariableByName( name_.c_str( ) )->AsVector( ))
		{
			//evv->SetBoolVector(
			return  S_OK == evv->SetFloatVector((float*)&m_);
		}
		return  false;
	}
//-----------------------------------------------------------------------------------
	//bool  sf_Matrix(const  string&  name_,  const  ML::Mat4x4&  m_)
	//{
	//	auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return  false; }
	//	ID3D10EffectMatrixVariable*  emv;
	//	if(emv = dgi->Effect( ).GetVariableByName( name_.c_str( ) )->AsMatrix())
	//	{
	//		return  S_OK == emv->SetMatrix((float*)&m_);
	//	}
	//	return  false;
	//}



	//		//エフェクト更新
	//		auto  dgi = DG::DGObject::GetInst( );	if(dgi == 0){	return; }
	//		ID3D10EffectMatrixVariable*  emv;
	//		if(emv = dgi->Effect( ).GetVariableByName("matView")->AsMatrix())
	//			emv->SetMatrix((float*)&data.matView);
	//		if(emv = dgi->Effect( ).GetVariableByName("matProj")->AsMatrix())
	//			emv->SetMatrix((float*)&data.matProjection);
	//		if(emv = dgi->Effect( ).GetVariableByName("matViewProj")->AsMatrix())
	//			emv->SetMatrix((float*)&data.matViewProjection);

	//		ID3D10EffectVectorVariable*  evv;
	//		if(evv = dgi->Effect( ).GetVariableByName("ambient")->AsVector())
	//			evv->SetFloatVector((float*)&data.ambient);

	//		//data.lights
	//	}
	//}
	//-----------------------------------------------------------------------
}