//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_SoundManager.h"
#include  "sound.h"

namespace  SoundManager
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		bgm::LoadFile("BGM","./data/sound/bgm/BattleBGM.mp3");
		se::LoadFile("se_Hit","./data/sound/se/Hitsound.wav");
		se::LoadFile("se_Guard", "./data/sound/se/Guardsound.wav");
		se::LoadFile("se_Round1", "./data/sound/se/ROUND1.wav");
		se::LoadFile("se_Round2", "./data/sound/se/ROUND2.wav");
		se::LoadFile("se_FinalRound", "./data/sound/se/FinalRound.wav");
		se::LoadFile("se_Fight", "./data/sound/se/ROUND_Fight.wav");
		se::LoadFile("se_KO", "./data/sound/se/ROUND_KO.wav");
		se::LoadFile("se_Timeover", "./data/sound/se/Timeover.wav");
		se::LoadFile("se_Hadou", "./data/sound/se/HadouSE.wav");
		se::LoadFile("se_Shoryu", "./data/sound/se/ShoryuSE.wav");
		se::LoadFile("se_Charge", "./data/sound/se/ChargeSE_long.wav");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		bgm::Stop("BGM");
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
		bgm::VolumeControl("BGM", 95);
		bgm::Play("BGM");
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
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
	}

	void Object::playHitSE() 
	{
		se::SetVolume("se_Hit", 20);
		se::Play("se_Hit");
	}

	void Object::playGuardSE() 
	{
		se::SetVolume("se_Guard", 20);
		se::Play("se_Guard");
	}

	void Object::playRound1SE() 
	{
		se::SetVolume("se_Round1", 10);
		se::Play("se_Round1");
	}

	void Object::playRound2SE() 
	{
		se::SetVolume("se_Round2", 10);
		se::Play("se_Round2");
	}

	void Object::playFinalRoundSE()
	{
		se::SetVolume("se_FinalRound", 15);
		se::Play("se_FinalRound");
	}

	void Object::playFightSE()
	{
		se::SetVolume("se_Fight", 10);
		se::Play("se_Fight");
	}

	void Object::playKOSE()
	{
		se::SetVolume("se_KO", 10);
		se::Play("se_KO");
	}

	void Object::playTimeoverSE()
	{
		se::SetVolume("se_Timeover", 15);
		se::Play("se_Timeover");
	}

	void Object::playHadouSE() 
	{
		se::SetVolume("se_Hadou", 50);
		se::Play("se_Hadou");
	}

	void Object::playShoryuSE()
	{
		se::SetVolume("se_Shoryu", 50);
		se::Play("se_Shoryu");
	}

	void Object::playChargeSE() 
	{
		se::SetVolume("se_Charge", 50);
		se::Play("se_Charge");
	}

	void Object::stopChargeSE()
	{
		se::Stop("se_Charge");
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