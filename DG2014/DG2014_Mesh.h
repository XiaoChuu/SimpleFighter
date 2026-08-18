#pragma warning(disable:4996)
#pragma once
#include "DG2014_Ver3_6.h"

namespace DG
{
	//マテリアルコンテナ
	class  Material{
	public:
		string				name;		//マテリアル名
		ML::Color			diffuse;	//マテリアル情報:デフューズ
		ML::Color			ambient;	//マテリアル情報:アンビエント
		ML::Color			speculer;	//マテリアル情報:スペキュラ
		ML::Color			emissive;	//マテリアル情報:エミッシブ
		Texture::SP			tex;		//テクスチャ
	};
	//同時にレンダリングする部位
	class  MeshGroup{
	public:
		DWORD				mcNumber;	//対応するマテリアル番号
		IndexBuffer::SP		index;		//インデックス情報
		MeshGroup( ):mcNumber(0){}
	};
	//メッシュ
	class  Mesh
	{
	//-------------------------------------------------------------------
	public:
		using SP = shared_ptr<Mesh>;
		using WP = weak_ptr<Mesh>;
	//-------------------------------------------------------------------
	private:
		static  unordered_map<string,  WP>   rep;
	//-------------------------------------------------------------------
	private:
		BOOL					normalEnable;	//法線ある？
		BOOL					uvEnable;		//UVある？
		DG::VB_Container::SP	vbc;			//バーテックスバッファコンテナ
		vector<Material>		mateConts;		//マテリアル
		vector<MeshGroup>		meshGroups;		//？
		InputLayout::SP			inputLayout;	//頂点情報の入力レイアウト
		ML::Collsion::AABB		bb;
		//-------------------------------------------------------------------
	private:
		Mesh( );
	public:
		~Mesh( );
	//-------------------------------------------------------------------
	public:
		string					tecName;		//レンダリングに使用するテクニック名
		//	メッシュを生成する(データを自前で用意する仮処理的なモノ）
		static  SP  Create(
				Material&				mc_,
				VB_Container::SP&		vbc_,//バーテックスバッファコンテナ
				Texture::SP&			tex_);//テクスチャ
	private:
		//　メッシュを生成する
		bool  Initialize(
				Material&				mc_,
				VB_Container::SP&		vbc_,	//バーテックスバッファコンテナ
				Texture::SP&			tex_);	//テクスチャ			
	//-------------------------------------------------------------------
	public:
		//SOBファイルからメッシュを生成する
		static  SP  Create_FromSOBFile(const  string&  fpath_);
		//意図的に破棄する
		static void Destroy(SP& sp_);
	private:
		//SOBファイルからメッシュを生成する
		static  SP  Create_FromSOBFileSub(const  string&  fpath_);
		//SOBファイルからメッシュを生成する
		bool  Initialize_FromSOBFile(const  string&  fpath_);
	//-------------------------------------------------------------------
	public:
		//Xファイルからメッシュを生成する(未実装
		static  SP  Create_FromXFile(const  string&  fpath_);
	private:
		//Xファイルからメッシュを生成する(未実装
		bool  Initialize_FromXFile(const  string&  fpath_);
	//-------------------------------------------------------------------
	public:
		//	メッシュを表示する
		void Draw(	);
	//-------------------------------------------------------------------
	public:
		//	メッシュとレイの接触判定を行う（例はメッシュのローカル座標系）
		bool  Hit_Ray(const  ML::Vec3&  pos_,  const  ML::Vec3&  dir_,  float&  dist_,  bool  qmode_=false);
	//-------------------------------------------------------------------
	private:
		//ＢＢとＢＳを生成する
		void  InitializeBB_BS( );
	//-------------------------------------------------------------------
	public:
		//	バウンディングボックスとレイの接触判定を行う（レイはメッシュのローカル座標系）
		bool  Hit_RayBB(const  ML::Vec3&  pos_,  const  ML::Vec3&  dir_,  float&  dist_);
	//-------------------------------------------------------------------
	public:
		//マテリアルを強制的に書き換える（使用は非推奨）
		void  UpDateMaterial(
			const ML::Color&  dif_,
			const ML::Color&  spe_,
			const ML::Color&  amb_,
			const ML::Color&  emi_);
	};
}
