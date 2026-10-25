//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_AtkBox.h"
#include  "Task_Player1.h"
#include  "Task_Player2.h"
#include  "Task_EffectManager.h"
#include  "Task_SoundManager.h"
#include  "Task_Game.h"

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
		auto eff = ge->GetTask<EffectManager::Object>(EffectManager::defGroupName, EffectManager::defName);
		auto sdm = ge->GetTask<SoundManager::Object>(SoundManager::defGroupName, SoundManager::defName);
		auto game = ge->GetTask<Game::Object>("本編", "統括");

		if (p1->motion == Motion::Stand && owner == Owner::Player1)
		{
			this->lifeTime = 0;
		}

		if (p2->motion == Motion::Stand && owner == Owner::Player2)
		{
			this->lifeTime = 0;
		}

		//当たり判定
		if (owner == Owner::Player1)
		{
			ML::Box2D me = this->attackBox.OffsetCopy(p1->pos);
			auto target = ge->GetTasks<BChara>("本編", "Player2");
			for (auto it = target->begin(); it != target->end(); it++)
			{
				ML::Box2D hit = (*it)->hitBase.OffsetCopy((*it)->pos);
				if (hit.Hit(me) && (*it)->motion == Motion::Guard && (*it)->motion != Motion::Down && (*it)->motion != Motion::Standing) //ガードに当たる際
				{	
					if (p1->motion != Motion::Shoryuken)
					{
						(*it)->takeGuardDamage(attackdmg, false);
						eff->p1guardEffect.playEffect((*it)->pos, p1->pos.x < (*it)->pos.x);
						sdm->playGuardSE();
						game->startHitStop(4);
						if (p1->angle == Angle_LR::Right)
						{
							p1->moveVec.x = -12.0f;
							(*it)->moveVec.x = 5.0f;
						}
						else
						{
							p1->moveVec.x = 12.0f;
							(*it)->moveVec.x = -5.0f;
						}
					}
					else
					{
						(*it)->takeGuardDamage(attackdmg, true);
						eff->p1guardEffect.playEffect((*it)->pos, p1->pos.x < (*it)->pos.x);
						sdm->playGuardSE();
						game->startHitStop(4);
					}
					this->Kill();
					break;
				}
				if (hit.Hit(me) && (*it)->motion != Motion::Guard && (*it)->motion != Motion::KO && (*it)->motion != Motion::Down && (*it)->motion != Motion::Standing) //当たる際
				{
					if (p1->motion != Motion::Shoryuken)
					{
						(*it)->takeDamage(attackdmg);
						eff->p1hitEffect.playEffect((*it)->pos, p1->pos.x < (*it)->pos.x);
						sdm->playHitSE();
						game->startHitStop(6);
						if (p1->angle == Angle_LR::Right)
						{
							p1->moveVec.x = -5.0f;
							(*it)->moveVec.x = 8.0f;
						}
						else
						{
							p1->moveVec.x = 5.0f;
							(*it)->moveVec.x = -8.0f;
						}
					}
					else
					{
						(*it)->takeShoryuDamage(attackdmg);
						eff->p1hitEffect.playEffect((*it)->pos, p1->pos.x < (*it)->pos.x);
						sdm->playHitSE();
						game->startHitStop(6);
						if (p1->angle == Angle_LR::Right)
						{
							(*it)->moveVec.x = 20.0f;
						}
						else
						{
							(*it)->moveVec.x = -20.0f;
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
				if (hit.Hit(me) && (*it)->motion == Motion::Guard && (*it)->motion != Motion::Down && (*it)->motion != Motion::Standing) //ガードに当たる際
				{
					if (p2->motion != Motion::Shoryuken)
					{
						(*it)->takeGuardDamage(attackdmg, false);
						eff->p2guardEffect.playEffect((*it)->pos, p2->pos.x < (*it)->pos.x);
						sdm->playGuardSE();
						game->startHitStop(4);
						if (p2->angle == Angle_LR::Right)
						{
							p2->moveVec.x = -12.0f;
							(*it)->moveVec.x = 5.0f;
						}
						else
						{
							p2->moveVec.x = 12.0f;
							(*it)->moveVec.x = -5.0f;
						}
					}
					else
					{
						(*it)->takeGuardDamage(attackdmg, true);
						eff->p2guardEffect.playEffect((*it)->pos, p2->pos.x < (*it)->pos.x);
						sdm->playGuardSE();
						game->startHitStop(4);
					}
					this->Kill();
					break;
				}
				if (hit.Hit(me) && (*it)->motion != Motion::Guard && (*it)->motion != Motion::KO && (*it)->motion != Motion::Down && (*it)->motion != Motion::Standing) //当たる際
				{
					if (p2->motion != Motion::Shoryuken)
					{
						(*it)->takeDamage(attackdmg);
						eff->p2hitEffect.playEffect((*it)->pos, p2->pos.x < (*it)->pos.x);
						sdm->playHitSE();
						game->startHitStop(6);
						if (p2->angle == Angle_LR::Right)
						{
							p2->moveVec.x = -5.0f;
							(*it)->moveVec.x = 8.0f;
						}
						else
						{
							p2->moveVec.x = 5.0f;
							(*it)->moveVec.x = -8.0f;
						}
					}
					else
					{
						(*it)->takeShoryuDamage(attackdmg);
						eff->p2hitEffect.playEffect((*it)->pos, p2->pos.x < (*it)->pos.x);
						sdm->playHitSE();
						game->startHitStop(6);
						if (p2->angle == Angle_LR::Right)
						{
							(*it)->moveVec.x = 20.0f;
						}
						else
						{
							(*it)->moveVec.x = -20.0f;
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
		auto p1 = ge->GetTask<Player1::Object>(Player1::defGroupName, Player1::defName);
		auto p2 = ge->GetTask<Player2::Object>(Player2::defGroupName, Player2::defName);
		/*ML::Box2D src(0, 0, 32, 32);
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