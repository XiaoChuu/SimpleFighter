//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"
#include "randomLib.h"
#include  "Task_Ending.h"
#include  "Task_player1.h"
#include  "Task_player2.h"

bool onePWin;
bool twoPWin;

namespace  Game
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgHPFull = DG::Image::Create("./data/image/HPBAR_Full.png");
		this->imgHPEmp = DG::Image::Create("./data/image/HPBAR_Empty.png");
		this->imgGaugeFull = DG::Image::Create("./data/image/SUPER_BAR_Full.png");
		this->imgGaugeEmp = DG::Image::Create("./data/image/SUPER_BAR_Empty.png");
		this->imgGaugeFullR = DG::Image::Create("./data/image/SUPER_BAR_Full_R.png");
		this->imgGaugeEmpR = DG::Image::Create("./data/image/SUPER_BAR_Empty_R.png");
		this->imgTimebar = DG::Image::Create("./data/image/TIME_DP.png");
		this->imgBG = DG::Image::Create("./data/image/BG.png");
		this->imgPowerUpFull = DG::Image::Create("./data/image/Skill_Top.png");
		this->imgPowerUpEmp = DG::Image::Create("./data/image/Skill_Top_Grey.png");
		this->imgHadoukenFull = DG::Image::Create("./data/image/Skill_Right.png");
		this->imgHadoukenEmp = DG::Image::Create("./data/image/Skill_Right_Grey.png");
		this->imgShoryukenFull = DG::Image::Create("./data/image/Skill_Left.png");
		this->imgShoryukenEmp = DG::Image::Create("./data/image/Skill_Left_Grey.png");
		this->imgPlayer1Ava = DG::Image::Create("./data/image/Skill_Bottom_Chara2.png");
		this->imgPlayer2Ava = DG::Image::Create("./data/image/Skill_Bottom_Chara1.png");
		this->imgKO = DG::Image::Create("./data/image/KO.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgHPFull.reset();
		this->imgHPEmp.reset();
		this->imgGaugeEmp.reset();
		this->imgGaugeFull.reset();
		this->imgGaugeEmpR.reset();
		this->imgGaugeFullR.reset();
		this->imgHPEmp.reset();
		this->imgHPFull.reset();
		this->imgBG.reset();
		this->imgTimebar.reset();
		this->imgPowerUpFull.reset();
		this->imgPowerUpEmp.reset();
		this->imgHadoukenFull.reset();
		this->imgHadoukenEmp.reset();
		this->imgShoryukenFull.reset();
		this->imgShoryukenEmp.reset();
		this->imgPlayer1Ava.reset();
		this->imgPlayer2Ava.reset();
		this->imgKO.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		this->render2D_Priority[1] = 0.8f;
		this->time = 0;
		this->gameEndTime = 0;
		onePWin = false;
		twoPWin = false;
		mciSendString(TEXT("open \".\\data\\sound\\bgm\\TestBGM.mp3\" alias TestBGM"), NULL, 0, NULL); //BGM read file
		mciSendString(TEXT("play TestBGM repeat"), NULL, 0, NULL); //BGM repeat

		//デバッグ用フォントの準備
		this->TestFont = DG::Font::Create("ＭＳ ゴシック", 30, 30);

		//★タスクの生成
		auto p1 = Player1::Object::Create(true);
		auto p2 = Player2::Object::Create(true);

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		mciSendString(TEXT("close TestBGM"), NULL, 0, NULL); //BGM
		ge->KillAll_G("本編");
		

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto next = Ending::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		time++;
		auto inp = ge->in1->GetState( );
		
		auto p1 = ge->GetTask<Player1::Object>(Player1::defGroupName, Player1::defName);
		auto p2 = ge->GetTask<Player2::Object>(Player2::defGroupName, Player2::defName);

		//向き変換
		if (p1->CheckFoot() && p2->CheckFoot())
		{
			if (p2->pos.x - p1->pos.x >= 0)
			{
				p1->angle = Player1::Object::Angle_LR::Right;
				p2->angle = Player1::Object::Angle_LR::Left;
			}
			else
			{
				p1->angle = Player1::Object::Angle_LR::Left;
				p2->angle = Player1::Object::Angle_LR::Right;
			}
		}

		//プレイヤー二人重ねなし処理
		ML::Box2D p1HB = p1->hitBase.OffsetCopy(p1->pos);
		ML::Box2D p2HB = p2->hitBase.OffsetCopy(p2->pos);
		if (p1HB.Hit(p2HB))
		{
			if (p1HB.x < p2HB.x)
			{
				p1->pos.x -= push;
				p2->pos.x += push;
			}
			else
			{
				p1->pos.x += push;
				p2->pos.x -= push;
			}
		}

		//勝敗判定
		if (p1->currentHp <= 0)
		{
			twoPWin = true;
		}
		else if (p2->currentHp <= 0)
		{
			onePWin = true;
		}
		else if (time >= 5400)
		{
			if (p1->currentHp > p2->currentHp)
			{
				onePWin = true;
			}
			else if (p2->currentHp > p1->currentHp)
			{
				twoPWin = true;
			}
			else
			{
				//Draw
				twoPWin = true; //Kari
			}
		}

		if (onePWin)
		{
			this->p1Round++;
			if (this->p1Round >= 1)
			{
				if (!isGameEnd)
				{
					this->isGameEnd = true;
				}
			}
			//NextRound if FT2
		}
		
		if (twoPWin)
		{
			this->p2Round++;
			if (this->p2Round >= 1)
			{
				if (!isGameEnd)
				{
					this->isGameEnd = true;
				}
			}
			//NextRound if FT2
		}
		if (true == isGameEnd)
		{
			this->gameEndTime++;
			if (gameEndTime == 120)
			{
				ge->StartCounter("test", 45); //フェードは90フレームなので半分の45で切り替え
				ge->CreateEffect(99, ML::Vec2(0, 0));
			}
		}

		if (ge->getCounterFlag("test") == ge->LIMIT)
		{
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//HP計算用
		auto p1 = ge->GetTask<Player1::Object>(Player1::defGroupName, Player1::defName);
		auto p2 = ge->GetTask<Player2::Object>(Player2::defGroupName, Player2::defName);

		//背景
		ML::Box2D  draw_BG(0, 0, 1280, 720);
		ML::Box2D  src_BG(0, 0, 1672, 941);
		this->res->imgBG->Draw(draw_BG, src_BG);

		//HPバー
		//Player1
		ML::Box2D draw_HP_E(100, 40, 512, 128);
		ML::Box2D src_HP_E(0, 0, 512, 128);
		this->res->imgHPEmp->Draw(draw_HP_E, src_HP_E);
		ML::Box2D draw_HP_F(100, 40, 107 + (405 * p1->currentHp / p1->maxHp), 128);
		ML::Box2D src_HP_F(0, 0, 107 + (405 * p1->currentHp / p1->maxHp), 128); //HP0の時wはちょうど107のとこ
		this->res->imgHPFull->Draw(draw_HP_F, src_HP_F);
		//Player2
		ML::Box2D draw_HP_E2(668, 40, 512, 128);
		ML::Box2D src_HP_E2(512, 0, -512, 128);
		this->res->imgHPEmp->Draw(draw_HP_E2, src_HP_E2);
		ML::Box2D draw_HP_F2(668 + (512 - (107 + (405 * p2->currentHp / p2->maxHp))), 40, 107 + (405 * p2->currentHp / p2->maxHp), 128);
		ML::Box2D src_HP_F2(107 + (405 * p2->currentHp / p2->maxHp), 0, -107 - (405 * p2->currentHp / p2->maxHp), 128); //HP0の時wはちょうど107のとこ
		this->res->imgHPFull->Draw(draw_HP_F2, src_HP_F2);

		//必殺技ゲージ
		//Player1
		ML::Box2D draw_Gauge_E(100, 584, 256, 48);
		ML::Box2D src_Gauge_E(0, 0, 512, 96);
		this->res->imgGaugeEmp->Draw(draw_Gauge_E, src_Gauge_E);
		ML::Box2D draw_Gauge_F(100, 584, (179 + (333 * p1->currentGauge / p1->MaxGauge)) / 2, 48);
		ML::Box2D src_Gauge_F(0, 0, 179 + (333 * p1->currentGauge / p1->MaxGauge), 96); //Gauge0の時wはちょうど179のとこ
		this->res->imgGaugeFull->Draw(draw_Gauge_F, src_Gauge_F);
		//Player2
		ML::Box2D draw_Gauge_E2(924, 584, 256, 48);
		ML::Box2D src_Gauge_E2(0, 0, 512, 96);
		this->res->imgGaugeEmpR->Draw(draw_Gauge_E2, src_Gauge_E2);
		ML::Box2D draw_Gauge_F2(924 + (333 / 2) - (333 * p2->currentGauge / p2->MaxGauge) / 2 , 584, (179 + (333 * p2->currentGauge / p2->MaxGauge)) / 2, 48);
		ML::Box2D src_Gauge_F2(333 - (333 * p2->currentGauge / p2->MaxGauge), 0, 179 + (333 * p2->currentGauge / p2->MaxGauge), 96); //Gauge0の時wはちょうど179のとこ
		this->res->imgGaugeFullR->Draw(draw_Gauge_F2, src_Gauge_F2);

		//アバター画像
		//Player1
		ML::Box2D draw_AVA(100, 40, 512, 128);
		ML::Box2D src_AVA(0, 0, 512, 128);
		this->res->imgPlayer1Ava->Draw(draw_AVA, src_AVA);
		//Player2
		ML::Box2D draw_AVA2(668, 40, 512, 128);
		ML::Box2D src_AVA2(512, 0, -512, 128);
		this->res->imgPlayer2Ava->Draw(draw_AVA2, src_AVA2);

		//スキル画像
		//Player1
		//PowerUp
		ML::Box2D draw_POWERUP_E(100, 40, 512, 128);
		ML::Box2D src_POWERUP_E(0, 0, 512, 128);
		this->res->imgPowerUpEmp->Draw(draw_POWERUP_E, src_POWERUP_E);
		ML::Box2D draw_POWERUP_F(100, 40, 512, 128);
		ML::Box2D src_POWERUP_F(0, 0, 512, 128);
		if (!p1->isUsedPowerUp)
		{
			this->res->imgPowerUpFull->Draw(draw_POWERUP_F, src_POWERUP_F);
		}
		//Hadouken
		ML::Box2D draw_HADOU_E(100, 40, 512, 128);
		ML::Box2D src_HADOU_E(0, 0, 512, 128);
		this->res->imgHadoukenEmp->Draw(draw_HADOU_E, src_HADOU_E);
		ML::Box2D draw_HADOU_F(100, 40, 512, 128);
		ML::Box2D src_HADOU_F(0, 0, 512, 128);
		if (!p1->isUsedHadouken)
		{
			this->res->imgHadoukenFull->Draw(draw_HADOU_F, src_HADOU_F);
		}
		//Shoryuken
		ML::Box2D draw_SHORYU_E(100, 40, 512, 128);
		ML::Box2D src_SHORYU_E(0, 0, 512, 128);
		this->res->imgShoryukenEmp->Draw(draw_SHORYU_E, src_SHORYU_E);
		ML::Box2D draw_SHORYU_F(100, 40, 512, 128);
		ML::Box2D src_SHORYU_F(0, 0, 512, 128); 
		if (!p1->isUsedShoryu)
		{
			this->res->imgShoryukenFull->Draw(draw_HADOU_F, src_HADOU_F);
		}

		//Player2
		//PowerUp
		ML::Box2D draw_POWERUP_E2(668, 40, 512, 128);
		ML::Box2D src_POWERUP_E2(512, 0, -512, 128);
		this->res->imgPowerUpEmp->Draw(draw_POWERUP_E2, src_POWERUP_E2);
		ML::Box2D draw_POWERUP_F2(668, 40, 512, 128);
		ML::Box2D src_POWERUP_F2(512, 0, -512, 128);
		if (!p2->isUsedPowerUp)
		{
			this->res->imgPowerUpFull->Draw(draw_POWERUP_F2, src_POWERUP_F2);
		}
		//Hadouken
		ML::Box2D draw_HADOU_E2(668, 40, 512, 128);
		ML::Box2D src_HADOU_E2(512, 0, -512, 128);
		this->res->imgHadoukenEmp->Draw(draw_HADOU_E2, src_HADOU_E2);
		ML::Box2D draw_HADOU_F2(668, 40, 512, 128);
		ML::Box2D src_HADOU_F2(512, 0, -512, 128);
		if (!p2->isUsedHadouken)
		{
			this->res->imgHadoukenFull->Draw(draw_HADOU_F2, src_HADOU_F2);
		}
		//Shoryuken
		ML::Box2D draw_SHORYU_E2(668, 40, 512, 128);
		ML::Box2D src_SHORYU_E2(512, 0, -512, 128);
		this->res->imgShoryukenEmp->Draw(draw_SHORYU_E2, src_SHORYU_E2);
		ML::Box2D draw_SHORYU_F2(668, 40, 512, 128);
		ML::Box2D src_SHORYU_F2(512, 0, -512, 128);
		if (!p2->isUsedShoryu)
		{
			this->res->imgShoryukenFull->Draw(draw_HADOU_F2, src_HADOU_F2);
		}
		//KO
		ML::Box2D draw_KO(384, 232, 512, 256);
		ML::Box2D src_KO(0, 0, 256, 128);
		if (true == this->isGameEnd)
		{
			this->res->imgKO->Draw(draw_KO, src_KO);
		}
	}

	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}