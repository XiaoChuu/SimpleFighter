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
	struct  Light{
		BOOL		enable;
		enum class Kind : UINT{Directional,  Point,  Spot, Unnon = 0xFFFFFFFF};
		Kind		kind;		//0ディレクショナル　1ポイント	2スポット
		ML::Vec3	direction;	//ライト照射方向（ディレクショナル・スポット）
		ML::Color	color;		//ライトの色兼強さ
		ML::Vec3	pos;		//ライトの位置（追加！）
		float		attenuation;//減衰率（追加！）
		float		range;		//有効距離（追加！）
		bool  operator==(const  Light&  obj){
			return	   (enable	  == obj.enable &&
						kind	  == obj.kind &&
						direction == obj.direction &&
						pos       == obj.pos &&				//（追加！）
						attenuation  == obj.attenuation &&	//（追加！）
						range  == obj.range &&				//（追加！）
						color	  == obj.color);
		}
		bool  operator!=(const  Light&  obj){
			return !((*this) == obj);
		}

	};
	class EffectParam
	{
	public:
		EffectParam( );
		//行列
		ML::Mat4x4	matWorld;
		ML::Mat4x4	matView;
		ML::Mat4x4	matProjection;
		ML::Vec3	eyePos;		//視点座標(ライティングなどで使用）
		//マテリアル
		ML::Color	mateDiffuse;	//マテリアル情報:デフューズ
		ML::Color	mateSpeculer;	//マテリアル情報:スペキュラ
		ML::Color	mateAmbient;	//マテリアル情報:アンビエント
		ML::Color	mateEmissive;	//マテリアル情報:エミッシブ
		ML::Color	objectColor;	//モデルの色を強制的に変化させる
		//フォグ
		bool		fogEnable;	//フォグの有効・無効
		bool		fogMode;	//通常フォグor透過
		float		fogNear;	//フォグ開始地点
		float		fogFore;	//フォグ最大化地点
		ML::Color	fogColor;	//フォグの色
		//ライト
		bool		lightsEnable;	//ライトの有効無効（元栓）
		ML::Color	lightAmbient;	//環境光
		Light		light[4];
		//微妙な状態
		ML::Color	bgColor;		//背景色
		//コンスタントバッファ以外		
		ID3D10BlendState*			bs_Active;		//解放不要
		ID3D10RasterizerState*		rs_Active;		//解放不要
		ID3D10DepthStencilState*	dss_Active;		//解放不要
	};
	class EffectState
	{
		ID3D10BlendState*	bs_Std;		//アルファなし通常描画
		ID3D10BlendState*	bs_Alpha;	//アルファあり
		ID3D10BlendState*	bs_Add;		//加算合成
		ID3D10BlendState*	bs_Dec;		//減算合成

		ID3D10RasterizerState*  rs_Def3D;		//ラスタライザ　３Ｄデフォルト
		ID3D10RasterizerState*  rs_Def2D;		//ラスタライザ　２Ｄデフォルト

		ID3D10DepthStencilState*  dss_Def3D;			//深度・ステンシル　３Ｄデフォルト
		ID3D10DepthStencilState*  dss_Def3D_Dis;		//深度・ステンシル　３Ｄデフォルト（深度無効）
		ID3D10DepthStencilState*  dss_Def3D_WriteDis;	//深度・ステンシル　３Ｄデフォルト（深度書き込み無効）
		ID3D10DepthStencilState*  dss_Def2D;			//深度・ステンシル　２Ｄデフォルト

		EffectParam		prevParam;
	public:
		D3D10_VIEWPORT  viewPort;
		EffectParam		param;
//		void  Set(EffectParam&  p_);	//パラメータを全て再設定する
		EffectState( );
		~EffectState( );
		//Light


		//ビューポート
		void  ViewPort(const  D3D10_VIEWPORT&  vp_);//ビューポート更新
		//ブレンドステート
		void  BS_Set(ID3D10BlendState*  bs_);
		ID3D10BlendState*  BS_Get( );
		void  BS_Std( );		//アルファブレンド無効
		void  BS_Alpha( );		//アルファブレンド有効
		void  BS_AlphaAdd( );	//アルファブレンド加算合成
		void  BS_AlphaDec( );	//アルファブレンド減算合成
		//ラスタライザ
		void  RS_Set(ID3D10RasterizerState*  rs_);
		ID3D10RasterizerState*  RS_Get( );
		void  RS_Def3D( );	//ラスタライザ３Ｄデフォルト
		void  RS_Def2D( );	//ラスタライザ２Ｄデフォルト
		//深度・ステンシル
		void  DSS_Set(ID3D10DepthStencilState*  dss_);
		ID3D10DepthStencilState*  DSS_Get( );
		void  DSS_Def3D( );	//深度・ステンシル　３Ｄデフォルト
		void  DSS_Def3D_Dis( );	//深度・ステンシル　３Ｄデフォルト(深度無効）
		void  DSS_Def3D_WriteDis( );	//深度・ステンシル　３Ｄデフォルト(深度書き込み無効）
		void  DSS_Def2D( );	//深度・ステンシル　２Ｄデフォルト

		//レンダリングする前に呼び出すparamに含まれる設定をレンダリングに反映させる
		void  Update_Params( );
	private:
		void  Update_Fog( );
		void  Update_Lights( );
		void  Update_Matrixs( );
		void  Update_Material( );
		void  Update_Etc( );
	};
}