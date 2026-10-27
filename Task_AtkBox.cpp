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
				if (hit.Hit(me) && isGuarding(*it)) //ガードに当たる際
				{	
					if (p1->motion == Motion::Shoryuken)
					{
						(*it)->takeGuardDamage(attackdmg, true);
						eff->p1guardEffect.playEffect((*it)->pos, p1->pos.x < (*it)->pos.x);
						sdm->playGuardSE();
						game->startHitStop(4);
					}
					else
					{
						handle_Guard(p1, (*it));
						eff->p1guardEffect.playEffect((*it)->pos, p1->pos.x < (*it)->pos.x);
						sdm->playGuardSE();
						game->startHitStop(4);
					}
					this->Kill();
					break;
				}
				if (hit.Hit(me) && isNotGuarding(*it)) //当たる際
				{
					if (p1->motion == Motion::Shoryuken)
					{
						this->handle_ShoryuAtk(p1, (*it));
						eff->p1hitEffect.playEffect((*it)->pos, p1->pos.x < (*it)->pos.x);
						sdm->playHitSE();
						game->startHitStop(6);
					}
					else
					{
						this->handle_Atk(p1, (*it));
						eff->p1hitEffect.playEffect((*it)->pos, p1->pos.x < (*it)->pos.x);
						sdm->playHitSE();
						game->startHitStop(6);
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
				if (hit.Hit(me) && isGuarding(*it)) //ガードに当たる際
				{
					if (p2->motion == Motion::Shoryuken)
					{
						(*it)->takeGuardDamage(attackdmg, true);
						eff->p2guardEffect.playEffect((*it)->pos, p2->pos.x < (*it)->pos.x);
						sdm->playGuardSE();
						game->startHitStop(4);
					}
					else
					{
						this->handle_Guard(p2, (*it));
						eff->p2guardEffect.playEffect((*it)->pos, p2->pos.x < (*it)->pos.x);
						sdm->playGuardSE();
						game->startHitStop(4);
					}
					this->Kill();
					break;
				}
				if (hit.Hit(me) && isNotGuarding(*it)) //当たる際
				{
					if (p2->motion == Motion::Shoryuken)
					{
						this->handle_ShoryuAtk(p2, (*it));
						eff->p2hitEffect.playEffect((*it)->pos, p2->pos.x < (*it)->pos.x);
						sdm->playHitSE();
						game->startHitStop(6);
					}
					else
					{
						this->handle_Atk(p2, (*it));
						eff->p2hitEffect.playEffect((*it)->pos, p2->pos.x < (*it)->pos.x);
						sdm->playHitSE();
						game->startHitStop(6);
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

	bool Object::isGuarding(const std::shared_ptr<BChara> target)
	{
		return target->motion == Motion::Guard && target->motion != Motion::Down && target->motion != Motion::Standing;
	}

	bool Object::isNotGuarding(const std::shared_ptr<BChara> target)
	{
		return target->motion != Motion::Guard && target->motion != Motion::KO && target->motion != Motion::Down && target->motion != Motion::Standing;
	}

	void Object::handle_Guard(const std::shared_ptr<BChara> attacker, const std::shared_ptr<BChara> target)
	{
		target->takeGuardDamage(this->attackdmg, false);

		if (attacker->angle == Angle_LR::Right)
		{
			attacker->moveVec.x = -12.0f;
			target->moveVec.x = 5.0f;
		}
		else
		{
			attacker->moveVec.x = 12.0f;
			target->moveVec.x = -5.0f;
		}
	}

	void Object::handle_Atk(const std::shared_ptr<BChara> attacker, const std::shared_ptr<BChara> target)
	{
		target->takeDamage(this->attackdmg);
		if (attacker->angle == Angle_LR::Right)
		{
			attacker->moveVec.x = -5.0f;
			target->moveVec.x = 8.0f;
		}
		else
		{
			attacker->moveVec.x = 5.0f;
			target->moveVec.x = -8.0f;
		}
	}

	void Object::handle_ShoryuAtk(const std::shared_ptr<BChara> attacker, const std::shared_ptr<BChara> target)
	{
		target->takeShoryuDamage(attackdmg);
		if (attacker->angle == Angle_LR::Right)
		{
			target->moveVec.x = 20.0f;
		}
		else
		{
			target->moveVec.x = -20.0f;
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