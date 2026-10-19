#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  EffectManager
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("本編");	//グループ名
	const  string  defName("EffectManager");	//タスク名
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
		DG::Image::SP imgHitEffect;
		DG::Image::SP imgGuardEffect;
		DG::Image::SP imgP1HadouEffect;
		DG::Image::SP imgP2HadouEffect;
		DG::Image::SP imgScoreEffect;
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
		//追加したい変数・メソッドはここに追加する
		 
		struct EffectInfo
		{
			int animCnt = 0;
			ML::Vec2 pos;
			bool isPlaying = false;
			bool isFaceRight;

			void playEffect(ML::Box2D pos, bool isFaceRight);
			void playEffect(ML::Vec2 pos, bool isFaceRight);
			void updateEffect(int speed, int picCnt);
		};
		
		EffectInfo hitEffect;
		EffectInfo guardEffect;
		EffectInfo p1FBEffect, p2FBEffect;
		EffectInfo scoreEffect;

		struct DrawInfo
		{
			ML::Box2D draw, src;
		};

		//Animation
		DrawInfo hitEffectAnim();
		DrawInfo guardEffectAnim();
		DrawInfo p1FbEffectAnim();
		DrawInfo p2FbEffectAnim();
		DrawInfo scoreEffectAnim();

		void effectDirection(Object::DrawInfo& rtv, bool isFaceRight);
	};
}