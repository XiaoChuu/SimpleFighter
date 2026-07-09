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
		this->player1Img = DG::Image::Create("./data/image/kariplayer1.png");
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
		this->hitBase = ML::Box2D(-75, -150, 150, 300);
		this->angle = Angle_LR::Right;
		this->controller = ge->in1;
		this->motion = Motion::Stand;
		this->maxSpeed = 5.0f;
		this->addSpeed = 1.0f;
		this->decSpeed = 0.5f;
		this->maxFallSpeed = 10.0f;
		this->jumpPow = -10.0f;
		this->gravity = ML::Gravity(32) * 5;
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
		ML::Box2D Draw(-75, -150, 150, 300);
		Draw.Offset(this->pos);
		ML::Box2D Src(0, 0, 64, 64);
		this->res->player1Img->Draw(Draw, Src);
	}

	void Object::Think() 
	{
		auto inp = this->controller->GetState();
		BChara::Motion nm = this->motion;

		switch (nm)
		{
		case Motion::Stand:
			if (inp.LStick.BU.on)
			{
				nm = Motion::Jump;
			}
			if (inp.LStick.BL.on)
			{
				nm = Motion::Walk;
			}
			if (inp.LStick.BR.on)
			{
				nm = Motion::Walk;
			}
			break;
		case Motion::Walk:
			if (inp.LStick.BU.on)
			{
				nm = Motion::Jump;
			}
			if (inp.LStick.BL.off && inp.LStick.BR.off)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::AttackPunch:
			break;
		case Motion::AttackKick:
			break;
		case Motion::Jump:
			if (this->moveVec.y >= 0)
			{
				nm = Motion::Fall;
			}
			break;
		case Motion::Fall:
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
			if (inp.LStick.BL.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case Motion::Walk:
			if (inp.LStick.BL.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case Motion::AttackPunch:
			break;
		case Motion::AttackKick:
			break;
		case Motion::Jump:
			if (inp.LStick.BL.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow;
			}
			break;
		case Motion::Fall:
			if (inp.LStick.BL.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case Motion::Super1:
			break;
		case Motion::Super2:
			break;
		case Motion::Super3:
			break;
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