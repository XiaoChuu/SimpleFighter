#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

namespace  Game
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("本編");	//グループ名
	const  string  defName("統括");	//タスク名
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
		DG::Image::SP	imgHPFull;
		DG::Image::SP	imgHPEmp;
		DG::Image::SP	imgGaugeFull;
		DG::Image::SP	imgGaugeEmp;
		DG::Image::SP	imgGaugeFullR;
		DG::Image::SP	imgGaugeEmpR;
		DG::Image::SP	imgTimebar;
		DG::Image::SP	imgBG;
		DG::Image::SP	imgPowerUpFull;
		DG::Image::SP	imgPowerUpEmp;
		DG::Image::SP	imgHadoukenFull;
		DG::Image::SP	imgHadoukenEmp;
		DG::Image::SP	imgShoryukenFull;
		DG::Image::SP	imgShoryukenEmp;
		DG::Image::SP	imgPlayer1Ava;
		DG::Image::SP	imgPlayer2Ava;
		DG::Image::SP	imgKO;
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
		void  UpDate()			override;	//「実行」１フレーム毎に行う処理
		void  Render2D_AF()		override;	//「2D描画」１フレーム毎に行う処理
		bool  Finalize();	//「終了」タスク消滅時に１回だけ行う処理
		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		//追加したい変数・メソッドはここに追加する
		
		int p1Round;
		int p2Round;
		bool isGameEnd = false;

		DG::Font::SP TestFont;
		int time;
		int gameEndTime;

		float push = 6.5f;
	};
}