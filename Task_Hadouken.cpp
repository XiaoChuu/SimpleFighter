//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Hadouken.h"
#include  "Task_Player1.h"
#include  "Task_Player2.h"

namespace  Hadouken
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->fireBallImg = DG::Image::Create("./data/image/FireBall.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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
		this->flyingSpeed = 0.0f;
		this->hitBase = ML::Box2D(-25, -25, 50, 50);
		this->attackdmg = 40;
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
		if (this->pos.x <= 0 - 48 || this->pos.x >= 1280 + 48)
		{
			this->Kill();
			return;
		}

		auto p1 = ge->GetTask<Player1::Object>(Player1::defGroupName, Player1::defName);
		auto p2 = ge->GetTask<Player2::Object>(Player2::defGroupName, Player2::defName);

		//Playerとの当たり判定
		if (owner == Owner::Player1)
		{
			ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
			auto target = ge->GetTasks<BChara>("本編", "Player2");
			for (auto it = target->begin(); it != target->end(); it++)
			{
				ML::Box2D hit = (*it)->hitBase.OffsetCopy((*it)->pos);
				if (hit.Hit(me) && (*it)->motion != Motion::Guard && (*it)->motion != Motion::KO)
				{
					(*it)->takeDamage(p1->getAtkDamage(this->attackdmg));
					this->Kill();
					break;
				}
				else if (hit.Hit(me) && (*it)->motion == Motion::Guard)
				{
					this->Kill();
					break;
				}
			}
		}
		else if (owner == Owner::Player2)
		{
			ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
			auto target = ge->GetTasks<BChara>("本編", "Player1");
			for (auto it = target->begin(); it != target->end(); it++)
			{
				ML::Box2D hit = (*it)->hitBase.OffsetCopy((*it)->pos);
				if (hit.Hit(me) && (*it)->motion != Motion::Guard && (*it)->motion != Motion::KO)
				{
					(*it)->takeDamage(p2->getAtkDamage(this->attackdmg));
					this->Kill();
					break;
				}
				else if (hit.Hit(me) && (*it)->motion == Motion::Guard)
				{
					this->Kill();
					break;
				}
			}
		}

		//Hadouken同士との当たり判定
		ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
		auto target = ge->GetTasks<Hadouken::Object>("本編", "Hadouken");
		for (auto it = target->begin(); it != target->end(); it++)
		{
			if ((*it)->owner == this->owner)
			{
				continue;
			}
			ML::Box2D hit = (*it)->hitBase.OffsetCopy((*it)->pos);
			if (hit.Hit(me))
			{
				this->Kill();
				(*it)->Kill();
				break;
			}
		}

		this->animCnt++;
		this->pos.x += this->flyingSpeed;

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		Object::DrawInfo di = this->fireballAnim();
		di.draw.Offset(this->pos);
		this->res->fireBallImg->Draw(di.draw, di.src, di.color);
	}

	Object::DrawInfo Object::fireballAnim()
	{
		ML::Color color;
		if (this->owner == Owner::Player1)
		{
			color = ML::Color(1, 1, 1, 1);
		}
		else
		{
			color = ML::Color(1, 0, 0, 1);
		}

		Object::DrawInfo imageTable[] =
		{
			{ML::Box2D(-48, -48, 96, 96), ML::Box2D(0, 0, 64, 64), color},   //fireball 1
			{ML::Box2D(-48, -48, 96, 96), ML::Box2D(64, 0, 64, 64), color},	 //fireball 2
			{ML::Box2D(-48, -48, 96, 96), ML::Box2D(128, 0, 64, 64), color}, //fireball 3
			{ML::Box2D(-48, -48, 96, 96), ML::Box2D(192, 0, 64, 64), color}, //fireball 4
			{ML::Box2D(-48, -48, 96, 96), ML::Box2D(256, 0, 64, 64), color}, //fireball 5
			{ML::Box2D(-48, -48, 96, 96), ML::Box2D(320, 0, 64, 64), color}, //hit effect 1
			{ML::Box2D(-48, -48, 96, 96), ML::Box2D(384, 0, 64, 64), color}, //hit effect 2
			{ML::Box2D(-48, -48, 96, 96), ML::Box2D(448, 0, 64, 64), color}, //hit effect 3
		};
		Object::DrawInfo rtv;
		int work;
		work = (this->animCnt / 8) % 5;
		rtv = imageTable[work];
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