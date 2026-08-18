#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//エフェクト00
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  Effect00
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("effect");	//グループ名
	const  string  defName("effect");	//タスク名
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()	override;
		bool  Finalize()	override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//共有する変数はここに追加する
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate()			override;//「実行」１フレーム毎に行う処理
		void  Render2D_AF()		override;//「2D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		DG::Image::SP  img;

		//追加したい変数・メソッドはここに追加する
		ML::Vec2	pos;		//キャラクタ位置
		ML::Vec2	moveVec;	//移動方向ベクトル
		ML::Vec2	graVec;		//重力各方向に準ずる
		int			resSizeX, resSizeY;//エフェクト画像一コマサイズ
		int			drawSizeX,drawSizeY;//エフェクト描画一コマサイズ

		float		scale;		//拡大率
		float		addScale;	//拡大率加算

		float		angle;		//向き
		float		addAngle;	//角度加算量

		bool		AddRender;	//加算合成フラグ
		float		alpha;		//半透明
		float		addAlpha;	//加算量

		int			count;		//フレームカウント
		int			countMax;	//消滅フレーム

		float		animno;		//アニメーションコマ数
		float		addAnim;	//アニメーション加算
		int			animMax;	//終了到達コマ
		int			animResNum;	//リソースの折り返し数

		bool fade;

		std::string filename;
	};
}