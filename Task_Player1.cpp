//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player1.h"
#include  "Task_Hadouken.h"
#include  "Task_AtkBox.h"
#include  "Task_Game.h"
#include  "Task_SoundManager.h"

namespace  Player1
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->player1Img = DG::Image::Create("./data/image/Player1Large.png");
		this->powerUpImg = DG::Image::Create("./data/image/Player1power.png");
		this->playerhitbasedebug = DG::Image::Create("./data/image/debug.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->player1Img.reset();
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
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-48, -92, 96, 184);
		this->angle = Angle_LR::Right;
		this->controller = ge->in1;
		this->motion = Motion::Stand;
		this->maxSpeed = 6.5f;
		this->addSpeed = 1.0f;
		this->decSpeed = 0.5f;
		this->maxFallSpeed = 45.0f;
		this->jumpPow = -28.0f;
		this->gravity = ML::Gravity(32) * 16;
		this->pos.x = 275;
		this->pos.y = 458;
		this->currentHp = this->maxHp;
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto game = ge->GetTask<Game::Object>("本編", "統括");

		if (game == nullptr)
		{
			return;
		}

		if (game->isHitStop() || game->gamePhase == GamePhase::KO)
		{
			return;
		}

		if (this->pos.x <= 0 + 75)
		{
			this->pos.x = 0 + 75;
		}
		if (this->pos.x >= 1280 - 75)
		{
			this->pos.x = 1280 - 75;
		}
		this->moveCnt++;
		this->animCnt++;
		this->Think();
		if (game->canPlayerMove())
		{	
			this->CheckDashInput();
			this->Move();
			ML::Vec2 est = this->moveVec;
			this->CheckMove(est);
			this->addGaugeBar(0.1f);
		}
		else if (this->motion != Motion::KO || !this->CheckFoot())
		{
			if (this->CheckFoot())
			{
				this->UpdateMotion(Motion::Stand);
			}
			else
			{
				this->Move();
				ML::Vec2 est = this->moveVec;
				this->CheckMove(est);
			}
		}
		
		this->powerUpTimerUpdate();
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		/*ML::Box2D Draw(-128, -128, 256, 256);
		ML::Box2D Src(0, 0, 64, 64);*/
		BChara::DrawInfo di = this->Anim();
		di.draw.Offset(this->pos);
		this->res->player1Img->Draw(di.draw, di.src);

		BChara::DrawInfo pdi = this->PowerUpAnim();
		pdi.draw.Offset(this->pos);
		if (isPowerUp == true)
		{
			this->res->powerUpImg->Draw(pdi.draw, pdi.src);
		}

		/*ML::Box2D src(0, 0, 32, 32);
		this->res->playerhitbasedebug->Draw(this->hitBase.OffsetCopy(this->pos), src);*/
	}

	void Object::Think()
	{
		auto inp = this->controller->GetState();
		BChara::Motion nm = this->motion;

		if (this->currentHp <= 0)
		{
			nm = Motion::KO;
		}

		switch (nm)
		{
		case Motion::Stand:
			if (inp.R1.on && CheckFoot()) //W:R1 A:SE S:ST D:L3
			{
				nm = Motion::NaturalJump;
			}
			if (inp.SE.on && CheckFoot())
			{
				if (this->angle == Angle_LR::Right)
				{
					nm = Motion::BackWalk;
				}
				else
				{
					nm = Motion::ForwardWalk;
				}
			}
			if (inp.L3.on && CheckFoot())
			{
				if (this->angle == Angle_LR::Left)
				{
					nm = Motion::BackWalk;
				}
				else
				{
					nm = Motion::ForwardWalk;
				}
			}
			if (inp.SE.on && inp.ST.on && this->angle == Angle_LR::Right && CheckFoot())
			{
				nm = Motion::Guard;
			}
			if (inp.L3.on && inp.ST.on && this->angle == Angle_LR::Left && CheckFoot())
			{
				nm = Motion::Guard;
			}
			if (this->isForwardDash)
			{
				nm = Motion::ForwardDash;
			}
			if (this->isBackDash)
			{
				nm = Motion::BackDash;
			}
			break;
		case Motion::ForwardWalk:
			if (inp.R1.on && inp.SE.on)
			{
				if (this->angle == Angle_LR::Left)
				{
					nm = Motion::ForwardJump;
				}
			}
			if (inp.R1.on && inp.L3.on)
			{
				if (this->angle == Angle_LR::Right)
				{
					nm = Motion::ForwardJump;
				}
			}
			if (inp.SE.on)
			{
				if (this->angle == Angle_LR::Right)
				{
					nm = Motion::BackWalk;
				}
			}
			if (inp.L3.on)
			{
				if (this->angle == Angle_LR::Left)
				{
					nm = Motion::BackWalk;
				}
			}
			if (inp.SE.off && inp.L3.off)
			{
				nm = Motion::Stand;
			}
			if (this->isForwardDash)
			{
				nm = Motion::ForwardDash;
			}
			if (this->isBackDash)
			{
				nm = Motion::BackDash;
			}
			break;
		case Motion::BackWalk:
			if (inp.R1.on && inp.SE.on)
			{
				if (this->angle == Angle_LR::Right)
				{
					nm = Motion::BackJump;
				}
			}
			if (inp.R1.on && inp.L3.on)
			{
				if (this->angle == Angle_LR::Left)
				{
					nm = Motion::BackJump;
				}
			}
			if (inp.SE.on)
			{
				if (this->angle == Angle_LR::Left)
				{
					nm = Motion::ForwardWalk;
				}
			}
			if (inp.L3.on)
			{
				if (this->angle == Angle_LR::Right)
				{
					nm = Motion::ForwardWalk;
				}
			}
			if (inp.SE.off && inp.L3.off)
			{
				nm = Motion::Stand;
			}
			if (inp.SE.on && inp.ST.on && this->angle == Angle_LR::Right)
			{
				nm = Motion::Guard;
			}
			if (inp.L3.on && inp.ST.on && this->angle == Angle_LR::Left)
			{
				nm = Motion::Guard;
			}
			if (this->isForwardDash)
			{
				nm = Motion::ForwardDash;
			}
			if (this->isBackDash)
			{
				nm = Motion::BackDash;
			}
			break;
		case Motion::AttackPunch:
			if (this->moveCnt >= 30)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::AttackKick:
			if (this->moveCnt >= 22)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::NaturalJump:
			if (this->moveVec.y >= 0)
			{
				nm = Motion::NaturalFall;
			}
			break;
		case Motion::ForwardJump:
			if (this->moveVec.y >= 0)
			{
				nm = Motion::ForwardFall;
			}
			break;
		case Motion::BackJump:
			if (this->moveVec.y >= 0)
			{
				nm = Motion::BackFall;
			}
			break;
		case Motion::NaturalFall:
			if (this->CheckFoot() == true)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::ForwardFall:
			if (this->CheckFoot() == true)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::BackFall:
			if (this->CheckFoot() == true)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::ForwardDash:
			if (this->moveCnt == 15)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::BackDash:
			if (this->moveCnt == 18)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::NaturalJumpAtk:
			if (this->CheckFoot() == true)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::ForwardJumpAtk:
			if (this->CheckFoot() == true)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::BackJumpAtk:
			if (this->CheckFoot() == true)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::Hurt:
			if (this->moveCnt >= 15)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::Guard:
			if ((inp.ST.off && this->angle == Angle_LR::Right) || (inp.ST.off && this->angle == Angle_LR::Left))
			{
				nm = Motion::BackWalk;
			}
			if (inp.SE.up || inp.ST.up && this->angle == Angle_LR::Right)
			{
				nm = Motion::Stand;
			}
			if (inp.L3.up || inp.ST.up && this->angle == Angle_LR::Left)
			{
				nm = Motion::Stand;
			}
			//仮　もしかしたらもっといい方法がある
			if (inp.SE.on && inp.ST.on && this->angle != Angle_LR::Right)
			{
				nm = Motion::Stand;
			}
			if (inp.L3.on && inp.ST.on && this->angle != Angle_LR::Left)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::Down:
			if (this->CheckFoot())
			{
				this->downTimer++;
				if (this->downTimer >= 60)
				{
					this->downTimer = 0;
					nm = Motion::Standing;
				}
			}
			break;
		case Motion::Standing:
			if (this->moveCnt >= 30)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::Hadouken:
			if (this->moveCnt >= 60)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::Shoryuken:
			if (this->moveCnt >= 90)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::PowerUP:
			if (this->moveCnt >= 15)
			{
				nm = Motion::Stand;
			}
			break;
		}

		if (motion == Motion::ForwardJump || motion == Motion::ForwardFall)
		{
			if (inp.B1.down || inp.B2.down)
			{
				nm = Motion::ForwardJumpAtk;
			}
		}

		if (motion == Motion::NaturalJump || motion == Motion::NaturalFall)
		{
			if (inp.B1.down || inp.B2.down)
			{
				nm = Motion::NaturalJumpAtk;
			}
		}

		if (motion == Motion::BackJump || motion == Motion::BackFall)
		{
			if (inp.B1.down || inp.B2.down)
			{
				nm = Motion::BackJumpAtk;
			}
		}

		if (motion == Motion::Stand || motion == Motion::ForwardWalk || motion == Motion::BackWalk || motion == Motion::Guard)
		{
			if (inp.B1.down) //U:B1 J:B2 I:B3 K:B4 O:L1
			{
				nm = Motion::AttackPunch;
			}
			else if (inp.B2.down)
			{
				nm = Motion::AttackKick;
			}
			else if (this->isGaugeBarFull())
			{
				if (inp.B3.down && !this->isUsedHadouken)
				{
					nm = Motion::Hadouken;
				}
				else if (inp.B4.down && !this->isUsedShoryu)
				{
					nm = Motion::Shoryuken;
				}
				else if (inp.L1.down && !this->isUsedPowerUp)
				{
					nm = Motion::PowerUP;
				}
			}
		}

		this->UpdateMotion(nm);
	}

	void Object::Move()
	{
		auto inp = this->controller->GetState();
		auto sdm = ge->GetTask<SoundManager::Object>(SoundManager::defGroupName, SoundManager::defName);
		BChara::Motion nm = this->motion;

		//重力加速
		switch (this->motion)
		{
		default:
			if (this->moveVec.y < 0 || this->CheckFoot() == false)
			{
				this->moveVec.y = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
			}
			else
			{
				this->moveVec.y = 0.0f;
			}
			break;
		case Motion::Unnon:
			break;
		}

		//移動速度減衰
		switch (this->motion)
		{
		default:
			if (this->moveVec.x < 0)
			{
				this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
			}
			else
			{
				this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
			}
			break;
		case Motion::Unnon:
			break;
		case Motion::ForwardDash:
		case Motion::BackDash:
			if (this->moveVec.x > 0)
			{
				this->moveVec.x--;
			}
			else if (this->moveVec.x < 0)
			{
				this->moveVec.x++;
			}
			break;
		}

		switch (nm)
		{
		case Motion::Stand:
			if (inp.ST.on) //防御の時移動しないように
			{
				this->moveVec.x = 0.0f;
				break;
			}
			if (inp.SE.on)
			{
				if (this->angle == Angle_LR::Right)
				{
					this->moveVec.x = -this->maxSpeed;
				}
				else
				{
					this->moveVec.x = this->maxSpeed;
				}
			}
			if (inp.L3.on)
			{
				if (this->angle == Angle_LR::Left)
				{
					this->moveVec.x = -this->maxSpeed;
				}
				else
				{
					this->moveVec.x = this->maxSpeed;
				}
			}
			break;
		case Motion::ForwardWalk:
			if (inp.SE.on)
			{
				if (this->angle == Angle_LR::Left)
				{
					this->moveVec.x = -this->maxSpeed;
				}
			}
			if (inp.L3.on)
			{
				if (this->angle == Angle_LR::Right)
				{
					this->moveVec.x = this->maxSpeed;
				}
			}
			break;
		case Motion::BackWalk:
			if (inp.SE.on)
			{
				if (this->angle == Angle_LR::Right)
				{
					this->moveVec.x = -this->maxSpeed;
				}
			}
			if (inp.L3.on)
			{
				if (this->angle == Angle_LR::Left)
				{
					this->moveVec.x = this->maxSpeed;
				}
			}
			break;
		case Motion::AttackPunch:
			if (this->moveCnt == 0)
			{
				if (this->angle == Angle_LR::Right)
				{
					this->moveVec.x = 10.0f;
				}
				else
				{
					this->moveVec.x = -10.0f;
				}
			}
			if (this->moveCnt == 7)
			{
				auto AtkBox = AtkBox::Object::Create(true);
				AtkBox->owner = AtkBox::Object::Owner::Player1;
				if (this->angle == Angle_LR::Right)
				{
					AtkBox->attackBox = ML::Box2D(this->hitBase.w - 60, -this->hitBase.h / 2, 30, 96);
				}
				else
				{
					AtkBox->attackBox = ML::Box2D(-this->hitBase.w + 30, -this->hitBase.h / 2, 30, 96);
				}
				AtkBox->attackdmg = getAtkDamage(10);
				AtkBox->lifeTime = 15;
			}
			break;
		case Motion::AttackKick:
			if (this->moveCnt == 5)
			{
				auto AtkBox = AtkBox::Object::Create(true);
				AtkBox->owner = AtkBox::Object::Owner::Player1;
				if (this->angle == Angle_LR::Right)
				{
					AtkBox->attackBox = ML::Box2D(-5 + this->hitBase.w / 2, 70, 50, 16);
				}
				else
				{
					AtkBox->attackBox = ML::Box2D(-45 + -this->hitBase.w / 2, 70, 50, 16);
				}
				AtkBox->attackdmg = getAtkDamage(15);
				AtkBox->lifeTime = 12;
			}
			break;
		case Motion::NaturalJump:
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow;
			}
			break;
		case Motion::ForwardJump:
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow;
			}
			if (this->angle == Angle_LR::Right)
			{
				this->moveVec.x = this->maxSpeed;
			}
			else
			{
				this->moveVec.x = -this->maxSpeed;
			}
			break;
		case Motion::BackJump:
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow;
			}
			if (this->angle == Angle_LR::Left)
			{
				this->moveVec.x = this->maxSpeed;
			}
			else
			{
				this->moveVec.x = -this->maxSpeed;
			}
			break;
		case Motion::NaturalFall:
			break;
		case Motion::ForwardFall:
			if (this->angle == Angle_LR::Right)
			{
				this->moveVec.x = this->maxSpeed;
			}
			else
			{
				this->moveVec.x = -this->maxSpeed;
			}
			break;
		case Motion::BackFall:
			if (this->angle == Angle_LR::Left)
			{
				this->moveVec.x = this->maxSpeed;
			}
			else
			{
				this->moveVec.x = -this->maxSpeed;
			}
			break;
		case Motion::ForwardDash:
			this->isForwardDash = false;
			if (this->moveCnt == 0)
			{
				if (this->angle == Angle_LR::Right)
				{
					this->moveVec.x = this->maxSpeed * 3.0f;
				}
				else
				{
					this->moveVec.x = -this->maxSpeed * 3.0f;
				}
			}
			break;
		case Motion::BackDash:
			this->isBackDash = false;
			if (this->moveCnt == 0)
			{
				if (this->angle == Angle_LR::Left)
				{
					this->moveVec.x = this->maxSpeed * 2.5f;
				}
				else
				{
					this->moveVec.x = -this->maxSpeed * 2.5f;
				}
			}
			break;
		case Motion::NaturalJumpAtk:
			if (this->moveCnt == 3)
			{
				auto AtkBox = AtkBox::Object::Create(true);
				AtkBox->owner = AtkBox::Object::Owner::Player1;
				if (this->angle == Angle_LR::Right)
				{
					AtkBox->attackBox = ML::Box2D(-5 + this->hitBase.w / 4, 40, 50, 50);
				}
				else
				{
					AtkBox->attackBox = ML::Box2D(-45 + -this->hitBase.w / 4, 40, 50, 50);
				}
				AtkBox->attackdmg = getAtkDamage(12);
				AtkBox->lifeTime = 10;
			}
			break;
		case Motion::ForwardJumpAtk:
			if (this->angle == Angle_LR::Right)
			{
				this->moveVec.x = this->maxSpeed;
			}
			else
			{
				this->moveVec.x = -this->maxSpeed;
			}

			if (this->moveCnt == 3)
			{
				auto AtkBox = AtkBox::Object::Create(true);
				AtkBox->owner = AtkBox::Object::Owner::Player1;
				if (this->angle == Angle_LR::Right)
				{
					AtkBox->attackBox = ML::Box2D(-5 + this->hitBase.w / 4, 40, 50, 50);
				}
				else
				{
					AtkBox->attackBox = ML::Box2D(-45 + -this->hitBase.w / 4, 40, 50, 50);
				}
				AtkBox->attackdmg = getAtkDamage(12);
				AtkBox->lifeTime = 10;
			}
			break;
		case Motion::BackJumpAtk:
			if (this->angle == Angle_LR::Left)
			{
				this->moveVec.x = this->maxSpeed;
			}
			else
			{
				this->moveVec.x = -this->maxSpeed;
			}

			if (this->moveCnt == 3)
			{
				auto AtkBox = AtkBox::Object::Create(true);
				AtkBox->owner = AtkBox::Object::Owner::Player1;
				if (this->angle == Angle_LR::Right)
				{
					AtkBox->attackBox = ML::Box2D(-5 + this->hitBase.w / 4, 40, 50, 50);
				}
				else
				{
					AtkBox->attackBox = ML::Box2D(-45 + -this->hitBase.w / 4, 40, 50, 50);
				}
				AtkBox->attackdmg = getAtkDamage(12);
				AtkBox->lifeTime = 10;
			}
			break;
		case Motion::Hurt:
			break;
		case Motion::Guard:
			break;
		case Motion::Down:
			break;
		case Motion::Standing:
			break;
		case Motion::Hadouken:
			this->useGaugeBar();
			this->isUsedHadouken = true;
			if (this->moveCnt == 0)
			{
				auto hadouken = Hadouken::Object::Create(true);
				hadouken->owner = Hadouken::Object::Owner::Player1;
				hadouken->pos = this->pos;
				sdm->playHadouSE();
				if (this->angle == Angle_LR::Right)
				{
					hadouken->pos.x += 20;
					hadouken->flyingSpeed = 25.0f;
				}
				else
				{
					hadouken->pos.x -= 20;
					hadouken->flyingSpeed = -25.0f;
				}
			}
			break;
		case Motion::Shoryuken:
			this->useGaugeBar();
			this->isUsedShoryu = true;
			if (this->moveCnt == 0)
			{
				sdm->playShoryuSE();
				moveVec.y = this->jumpPow;
				if (this->angle == Angle_LR::Right)
				{
					this->moveVec.x = 12.5f;
				}
				else
				{
					this->moveVec.x = -12.5f;
				}
			}
			if (this->moveCnt == 3)
			{
				auto AtkBox = AtkBox::Object::Create(true);
				AtkBox->owner = AtkBox::Object::Owner::Player1;
				AtkBox->attackBox = this->hitBase;
				AtkBox->attackdmg = getAtkDamage(50);
				AtkBox->lifeTime = 15;
			}
			break;
		case Motion::PowerUP:
			this->useGaugeBar();
			this->isUsedPowerUp = true;
			this->isPowerUp = true;
			this->powerUpTimer = this->maxPowerUpTime;
			break;
		}
	}

	void Object::CheckDashInput()
	{
		auto inp = this->controller->GetState(); //A:SE D:L3

		if (forwardDashTimer != -1)
		{
			forwardDashTimer++;
		}
		if (BackDashTimer != -1)
		{
			BackDashTimer++;
		}

		if (this->motion == Motion::Hurt || this->motion == Motion::Guard)
		{
			return;
		}

		if (inp.L3.down)
		{
			if (this->angle == Angle_LR::Right)
			{
				if (this->forwardDashTimer >= 0 && this->forwardDashTimer <= 12 && this->CheckFoot())
				{
					this->forwardDashTimer = -1;
					this->isForwardDash = true;
				}
				else
				{
					this->forwardDashTimer = 0;
				}
			}
			else
			{
				if (this->BackDashTimer >= 0 && this->BackDashTimer <= 12 && this->CheckFoot())
				{
					this->BackDashTimer = -1;
					this->isBackDash = true;
				}
				else
				{
					this->BackDashTimer = 0;
				}
			}
		}
		if (inp.SE.down)
		{
			if (this->angle == Angle_LR::Left)
			{
				if (this->forwardDashTimer >= 0 && this->forwardDashTimer <= 12 && this->CheckFoot())
				{
					this->forwardDashTimer = -1;
					this->isForwardDash = true;
				}
				else
				{
					this->forwardDashTimer = 0;
				}
			}
			else
			{
				if (this->BackDashTimer >= 0 && this->BackDashTimer <= 12 && this->CheckFoot())
				{
					this->BackDashTimer = -1;
					this->isBackDash = true;
				}
				else
				{
					this->BackDashTimer = 0;
				}
			}
		}
	}

	BChara::DrawInfo Object::PowerUpAnim() 
	{
		BChara::DrawInfo imageTable[] =
		{
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(0, 384, 64, 64)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(64, 384, 64, 64)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(128, 384, 64, 64)},
		};
		BChara::DrawInfo rtv;
		int work;
		work = (this->animCnt / 8) % 3;
		rtv = imageTable[work];
		return rtv;
	}

	//Animation
	BChara::DrawInfo Object::Anim() 
	{
		BChara::DrawInfo Punch[] =
		{
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(192, 128, 64, 64)}, //Punch 1
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(256, 128, 64, 64)}, //Punch 2
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(320, 128, 64, 64)}, //Punch 3
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(192*4, 128*4, 64*4, 64*4)}, //Punch 1
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(256*4, 128*4, 64*4, 64*4)}, //Punch 2
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(320*4, 128*4, 64*4, 64*4)}, //Punch 3
		};

		BChara::DrawInfo Kick[] =
		{
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(896 , 0, 64, 64)}, //Kick 1
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(960 , 0, 64, 64)}, //Kick 2
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(1024, 0, 64, 64)}, //Kick 3
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(896 *4, 0*4, 64*4, 64*4)}, //Kick 1
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(960 *4, 0*4, 64*4, 64*4)}, //Kick 2
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(1024*4, 0*4, 64*4, 64*4)}, //Kick 3
		};

		BChara::DrawInfo Shoryuken[] =
		{
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(512, 128, 64, 64)}, //Shoryuken 1
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(576, 128, 64, 64)}, //Shoryuken 2
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(640, 128, 64, 64)}, //Shoryuken 3
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(704, 128, 64, 64)}, //Shoryuken 4
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(768, 128, 64, 64)}, //Shoryuken 5
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(832, 128, 64, 64)}, //Shoryuken 6
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(0  , 0  , 64, 64)}, //Shoryuken 7
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(512*4, 128*4, 64*4, 64*4)}, //Shoryuken 1
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(576*4, 128*4, 64*4, 64*4)}, //Shoryuken 2
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(640*4, 128*4, 64*4, 64*4)}, //Shoryuken 3
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(704*4, 128*4, 64*4, 64*4)}, //Shoryuken 4
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(768*4, 128*4, 64*4, 64*4)}, //Shoryuken 5
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(832*4, 128*4, 64*4, 64*4)}, //Shoryuken 6
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(0  *4, 0  *4, 64*4, 64*4)}, //Shoryuken 7
		};

		BChara::DrawInfo Hadouken[] =
		{
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(576, 320, 64, 64)}, //Hadouken 1
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(640, 320, 64, 64)}, //Hadouken 2
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(704, 320, 64, 64)}, //Hadouken 3
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(576*4, 320*4, 64*4, 64*4)}, //Hadouken 1
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(640*4, 320*4, 64*4, 64*4)}, //Hadouken 2
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(704*4, 320*4, 64*4, 64*4)}, //Hadouken 3
		};

		BChara::DrawInfo imageTable[] =
		{
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(0   , 0  , 64, 64)},//Stand　1番目
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(64  , 64 , 64, 64)},//ForwardWalk
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(0   , 64 , 64, 64)},//BackWalk
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(960 , 128, 64, 64)}, //Back jump fall
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(1024, 128, 64, 64)}, //Natural jump fall 5番目
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(896 , 256, 64, 64)}, //Forward jump fall
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(64  , 192, 64, 64)}, //Guard
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(192 , 192, 64, 64)}, //Hurt
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(448 , 192, 64, 64)}, //KO
			//{ML::Box2D(-128, -128, 256, 256), ML::Box2D(1088, 128, 64, 64)}, //JumpAtk 10番目
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(0   *4, 0  *4, 64*4, 64*4)},//Stand　1番目
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(64  *4, 64 *4, 64*4, 64*4)},//ForwardWalk
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(0   *4, 64 *4, 64*4, 64*4)},//BackWalk
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(960 *4, 128*4, 64*4, 64*4)}, //Back jump fall
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(1024*4, 128*4, 64*4, 64*4)}, //Natural jump fall 5番目
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(896 *4, 256*4, 64*4, 64*4)}, //Forward jump fall
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(64  *4, 192*4, 64*4, 64*4)}, //Guard
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(192 *4, 192*4, 64*4, 64*4)}, //Hurt
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(448 *4, 192*4, 64*4, 64*4)}, //KO Down
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(1088*4, 128*4, 64*4, 64*4)}, //JumpAtk 10番目
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(640 *4, 192*4, 64*4, 64*4)}, //Standing
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(448 *4, 0  *4, 64*4, 64*4)}, //ForwardDash
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(128 *4, 0  *4, 64*4, 64*4)}, //BackDash
		};
		BChara::DrawInfo rtv;
		int work;
		switch (this->motion)
		{
		default:
			rtv = imageTable[0];
			break;
		case Motion::Stand:
			rtv = imageTable[0];
			break;
		case Motion::ForwardWalk:
			rtv = imageTable[1];
			break;
		case Motion::BackWalk:
			rtv = imageTable[2];
			break;
		case Motion::NaturalJump:
		case Motion::NaturalFall:
			rtv = imageTable[4];
			break;
		case Motion::ForwardJump:
		case Motion::ForwardFall:
			rtv = imageTable[5];
			break;
		case Motion::BackJump:
		case Motion::BackFall:
			rtv = imageTable[3];
			break;
		case Motion::AttackPunch:
			//work = (this->animCnt / 20) % 3;
			work = min(animCnt / 10, 2);
			rtv = Punch[work];
			break;
		case Motion::AttackKick:
			//work = (this->animCnt / 20) % 3;
			work = min(animCnt / 10, 2);
			rtv = Kick[work];
			break;
		case Motion::NaturalJumpAtk:
		case Motion::ForwardJumpAtk:
		case Motion::BackJumpAtk:
			rtv = imageTable[9];
			break;
		case Motion::Hadouken:
			//work = (this->animCnt / 40) % 3;
			work = min(animCnt / 10, 2);
			rtv = Hadouken[work];
			break;
		case Motion::Shoryuken:
			//work = (this->animCnt / 20) % 6;
			work = min(animCnt / 15, 6);
			rtv = Shoryuken[work];
			break;
		case Motion::Guard:
			rtv = imageTable[6];
			break;
		case Motion::Hurt:
			rtv = imageTable[7];
			break;
		case Motion::KO:
			rtv = imageTable[8];
			break;
		case Motion::Down:
			rtv = imageTable[8];
			break;
		case Motion::Standing:
			rtv = imageTable[10];
			break;
		case Motion::ForwardDash:
			rtv = imageTable[11];
			break;
		case Motion::BackDash:
			rtv = imageTable[12];
			break;
		}
		

		if (this->angle == Angle_LR::Left)
		{
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}
		return rtv;
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