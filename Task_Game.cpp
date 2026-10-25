//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"
#include "randomLib.h"
#include  "Task_Ending.h"
#include  "Task_player1.h"
#include  "Task_player2.h"
#include  "Task_EffectManager.h"
#include  "Task_RoundRender.h"
#include  "Task_SoundManager.h"

bool onePWin;
bool twoPWin;
bool drawGame;

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
		this->imgTimeFont = DG::Image::Create("./data/image/Time.png");
		this->imgEmptyScore = DG::Image::Create("./data/image/VCup_Empty.png");
		this->imgScore = DG::Image::Create("./data/image/VIcon.png");
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
		this->imgTimeFont.reset();
		this->imgEmptyScore.reset();
		this->imgScore.reset();
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
		this->battleTime = 3600;
		this->gameEndTime = 0;
		this->p1Round = 0;
		this->p2Round = 0;
		this->p1DisplayRound = 0;
		this->p2DisplayRound = 0;
		onePWin = false;
		twoPWin = false;
		drawGame = false;
		this->p1RoundWin = false;
		this->p2RoundWin = false;
		this->drawRound = false;
		//mciSendString(TEXT("open \".\\data\\sound\\bgm\\TestBGM.mp3\" alias TestBGM"), NULL, 0, NULL); //BGM read file
		//mciSendString(TEXT("play TestBGM repeat"), NULL, 0, NULL); //BGM repeat
		this->gamePhase = GamePhase::p1Entrance;

		//デバッグ用フォントの準備
		this->TestFont = DG::Font::Create("ＭＳ ゴシック", 30, 30);

		//★タスクの生成
		auto p1 = Player1::Object::Create(true);
		auto p2 = Player2::Object::Create(true);
		auto eff = EffectManager::Object::Create(true);
		auto RRD = RoundRender::Object::Create(true);
		auto SDM = SoundManager::Object::Create(true);
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
		this->phaseChange();

		if (this->gamePhase == GamePhase::Battle)
		{
			battleTime--;
		}

		auto inp = ge->in1->GetState();

		auto p1 = ge->GetTask<Player1::Object>(Player1::defGroupName, Player1::defName);
		auto p2 = ge->GetTask<Player2::Object>(Player2::defGroupName, Player2::defName);

		//HitStop処理
		this->hitStopCnt--;

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
		if (this->gamePhase != GamePhase::RoundTransition)
		{
			if (p1->currentHp <= 0 && p2->currentHp <= 0)
			{
				if (this->gamePhase == GamePhase::KO)
				{
					return;
				}
				this->gamePhase = GamePhase::KO;
				this->drawRound = true;
				this->p1Round++;
				this->p2Round++;
			}
			else if (p1->currentHp <= 0 && this->gamePhase != GamePhase::KO)
			{
				this->gamePhase = GamePhase::KO;
				this->p2RoundWin = true;
				this->p2Round++;
			}
			else if (p2->currentHp <= 0 && this->gamePhase != GamePhase::KO)
			{
				this->gamePhase = GamePhase::KO;
				this->p1RoundWin = true;
				this->p1Round++;
			}
			else if (this->battleTime <= 0 && this->gamePhase != GamePhase::TimeOver)
			{
				if (p1->currentHp > p2->currentHp)
				{
					this->gamePhase = GamePhase::TimeOver;
					this->p1RoundWin = true;
					this->p1Round++;
				}
				else if (p2->currentHp > p1->currentHp)
				{
					this->gamePhase = GamePhase::TimeOver;
					this->p2RoundWin = true;
					this->p2Round++;
				}
				else
				{
					this->gamePhase = GamePhase::TimeOver;
					this->drawRound = true;
					this->p1Round++;
					this->p2Round++;
				}
			}
		}

		if (true == isGameEnd)
		{
			this->gameEndTime++;
			if (this->p1Round > this->p2Round)
			{
				onePWin = true;
			}
			else if (this->p1Round < this->p2Round)
			{
				twoPWin = true;
			}
			else
			{
				drawGame = true;
			}

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
		//背景
		ML::Box2D  draw_BG(0, 0, 1280, 720);
		ML::Box2D  src_BG(0, 0, 1672, 941);
		this->res->imgBG->Draw(draw_BG, src_BG);
		//制限時間
		number_Render(592, 80, battleTime / 600);//587
		number_Render(638, 80, battleTime / 60);//642

		//PlayerUI
		this->UI_Render();

		//VictoryScore
		this->EmptyScore_Render();
		this->Score_Render();
	}

	void Object::UI_Render()
	{
		//HP計算用
		auto p1 = ge->GetTask<Player1::Object>(Player1::defGroupName, Player1::defName);
		auto p2 = ge->GetTask<Player2::Object>(Player2::defGroupName, Player2::defName);

		int imgOffset = 30;
		//HPバー
		//Player1
		ML::Box2D draw_HP_E(100 - imgOffset, 40, 512, 128);
		ML::Box2D src_HP_E(0, 0, 512, 128);
		this->res->imgHPEmp->Draw(draw_HP_E, src_HP_E);
		ML::Box2D draw_HP_F(100 - imgOffset, 40, 107 + (405 * p1->currentHp / p1->maxHp), 128);
		ML::Box2D src_HP_F(0, 0, 107 + (405 * p1->currentHp / p1->maxHp), 128); //HP0の時wはちょうど107のとこ
		this->res->imgHPFull->Draw(draw_HP_F, src_HP_F);
		//Player2
		ML::Box2D draw_HP_E2(668 + imgOffset, 40, 512, 128);
		ML::Box2D src_HP_E2(512, 0, -512, 128);
		this->res->imgHPEmp->Draw(draw_HP_E2, src_HP_E2);
		ML::Box2D draw_HP_F2(668 + imgOffset + (512 - (107 + (405 * p2->currentHp / p2->maxHp))), 40, 107 + (405 * p2->currentHp / p2->maxHp), 128);
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
		ML::Box2D draw_Gauge_F2(924 + (334 / 2) - (334 * p2->currentGauge / p2->MaxGauge) / 2, 584, (178 + (334 * p2->currentGauge / p2->MaxGauge)) / 2, 48);
		ML::Box2D src_Gauge_F2(334 - (334 * p2->currentGauge / p2->MaxGauge), 0, 178 + (334 * p2->currentGauge / p2->MaxGauge), 96); //Gauge0の時wはちょうど179のとこ
		this->res->imgGaugeFullR->Draw(draw_Gauge_F2, src_Gauge_F2);

		//アバター画像
		//Player1
		ML::Box2D draw_AVA(100 - imgOffset, 40, 512, 128);
		ML::Box2D src_AVA(0, 0, 512, 128);
		this->res->imgPlayer1Ava->Draw(draw_AVA, src_AVA);
		//Player2
		ML::Box2D draw_AVA2(668 + imgOffset, 40, 512, 128);
		ML::Box2D src_AVA2(512, 0, -512, 128);
		this->res->imgPlayer2Ava->Draw(draw_AVA2, src_AVA2);

		//スキル画像
		//Player1
		//PowerUp
		ML::Box2D draw_POWERUP_E(100 - imgOffset, 40, 512, 128);
		ML::Box2D src_POWERUP_E(0, 0, 512, 128);
		this->res->imgPowerUpEmp->Draw(draw_POWERUP_E, src_POWERUP_E);
		ML::Box2D draw_POWERUP_F(100 - imgOffset, 40, 512, 128);
		ML::Box2D src_POWERUP_F(0, 0, 512, 128);
		if (!p1->isUsedPowerUp)
		{
			this->res->imgPowerUpFull->Draw(draw_POWERUP_F, src_POWERUP_F);
		}
		//Hadouken
		ML::Box2D draw_HADOU_E(100 - imgOffset, 40, 512, 128);
		ML::Box2D src_HADOU_E(0, 0, 512, 128);
		this->res->imgHadoukenEmp->Draw(draw_HADOU_E, src_HADOU_E);
		ML::Box2D draw_HADOU_F(100 - imgOffset, 40, 512, 128);
		ML::Box2D src_HADOU_F(0, 0, 512, 128);
		if (!p1->isUsedHadouken)
		{
			this->res->imgHadoukenFull->Draw(draw_HADOU_F, src_HADOU_F);
		}
		//Shoryuken
		ML::Box2D draw_SHORYU_E(100 - imgOffset, 40, 512, 128);
		ML::Box2D src_SHORYU_E(0, 0, 512, 128);
		this->res->imgShoryukenEmp->Draw(draw_SHORYU_E, src_SHORYU_E);
		ML::Box2D draw_SHORYU_F(100 - imgOffset, 40, 512, 128);
		ML::Box2D src_SHORYU_F(0, 0, 512, 128);
		if (!p1->isUsedShoryu)
		{
			this->res->imgShoryukenFull->Draw(draw_HADOU_F, src_HADOU_F);
		}

		//Player2
		//PowerUp
		ML::Box2D draw_POWERUP_E2(668 + imgOffset, 40, 512, 128);
		ML::Box2D src_POWERUP_E2(512, 0, -512, 128);
		this->res->imgPowerUpEmp->Draw(draw_POWERUP_E2, src_POWERUP_E2);
		ML::Box2D draw_POWERUP_F2(668 + imgOffset, 40, 512, 128);
		ML::Box2D src_POWERUP_F2(512, 0, -512, 128);
		if (!p2->isUsedPowerUp)
		{
			this->res->imgPowerUpFull->Draw(draw_POWERUP_F2, src_POWERUP_F2);
		}
		//Hadouken
		ML::Box2D draw_HADOU_E2(668 + imgOffset, 40, 512, 128);
		ML::Box2D src_HADOU_E2(512, 0, -512, 128);
		this->res->imgHadoukenEmp->Draw(draw_HADOU_E2, src_HADOU_E2);
		ML::Box2D draw_HADOU_F2(668 + imgOffset, 40, 512, 128);
		ML::Box2D src_HADOU_F2(512, 0, -512, 128);
		if (!p2->isUsedHadouken)
		{
			this->res->imgHadoukenFull->Draw(draw_HADOU_F2, src_HADOU_F2);
		}
		//Shoryuken
		ML::Box2D draw_SHORYU_E2(668 + imgOffset, 40, 512, 128);
		ML::Box2D src_SHORYU_E2(512, 0, -512, 128);
		this->res->imgShoryukenEmp->Draw(draw_SHORYU_E2, src_SHORYU_E2);
		ML::Box2D draw_SHORYU_F2(668 + imgOffset, 40, 512, 128);
		ML::Box2D src_SHORYU_F2(512, 0, -512, 128);
		if (!p2->isUsedShoryu)
		{
			this->res->imgShoryukenFull->Draw(draw_HADOU_F2, src_HADOU_F2);
		}
	}

	void Object::EmptyScore_Render()
	{
		ML::Box2D src_p1EmptyScore(32, 0, -32, 32);
		ML::Box2D src_p2EmptyScore(0, 0, 32, 32);

		this->res->imgEmptyScore->Draw(this->draw_Score[0], src_p1EmptyScore);
		this->res->imgEmptyScore->Draw(this->draw_Score[1], src_p1EmptyScore);
		this->res->imgEmptyScore->Draw(this->draw_Score[2], src_p2EmptyScore);
		this->res->imgEmptyScore->Draw(this->draw_Score[3], src_p2EmptyScore);
	}

	void Object::Score_Render()
	{
		ML::Box2D src_p1Score[2]
		{
			ML::Box2D(416, 0, -32, 32),
			ML::Box2D(512, 0, -32, 32),
		};
		ML::Box2D src_p2Score[2]
		{
			ML::Box2D(384, 0, 32, 32),
			ML::Box2D(480, 0, 32, 32),
		};
		if (this->p1DisplayRound >= 1)
		{
			this->res->imgScore->Draw(this->draw_Score[0], src_p1Score[0]);
		}
		if (this->p1DisplayRound >= 2)
		{
			this->res->imgScore->Draw(this->draw_Score[1], src_p1Score[1]);
		}
		if (this->p2DisplayRound >= 1)
		{
			this->res->imgScore->Draw(this->draw_Score[2], src_p2Score[0]);
		}
		if (this->p2DisplayRound >= 2)
		{
			this->res->imgScore->Draw(this->draw_Score[3], src_p2Score[1]);
		}
	}

	//-------------------------------------------------------------------
	void Object::number_Render(int x, int y, int t)
	{
		int draw_x, draw_y;
		int font_x, font_y;

		//文字開始位置をセット
		draw_x = x; draw_y = y;

		//画像データの参照位置を求める
		if (t <= 0)
		{
			t = 0;
		}
		font_x = (t % 10) * 100;
		font_y = 10;

		ML::Box2D draw(0, 0, 55, 80);
		ML::Box2D src(font_x, font_y, 100, 160);

		draw.Offset(draw_x, draw_y);
		this->res->imgTimeFont->Draw(draw, src);
		//一文字分ずらす
		//draw_x += 240;
	}

	void Object::phaseChange()
	{
		auto eff = ge->GetTask<EffectManager::Object>(EffectManager::defGroupName, EffectManager::defName);
		auto rrd = ge->GetTask<RoundRender::Object>(RoundRender::defGroupName, RoundRender::defName);
		auto sdm = ge->GetTask<SoundManager::Object>(SoundManager::defGroupName, SoundManager::defName);
		this->phaseTimer++;

		switch (this->gamePhase)
		{
		case GamePhase::p1Entrance:
			if (this->phaseTimer >= 30)
			{
				this->gamePhase = GamePhase::Round1;
				this->phaseTimer = 0;
			}
			break;
		case GamePhase::Round1:
			if (this->phaseTimer == 1)
			{
				rrd->scale = 10.0f;
				sdm->playRound1SE();
			}
			if (this->phaseTimer >= ROUND_EFFECT_TIME)
			{
				this->gamePhase = GamePhase::Fight;
				this->phaseTimer = 0;
			}
			break;
		case GamePhase::Round2:
			if (this->phaseTimer == 1)
			{
				rrd->scale = 10.0f;
				sdm->playRound2SE();
			}
			if (this->phaseTimer >= ROUND_EFFECT_TIME)
			{
				this->gamePhase = GamePhase::Fight;
				this->phaseTimer = 0;
			}
			break;
		case GamePhase::FinalRound:
			if (this->phaseTimer == 1)
			{
				rrd->scale = 10.0f;
				sdm->playFinalRoundSE();
			}
			if (this->phaseTimer >= ROUND_EFFECT_TIME)
			{
				this->gamePhase = GamePhase::Fight;
				this->phaseTimer = 0;
			}
			break;
		case GamePhase::Fight:
			if (this->phaseTimer == 1)
			{
				rrd->scale = 10.0f;
				sdm->playFightSE();
			}
			if (this->phaseTimer >= FIGHT_EFFECT_TIME)
			{
				this->gamePhase = GamePhase::Battle;
				this->phaseTimer = 0;
			}
			break;
		case GamePhase::Battle:
			this->phaseTimer = 0;
			break;
		case GamePhase::KO:
			if (this->phaseTimer == 1)
			{
				rrd->scaleK = 10.0f;
				sdm->playKOSE();
			}
			if (rrd->scaleK <= 1.0f && this->phaseTimer == KO_O_DRAW_TIME)
			{
				rrd->scaleO = 10.0f;
			}
			if (this->phaseTimer == KO_EFFECT_TIME)
			{
				this->gamePhase = GamePhase::RoundTransition;
				this->phaseTimer = 0;
			}
			break;
		case GamePhase::TimeOver:
			if (this->phaseTimer == 1)
			{
				sdm->playTimeoverSE();
			}
			if (this->phaseTimer == KO_EFFECT_TIME)
			{
				this->gamePhase = GamePhase::RoundTransition;
				this->phaseTimer = 0;
			}
			break;
		case GamePhase::RoundTransition:
			if (this->phaseTimer == 30)
			{
				if (this->p1RoundWin)
				{
					this->p1DisplayRound = p1Round;
					eff->p1ScoreEffect.playEffect(draw_Score[this->p1Round - 1]);
				}
				if (this->p2RoundWin)
				{
					this->p2DisplayRound = p2Round;
					eff->p2ScoreEffect.playEffect(draw_Score[2 + this->p2Round - 1]);
				}
				if (this->drawRound)
				{
					this->p1DisplayRound = p1Round;
					this->p2DisplayRound = p2Round;
					eff->p1ScoreEffect.playEffect(draw_Score[this->p1Round - 1]);
					eff->p2ScoreEffect.playEffect(draw_Score[2 + this->p2Round - 1]);
				}
			}
			if (this->p1Round >= 2 || this->p2Round >= 2)
			{
				this->isGameEnd = true;
			}
			else if (this->phaseTimer == ROUND_TRANSIT_EFFECT_TIME)
			{
				ge->CreateEffect(99, ML::Vec2(0, 0));
			}
			else if (this->phaseTimer >= KO_NEXT_ROUND_TIME)
			{
				this->phaseTimer = 0;
				this->nextRoundInitialize();
			}
			break;
		case GamePhase::BlankGap:
			if (this->phaseTimer == 30)
			{
				//round reset
				if (p1Round == 1 && p2Round == 1)
				{
					this->gamePhase = GamePhase::FinalRound;
				}
				else if (p1Round == 1 || p2Round == 1)
				{
					this->gamePhase = GamePhase::Round2;
				}
				this->phaseTimer = 0;
			}
			break;
		}
	}

	bool Object::canPlayerMove()
	{
		return gamePhase == GamePhase::Battle;
	}

	void Object::nextRoundInitialize()
	{
		auto p1 = ge->GetTask<Player1::Object>(Player1::defGroupName, Player1::defName);
		auto p2 = ge->GetTask<Player2::Object>(Player2::defGroupName, Player2::defName);

		//Player reset
		p1->resetPlayer(275, 458);
		p2->resetPlayer(1000, 458);
		p1->isPowerUp = false;
		p2->isPowerUp = false;

		//ToTheGap
		this->gamePhase = GamePhase::BlankGap;

		//batte result reset
		this->p1RoundWin = false;
		this->p2RoundWin = false;
		this->drawRound = false;

		//timereset
		this->battleTime = 3600;
		this->gameEndTime = 0;
		this->phaseTimer = 0;
	}

	void Object::startHitStop(int frame)
	{
		if (this->hitStopCnt < frame)
		{
			this->hitStopCnt = frame;
		}
	}

	bool Object::isHitStop() const
	{
		return this->hitStopCnt > 0;
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