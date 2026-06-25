//--------------------------------------------------------------------------------------
//Light
struct   sLightParam
{
	bool		enable;
	int			kind;			//0ディレクショナル　1ポイント	2スポット
	float		attenuation;	//減衰（追加！）
	float		range;			//有効距離（追加！）
	float4		direction;		//ライト照射方向
	float4		directionInv;	//ライト照射方向(逆ベクトル）
	float4		pos;			//光源の位置（追加！）
	float4		color;			//ライトの色兼強さ
};
struct   sLightsParam
{
	bool		enable;
	bool		n0;			//サイズ調整用
	bool		n1;			//サイズ調整用
	bool		n2;			//サイズ調整用
	float4		ambient;
	sLightParam	light[4];
};
cbuffer  LightsCB{
	sLightsParam	 lights;
};
//--------------------------------------------------------------------------------------
//Fog
struct  sFogParam
{
	bool	enable;
	bool	mode;
	bool	n0;
	bool	n1;
	float	near;
	float	fore;
	float	p0;
	float	p1;
	float4	color;
};
cbuffer  FogCB
{
	sFogParam	fog;
};
//--------------------------------------------------------------------------------------
//Matrix
struct  sMatrixParam
{
	matrix		world;
	matrix		view;
	matrix		projection;

	matrix		w_v;
	matrix		w_v_p;
	matrix		v_p;
};
cbuffer  MatrixCB
{
	sMatrixParam		mat;
};
//--------------------------------------------------------------------------------------
//Material
struct  sMaterialParam
{
	float4		diffuse;
	float4		speculer;
	float4		ambient;
	float4		emissive;
	float4		objectColor;	//強制色変更
};
cbuffer  MaterialCB
{
	sMaterialParam	mate;
};
//--------------------------------------------------------------------------------------
//未分類
struct  sEtcParam
{
	float4		eyePos;				//視点座標
};
cbuffer  EtcCB
{
	sEtcParam	etc;
};
//--------------------------------------------------------------------------------------
//テクスチャ
Texture2D	tex0, tex1, tex2, tex3, tex4, tex5, tex6, tex7;

//--------------------------------------------------------------------------------------
//サンプラ
//線形補間　UVミラーリング
SamplerState  samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;	//Wrap;
    AddressV = MIRROR;	//Wrap;
};
//▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼
//シェーダ―0
//------------------------------------------------------------------
//鏡面反射した光線の逆ベクトルを求める
float4  SpecularReflectInv(float4  toLight_,  float4  n_)//鏡面反射
{
	float  d =  dot(n_, toLight_);
	return  toLight_ + (n_ * (-d * 2));
}
//------------------------------------------------------------------
//ライティング
float4  Lighting(float4  pos_, float4  normal_,  sLightParam  lp_)
{
	float4  color  =  (float4)0;
	//ディレクショナル（平行光源）
	if(lp_.kind  ==  0){
		//拡散反射に対する光量計算
		{	//入射角による光量を求める
			float  angleL  =  max(0, dot(normal_,  lp_.directionInv));
			//反射光を求める
			color  +=  lp_.color  *  mate.diffuse  *  angleL;
		}
		//鏡面反射に対する光量計算(動作確認の為の改変版）
		{	//視線ベクトルを求める
			float4  eyeVec  =  normalize(pos_  -  etc.eyePos);
			//反射光の逆ベクトルを求める
			float4	refInv  =  SpecularReflectInv(lp_.directionInv,  normal_);
			//反射光と視線による光量を求める
			float  angleL  =  max(0,  dot(eyeVec,  refInv));
			//反射光を求める
			color  +=  lp_.color  *  mate.speculer  *  angleL;
		}
	//ポイント
	}else  if(lp_.kind  ==  1){
		//入射角と逆向きのベクトル（光源に向かうベクトル）を求める
		float4  toLight  =  lp_.pos  -  pos_;
		//光源までの距離を求める
		float  len  =  length(toLight);
		//有効距離内だけライティング処理を行う
		if(lp_.range  >=  len){
			//距離による光の量を求める（距離の２乗で光が弱くなる）
			float  distL  =  1 / max(1, len  *  len  *  lp_.attenuation);
			//光源に向かう単位ベクトルを求める
			float4  toLightNormal  =  toLight  /  len;
			//拡散反射に対する光量計算
			{	//角度による反射光の量を求める
				//float  angleL  =  1.0f;		//（未実装）
				float  angleL  =  max(0, dot(normal_,  toLightNormal));
				//拡散反射に対する光量計算
				color += lp_.color  *  mate.diffuse  *  angleL  *  distL;
			}
			//鏡面反射に対する光量計算
			{	//視線ベクトルを求める
				float4  eyeVec  =  normalize(pos_  -  etc.eyePos);
				//反射光の逆ベクトルを求める
				float4	refInv  =  SpecularReflectInv(toLightNormal,  normal_);
				//反射光と視線による光量を求める
				float  angleL  =  max(0,  dot(eyeVec,  refInv));
				//反射光を求める
				color  +=  lp_.color  *  mate.speculer  *  angleL  *  distL;
			}
		}
	}else{
	}
	return  color;
}
//------------------------------------------------------------------
//バーテックスシェーダ出力構造体　　＊シェーダではなくバッファになっているのは記述ミス
struct sVS_PNT_Out
{	
    float4  pos			: SV_POSITION;
	float2  uv			: TEXCOORD0;
	float	viewZ		: POSITION0;
	float4  posW		: POSITION1;
	float4  norW		: NORMAL;
	//float4  color		: COLOR;
};
//------------------------------------------------------------------
//
//バーテックスシェーダー
//
//頂点　法線　UVの構成の頂点に対する処理
//
sVS_PNT_Out  VS_PNT(	float3  pos_		: POSITION,
						float3  normal_		: NORMAL,
						float2  uv_			: TEXCOORD)
{
    sVS_PNT_Out output = (sVS_PNT_Out)0;

//　処理的には無駄だが流れが追える形で記述している

	//頂点と法線をワールド変換
	float4  posW  =  mul( float4(pos_.xyz,1), mat.world );
	float4  norW  =  mul( float4(normal_.xyz,0), mat.world );
	norW = normalize(norW);	//法線は正規化

	////ライティング処理(未実装）
	//float4  color  =  float4(0,0,0,0);
	//if(lights.enable){
	//	for(int  i = 0;  i < 4;  i++){
	//		if(lights.light[i].enable){
	//			//拡散反射・鏡面反射
	//			color += Lighting(posW,  norW,  lights.light[i]);
	//		}
	//	}
	//	//環境光反射量を適用
	//	color += mate.ambient  *  lights.ambient;
	//	//透明度はマテリアルのデフューズから得る
	//	color.a  =  mate.diffuse.a;
	//}
	//else{
	//	//ライト無効時
	//	color += mate.diffuse;
	//}
	////自己発光量を適用
	//color  +=  mate.emissive;
	////オブジェクトカラー（強制色変更）を適用
	//color  *=  mate.objectColor;

	//頂点をビュー変換
	float4  posV  =  mul( posW, mat.view );
	//頂点をプロジェクション変換
	float4  posP  =  mul( posV, mat.projection );

	//バーテックスバッファの出力情報を設定
    output.pos    = posP;
    output.uv     = uv_;
	output.viewZ  = posV.z;
	output.posW = posW;
	output.norW = norW;
	//output.color  = color;
    return output;
}
//------------------------------------------------------------------
////フォグ
float4  Fog(float  viewZ_,  float4  color_)
{	
	//near以下を0とする
	float  d  =  max(viewZ_ - fog.near,  0);
	//near～foreの間で濃度を調整する
	float  m  =  min(  d / (fog.fore - fog.near), 1);

	float4  rtv;
	if(fog.mode){
		rtv  =  fog.color  *  m  +  color_  *  (1 - m);
	}
	else{
		rtv  =  float4(color_.r, color_.g, color_.b, color_.a * (1 - m));
	}
	return  rtv;
}
//------------------------------------------------------------------
//
//ピクセルシェーダー
//
//頂点　法線　UVの構成の頂点に対する処理
//
float4 PS_PNT( sVS_PNT_Out  ip_ ) : SV_Target
{
	//ライティング処理
	float4  color = float4(0, 0, 0, 0);
	if (lights.enable){
		for (int i = 0; i < 4; i++){
			if (lights.light[i].enable){
				//拡散反射・鏡面反射
				color += Lighting(ip_.posW, ip_.norW, lights.light[i]);
			}
		}
		//環境光反射量を適用
		color += mate.ambient  *  lights.ambient;
		//透明度はマテリアルのデフューズから得る
		color.a = mate.diffuse.a;
	}
	else{
		//ライト無効時
		color += mate.diffuse;
	}
	//自己発光量を適用
	color.xyz += mate.emissive.xyz;
	//オブジェクトカラー（強制色変更）を適用
	color *= mate.objectColor;

	//テクスチャからピクセルを取得
	color *= tex0.Sample( samLinear, ip_.uv);
	//フォグを反映させる
	if(fog.enable){
		color  =  Fog(ip_.viewZ,  color);
	}
	return  color;
}
//▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲
//▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼
//シェーダ―1
//------------------------------------------------------------------
//バーテックスバッファー出力構造体
struct sVS_PCT_Out
{	
    float4  pos			: SV_POSITION;
	float4	color		: COLOR;
	float2  uv			: TEXCOORD0;
	float	viewZ		: POSITION;
};
//------------------------------------------------------------------
//
//バーテックスシェーダー
//
//頂点　カラー　UVの構成の頂点に対する処理
sVS_PCT_Out VS_PCT(		float3  pos_		: POSITION,
						float4  col_		: COLOR,
						float2  uv_			: TEXCOORD)
{
	sVS_PCT_Out output = (sVS_PCT_Out)0;
	output.pos = mul( float4(pos_.xyz,1), mat.w_v_p );
	output.color = mate.objectColor * col_;
	output.uv  = uv_;
	output.viewZ  = 0;
	return output;
}


//------------------------------------------------------------------
//
//ピクセルシェーダー
//
//頂点　色　UVの構成の頂点に対する処理
//
float4 PS_PCT( sVS_PCT_Out  ip_ ) : SV_Target
{
	float4  col = ip_.color;
	col *= tex0.Sample( samLinear, ip_.uv);
	return  col;
}
//▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲

//▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼
//シェーダ―2
//レイ描画用
//バーテックスバッファー出力構造体
struct sVS_Ray_Out
{
	float4  pos			: SV_POSITION;
};
//------------------------------------------------------------------
//
//バーテックスシェーダー
//
//頂点座標で構成された頂点に対する処理
//
sVS_Ray_Out VS_Ray(float3  pos_		: POSITION)
{
	sVS_Ray_Out output = (sVS_Ray_Out)0;
	float4  posW = mul(float4(pos_.xyz, 1), mat.world);
	float4  posV = mul(posW, mat.view);
	float4  posP = mul(posV, mat.projection);

	output.pos = posP;
	return output;
}
//------------------------------------------------------------------
//
//ピクセルシェーダー
//
//頂点座標で構成されたポリゴンのピクセルに対する処理
//
float4 PS_Ray(sVS_Ray_Out  ip_) : SV_Target
{
	return  mate.objectColor;
}
//▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲
//☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
//課題によりシェーダープログラムを作成する場合、ここより下に記述する
//☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
//▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲
//☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
//課題によりシェーダープログラムを作成する場合、ここより下に記述する
//☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆




//☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
//▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼
//テクニック(シェーダはここより上に書かないと動かないよ)
//
technique10 MyStdShaderTec
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS_PNT()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS_PNT()));
	}
	pass P1
	{
		SetVertexShader(CompileShader(vs_4_0, VS_PCT()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS_PCT()));
	}
	pass P2
	{
		SetVertexShader(CompileShader(vs_4_0, VS_Ray()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS_Ray()));
	}
	//☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
	//課題によりテクニックを追加する場合、ここより下に追加する
	//☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆


	//☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆☆
}

