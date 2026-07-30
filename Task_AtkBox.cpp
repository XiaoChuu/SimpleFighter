//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_AtkBox.h"
#include  "Task_Player1.h"
#include  "Task_Player2.h"

namespace  AtkBox
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->atkboxdebug = DG::Image::Create("./data/image/debug.png");
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
		this->attackBox = ML::Box2D(0, 0, 0, 0);
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
		auto p1 = ge->GetTask<Player1::Object>(Player1::defGroupName, Player1::defName);
		auto p2 = ge->GetTask<Player2::Object>(Player2::defGroupName, Player2::defName);

		//当たり判定
		if (owner == Owner::Player1)
		{
			ML::Box2D me = this->attackBox.OffsetCopy(p1->pos);
			auto target = ge->GetTasks<BChara>("本編", "Player2");
			for (auto it = target->begin(); it != target->end(); it++)
			{
				ML::Box2D hit = (*it)->hitBase.OffsetCopy((*it)->pos);
				if (hit.Hit(me) && (*it)->motion == Motion::Guard && p1->motion != Motion::Shoryuken)
				{
					if (p1->angle == Angle_LR::Right)
					{
						p1->moveVec.x = -10.0;
					}
					else
					{
						p1->moveVec.x = 10.0;
					}
				}
				if (hit.Hit(me) && (*it)->motion != Motion::Guard && (*it)->motion != Motion::KO)
				{
					(*it)->takeDamage(attackdmg);
					if (p1->motion != Motion::Shoryuken)
					{
						if (p1->angle == Angle_LR::Right)
						{
							p1->moveVec.x = -10.0;
						}
						else
						{
							p1->moveVec.x = 10.0;
						}
					}
					this->Kill();
					break;
				}
			}
		}
		else if (owner == Owner::Player2)
		{
			ML::Box2D me = this->attackBox.OffsetCopy(p2->pos);
			auto target = ge->GetTasks<BChara>("本編", "Player1");
			for (auto it = target->begin(); it != target->end(); it++)
			{
				ML::Box2D hit = (*it)->hitBase.OffsetCopy((*it)->pos);
				if (hit.Hit(me) && (*it)->motion == Motion::Guard && p2->motion != Motion::Shoryuken)
				{
					if (p2->angle == Angle_LR::Right)
					{
						p2->moveVec.x = -10.0;
					}
					else
					{
						p2->moveVec.x = 10.0;
					}
				}
				if (hit.Hit(me) && (*it)->motion != Motion::Guard && (*it)->motion != Motion::KO)
				{
					(*it)->takeDamage(attackdmg);
					if (p2->motion != Motion::Shoryuken)
					{
						if (p2->angle == Angle_LR::Right)
						{
							p2->moveVec.x = -10.0;
						}
						else
						{
							p2->moveVec.x = 10.0;
						}
					}	
					this->Kill();
					break;
				}
			}
		}
		
		lifeTime--;
		if (lifeTime <= 0)
		{
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		/*auto p1 = ge->GetTask<Player1::Object>(Player1::defGroupName, Player1::defName);
		auto p2 = ge->GetTask<Player2::Object>(Player2::defGroupName, Player2::defName);
		ML::Box2D src(0, 0, 32, 32);
		if (owner == Owner::Player1)
		{
			this->res->atkboxdebug->Draw(this->attackBox.OffsetCopy(p1->pos), src);
		}
		else
		{
			this->res->atkboxdebug->Draw(this->attackBox.OffsetCopy(p2->pos), src);
		}*/
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