//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player1.h"

namespace  Player1
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->player1Img = DG::Image::Create("./data/image/Player1.png");
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
		this->maxSpeed = 3.5f;
		this->addSpeed = 1.0f;
		this->decSpeed = 0.5f;
		this->maxFallSpeed = 10.0f;
		this->jumpPow = -15.0f;
		this->gravity = ML::Gravity(32) * 3;
		this->pos.x = 275;
		this->pos.y = 400;
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
		this->Move();
		ML::Vec2 est = this->moveVec;
		this->CheckMove(est);
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
	}

	void Object::Think() 
	{
		auto inp = this->controller->GetState();
		BChara::Motion nm = this->motion;

		switch (nm)
		{
		case Motion::Stand:
			if (inp.R1.on) //W:R1 A:SE S:ST D:L3
			{
				nm = Motion::NaturalJump;
			}
			if (inp.SE.on)
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
			if (inp.L3.on)
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
			break;
		case Motion::AttackPunch:
			break;
		case Motion::AttackKick:
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
		case Motion::Super1:
			break;
		case Motion::Super2:
			break;
		case Motion::Super3:
			break;
		}

		this->UpdateMotion(nm);
	}

	void Object::Move()
	{
		auto inp = this->controller->GetState();
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
		}

		switch (nm)
		{
		case Motion::Stand:
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
					this->moveVec.x = -this->maxSpeed;
				}
			}
			break;
		case Motion::AttackPunch:
			break;
		case Motion::AttackKick:
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
		case Motion::Guard:

			break;
		case Motion::Super1:
			break;
		case Motion::Super2:
			break;
		case Motion::Super3:
			break;
		}
	}

	//Animation
	BChara::DrawInfo Object::Anim() 
	{
		BChara::DrawInfo imageTable[] =
		{
			{ML::Box2D(-128, -128, 256, 256),ML::Box2D(0, 0, 64, 64)},//Stand
			{ML::Box2D(-128, -128, 256, 256),ML::Box2D(64, 64, 64, 64)},//ForwardWalk
			{ML::Box2D(-128, -128, 256, 256),ML::Box2D(0, 64, 64, 64)},//BackWalk
			{ML::Box2D(-128, -128, 256, 256),ML::Box2D(960, 128, 64, 64)}, //Back jump fall
			{ML::Box2D(-128, -128, 256, 256),ML::Box2D(1024, 128, 64, 64)}, //Natural jump fall
			{ML::Box2D(-128, -128, 256, 256),ML::Box2D(1088, 128, 64, 64)}, //Forward jump fall
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