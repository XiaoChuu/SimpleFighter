//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_RoundRender.h"
#include  "Task_Game.h"

namespace  RoundRender
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgK = DG::Image::Create("./data/image/KDot.png");
		this->imgO = DG::Image::Create("./data/image/ODot.png");
		this->imgRound1 = DG::Image::Create("./data/image/Round1.png");
		this->imgRound2 = DG::Image::Create("./data/image/Round2.png");
		this->imgFinalRound = DG::Image::Create("./data/image/FinalRound.png");
		this->imgFight = DG::Image::Create("./data/image/FIGHT.png");
		this->imgTimeOver = DG::Image::Create("./data/image/Timeover.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgK.reset();
		this->imgO.reset();
		this->imgRound1.reset();
		this->imgRound2.reset();
		this->imgFinalRound.reset();
		this->imgFight.reset();
		this->imgTimeOver.reset();
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

		scale -= 1.0f;

		if (scale <= 1.0f)
		{
			scale = 1.0f;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		auto game = ge->GetTask<Game::Object>("本編", "統括");
		
		//K
		ML::Box2D draw_K(-384 * this->scale, -128 * this->scale, 768 * this->scale, 256 * this->scale);
		draw_K.Offset(640, 360);//画面中心
		ML::Box2D src_K(0, 0, 1200, 400);
		if (game->gamePhase == GamePhase::KO)
		{
			this->res->imgK->Draw(draw_K, src_K);
		}
		//O
		ML::Box2D draw_O(-384 * this->scale, -128 * this->scale, 768 * this->scale, 256 * this->scale);
		draw_O.Offset(640, 360);//画面中心
		ML::Box2D src_O(0, 0, 1200, 400);
		if (game->gamePhase == GamePhase::KO)
		{
			this->res->imgO->Draw(draw_O, src_O);
		}
		//Round 1
		ML::Box2D draw_Round1(-384 * this->scale, -128 * this->scale, 768 * this->scale, 256 * this->scale);
		draw_Round1.Offset(640, 360);//画面中心
		ML::Box2D src_Round1(0, 0, 1200, 400);
		if (game->gamePhase == GamePhase::Round1)
		{
			this->res->imgRound1->Draw(draw_Round1, src_Round1);
		}
		//Round 2
		ML::Box2D draw_Round2(-384 * this->scale, -128 * this->scale, 768 * this->scale, 256 * this->scale);
		draw_Round2.Offset(640, 360);//画面中心
		ML::Box2D src_Round2(0, 0, 1200, 400);
		if (game->gamePhase == GamePhase::Round2)
		{
			this->res->imgRound2->Draw(draw_Round2, src_Round2);
		}
		//FinalRound
		ML::Box2D draw_FinalRound(-384 * this->scale, -128 * this->scale, 768 * this->scale, 256 * this->scale);
		draw_FinalRound.Offset(640, 360);//画面中心
		ML::Box2D src_FinalRound(0, 0, 1200, 400);
		if (game->gamePhase == GamePhase::FinalRound)
		{
			this->res->imgFinalRound->Draw(draw_FinalRound, src_FinalRound);
		}
		//Fight
		ML::Box2D draw_Fight(-256 * this->scale, -128 * this->scale, 512 * this->scale, 256 * this->scale);
		draw_Fight.Offset(640, 360);//画面中心
		ML::Box2D src_Fight(0, 0, 1200, 400);
		if (game->gamePhase == GamePhase::Fight)
		{
			this->res->imgFight->Draw(draw_Fight, src_Fight);
		}
		//Time Over
		ML::Box2D draw_TimeOver(-384, -128, 768, 256);
		draw_TimeOver.Offset(640, 360);//画面中心
		ML::Box2D src_TimeOver(0, 0, 1200, 400);
		if (game->gamePhase == GamePhase::TimeOver)
		{
			this->res->imgTimeOver->Draw(draw_TimeOver, src_TimeOver);
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