#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

enum class GamePhase
{
	p1Entrance,
	p2Entrance,
	Round1,
	Round2,
	FinalRound,
	Fight,
	Battle,
	KO,
	DoubleKO,
	TimeOver,
	RoundTransition,
	BlankGap,
};

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
		DG::Image::SP	imgHPDelay;
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
		DG::Image::SP	imgTimeFont;
		DG::Image::SP   imgEmptyScore;
		DG::Image::SP	imgScore;
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
		
		int p1Round, p2Round;
		int p1DisplayRound, p2DisplayRound;
		bool isGameEnd = false;
		bool p1RoundWin, p2RoundWin, drawRound;

		DG::Font::SP TestFont;
		int battleTime;
		int gameEndTime;

		int hitStopCnt = 0;
		float p1LastX = 0.0f;
		float p2LastX = 0.0f;
		bool hasLastPos = false;


		GamePhase gamePhase;
		int phaseTimer;

		//Magic Number
		const int KO_EFFECT_TIME = 90;
		const int KO_NEXT_ROUND_TIME = 130;
		const int ROUND_EFFECT_TIME = 120;
		const int FIGHT_EFFECT_TIME = 60;
		const int ROUND_TRANSIT_EFFECT_TIME = 90;
		const int KO_O_DRAW_TIME = 16;
		ML::Box2D draw_Score[4]
		{
			ML::Box2D(536, 32, 48, 48),
			ML::Box2D(488, 32, 48, 48),
			ML::Box2D(697, 32, 48, 48),
			ML::Box2D(745, 32, 48, 48),
		};


		void UI_Render();
		void EmptyScore_Render();
		void Score_Render();
		void number_Render(int x, int y, int t);

		void phaseChange();
		bool canPlayerMove();
		void nextRoundInitialize();
		void startHitStop(int frame);
		bool isHitStop() const;
	};
}