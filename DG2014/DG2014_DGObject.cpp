#include "DG2014_Ver3_6.h"
//#include "atlstr.h"


//┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//┃画像制御系機能群																		　┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
namespace DG
{
	DGObject::WP DGObject::winst;	//	１つしか存在できないインスタンス

	//------------------------------------------------------------
	DGObject::DGObject( )
	{
		this->effectState = nullptr;
		this->sprite = nullptr;
		this->effect = nullptr;
		this->renderTargetView = nullptr;
		this->depthView = nullptr;
		this->depthBuffer = nullptr;
		this->swapChain = nullptr;
		this->d3dDevice = nullptr;
	}
	//------------------------------------------------------------
	DGObject::~DGObject( )
	{
		ML::SafeDelete(&this->effectState);
		ML::SafeRelease(&this->sprite);
		ML::SafeRelease(&this->effect);
		//
		ML::SafeRelease(&this->renderTargetView);
		ML::SafeRelease(&this->depthView);
		ML::SafeRelease(&this->depthBuffer);
		ML::SafeRelease(&this->swapChain);
		ML::SafeRelease(&this->d3dDevice);
	}
	//------------------------------------------------------------
	ID3D10Device&				DGObject::Device( ){			return  *this->d3dDevice;		}	//	デバイスオブジェクト
	IDXGISwapChain&				DGObject::SChain( ){		return  *this->swapChain;		}	//スワップチェイン
	ID3D10RenderTargetView&		DGObject::RTView( ){		return  *this->renderTargetView;	}	//レンダーターゲットビュー
	ID3D10EffectTechnique&		DGObject::ETechnique( ){	return  *this->effectTechnique;	}	
	ID3D10Effect&				DGObject::Effect( ){		return  *this->effect;			}	

	//------------------------------------------------------------
	bool DGObject::Initialize(
					HWND     hw_,
					int      w_,
					int      h_,
					DWORD    ms_, 
					bool     sm_,
					int      w2D_,
					int      h2D_)
	{
		screenWidth  =  w_;
		screenHeight  =  h_;

		// デバイスとスワップチェーンの作成
		DXGI_SWAP_CHAIN_DESC  sd;
		ZeroMemory( &sd, sizeof(sd) );
		sd.BufferCount = 1;
		sd.BufferDesc.Width = screenWidth;
		sd.BufferDesc.Height = screenHeight;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator	  = 60;		//リフレッシュレート（分母）
		sd.BufferDesc.RefreshRate.Denominator = 1;		//リフレッシュレート（分子）
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.OutputWindow = hw_;
		sd.SampleDesc.Count = ms_;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = !sm_;


		HRESULT hr = D3D10CreateDeviceAndSwapChain(
				NULL,	//ビデオカード選択しない
				D3D10_DRIVER_TYPE_HARDWARE,	//
				NULL,
				0,
				D3D10_SDK_VERSION,
				&sd,
				&this->swapChain,
				&this->d3dDevice);
		if(FAILED(hr)){ return false;}

		//バックバッファをレンダリングターゲットとするための処理
		ID3D10Texture2D* pBackBuffer;
		hr = this->swapChain->GetBuffer(0,  __uuidof(ID3D10Texture2D),  (LPVOID*)&pBackBuffer);
		if(FAILED(hr)){ return false;}

		hr = this->d3dDevice->CreateRenderTargetView(
				pBackBuffer,	//ビューを介してアクセスするリソース（バックバッファ→テクスチャ）
				NULL,			//ビュー定義
				&this->renderTargetView);	//生成したビュー
		ML::SafeRelease(&pBackBuffer);	//バックバッファへのアクセス件を解除
		if(FAILED(hr)){ return false;}
		this->d3dDevice->OMSetRenderTargets(1, &this->renderTargetView, NULL);

		//深度・ステンシルテクスチャの生成
		D3D10_TEXTURE2D_DESC  depthD;
		depthD.Width = screenWidth;		depthD.Height = screenHeight;
		depthD.MipLevels = 1;	depthD.ArraySize = 1;
//		depthD.Format = DXGI_FORMAT_D16_UNORM;
		depthD.Format = DXGI_FORMAT_D32_FLOAT;
		depthD.SampleDesc.Count = ms_;
		depthD.SampleDesc.Quality = 0;
		depthD.Usage = D3D10_USAGE_DEFAULT;
		depthD.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		depthD.CPUAccessFlags = 0;
		depthD.MiscFlags = 0;
		hr = this->d3dDevice->CreateTexture2D(&depthD, NULL, &this->depthBuffer);

		if(FAILED(hr)){ return false;}
		//深度・ステンシルビューの生成
		D3D10_DEPTH_STENCIL_VIEW_DESC  depthViewD;
		depthViewD.Format = depthD.Format;
		depthViewD.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DMS;
		depthViewD.Texture2D.MipSlice = 0;
		this->d3dDevice->CreateDepthStencilView(
			this->depthBuffer,
			&depthViewD,
			&this->depthView);
		if(FAILED(hr)){ return false;}

		//ビューの登録
		this->d3dDevice->OMSetRenderTargets(1,  &this->renderTargetView,  this->depthView);

		//ビューポート設定
		D3D10_VIEWPORT	vp[1];
		vp[0].TopLeftX = 0;	vp[0].TopLeftY = 0;
		vp[0].Width = screenWidth;		vp[0].Height = screenHeight;
		vp[0].MinDepth = 0.0f;	vp[0].MaxDepth = 1.0f;
		this->d3dDevice->RSSetViewports(_countof(vp), vp);

		//エフェクトの生成
		ID3D10Blob*		blobErr    = NULL;
		hr = D3DX10CreateEffectFromFile(
				"data/shader/MyStdShader.fx", NULL, NULL, "fx_4_0", 
				D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0,
				this->d3dDevice, NULL, NULL, &this->effect, &blobErr, NULL );
		if(FAILED(hr)){
			if(blobErr){
				char*  err  =  (char*)(blobErr->GetBufferPointer( ));
				MessageBox(NULL,  err,  "シェーダコンパイルエラー", MB_OK);
			}
			return false;
		}
		//テクニックを得る
		this->effectTechnique = this->effect->GetTechniqueByName( "MyStdShaderTec" );


		//ラスタライズ設定
		D3D10_RASTERIZER_DESC rdc;
		ZeroMemory(&rdc,sizeof(rdc));
		rdc.CullMode=D3D10_CULL_NONE;
		rdc.FillMode=D3D10_FILL_SOLID;
	
		ID3D10RasterizerState* pIr=NULL;
		this->d3dDevice->CreateRasterizerState(&rdc,&pIr);
		this->d3dDevice->RSSetState(pIr);


		//スプライト生成
		hr =  D3DX10CreateSprite(this->d3dDevice, spriteBufferMax, &this->sprite);
		if(FAILED(hr)){ return false;}

		return true;	
	}


	//------------------------------------------------------------
	//	DGライブラリを生成する
	DGObject::SP DGObject::Create(
							HWND        hw_,	//	ウィンドウハンドル
							int         w_,		//	横幅
							int         h_,		//	縦幅
							DWORD       ms_,	//	マルチサンプル
							bool        sm_,	//	スクリーンモード
							int         w2D_,
							int         h2D_)
	{
		if(winst.expired( )){
			if( DGObject::SP sp = DGObject::SP(new DGObject) ){
				if(sp->Initialize(hw_, w_, h_, ms_, sm_, w2D_, h2D_)){
					winst = sp;
					//
					sp->effectState = new DG::EffectState( );
					return sp;
				}
			}
		}
		return 0;
	}
	//------------------------------------------------------------
	DGObject::SP DGObject::GetInst( )
	{
		if(!winst.expired( )){
			return winst.lock( );
		}
		return 0;
	}
	//------------------------------------------------------------
	//	画面描画（開始）
	BOOL DGObject::Begin(const ML::Color&  c_)
	{
		//深度バッファを有効化
		this->d3dDevice->OMSetRenderTargets(1, &this->renderTargetView, this->depthView);
		Device( ).ClearRenderTargetView(this->renderTargetView, c_.Ptr( ));
	//	深度バッファのクリア
		Device( ).ClearDepthStencilView(this->depthView, D3D10_CLEAR_DEPTH, 1.0f, 0);
		return true;
	}
	//------------------------------------------------------------
	//	3D描画の許可を得る
	void DGObject::Begin3D( )
	{
		//深度バッファを有効化
		this->d3dDevice->OMSetRenderTargets(1, &this->renderTargetView, this->depthView);
		this->effectState->RS_Def3D( );
		this->effectState->DSS_Def3D( );
	}
	//------------------------------------------------------------
	//	3D描画を終了する
	void  DGObject::End3D( )
	{
	}
	//------------------------------------------------------------
	//	2D描画の許可を得る
	void  DGObject::Begin2D( )
	{
		Font::dt = false;
		this->effectState->RS_Def2D( );
		this->effectState->DSS_Def2D( );
		//spriteBuffer.clear( );
		//spriteBuffer.reserve(spriteBufferMax);
		//深度バッファを無効化
		this->d3dDevice->OMSetRenderTargets(1, &this->renderTargetView, NULL);
//		this->sprite->Begin(D3DX10_SPRITE_SORT_DEPTH_BACK_TO_FRONT);
//		this->sprite->Begin(D3DX10_SPRITE_SORT_DEPTH_FRONT_TO_BACK);
		this->sprite->Begin(0);
		

		ML::Mat4x4  p, v, s, t;
		s.Scaling( ML::Vec3(2.0f/screenWidth, 2.0f/screenHeight, 1));
		t.Translation( ML::Vec3(-(screenWidth/2.0f), screenHeight/2.0f, 0));
		p = t * s;
		//p.Identity( );
		v.Identity( );
		this->sprite->SetProjectionTransform(&p);
		this->sprite->SetViewTransform(&v);
	}
	//------------------------------------------------------------
	//	2D描画のを終了する
	void  DGObject::End2D( )
	{
		Flush2D( );
		this->sprite->End( );
	}
	//	2D描画、登録済みの画像を書き込む
	void DGObject::Flush2D( )
	{
		if(Image::dt == true){
			Image::dt = false;
			//フォント描画モードであった場合、スプライト表示モードに再設定
			if( Font::dt == true){
				Font::dt = false;
				this->effectState->RS_Def2D( );
				this->effectState->DSS_Def2D( );
				ML::Mat4x4  p, v, s, t;
				s.Scaling( ML::Vec3(2.0f/960.0f, 2.0f/544.0f, 1));
				t.Translation( ML::Vec3(-480, +272, 0));
				p = t * s;
				//p.Identity( );
				v.Identity( );
				this->sprite->SetProjectionTransform(&p);
				this->sprite->SetViewTransform(&v);
			}
			//現在バッファに登録されているデータを全て描画する
			this->sprite->Flush( );
			//this->sprite->DrawSpritesImmediate(&spriteBuffer.front( ),  s,  0, 0);
			//spriteBuffer.clear( );
			//spriteBuffer.reserve(spriteBufferMax);
		}
	}
	//------------------------------------------------------------
	//	画面描画（終了）
	void  DGObject::End( )
	{
		this->swapChain->Present(1, 0);
	}
	//------------------------------------------------------------
	//2D描画データをバッファへ登録
	void  DGObject::PushBack_D3DX10_SPRITE(D3DX10_SPRITE  &s_)
	{
//		spriteBuffer.push_back(s_);
		this->sprite->DrawSpritesBuffered(&s_, 1);
	}
	DWORD	DGObject::ScreenWidth( ){		return screenWidth;}
	DWORD	DGObject::ScreenHeight( ){		return screenHeight;}


	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//テンプレート実装
//	template bool DGObject::VSSet_ConstBuffer<CB_SheenInfo::CBuffer>(const CB_SheenInfo::CBuffer&);
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★


}












