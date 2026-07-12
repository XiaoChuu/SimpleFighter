//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"
#include "randomLib.h"
#include  "Task_Ending.h"
#include  "Task_player1.h"

bool onePWin;
bool twoPWin;

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
		this->time = 0;
		
		//デバッグ用フォントの準備
		this->TestFont = DG::Font::Create("ＭＳ ゴシック", 30, 30);

		//★タスクの生成
		auto p1 = Player1::Object::Create(true);


		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
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
		time++;
		auto inp = ge->in1->GetState( );
		if (time == 5400) {
			ge->StartCounter("test", 45); //フェードは90フレームなので半分の45で切り替え
			ge->CreateEffect(99, ML::Vec2(0, 0));
		}
		if (ge->getCounterFlag("test") == ge->LIMIT) {
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

		//HPバー
		ML::Box2D draw_HP_E(100, 40, 512, 128);
		ML::Box2D src_HP_E(0, 0, 512, 128);
		this->res->imgHPEmp->Draw(draw_HP_E, src_HP_E);
		ML::Box2D draw_HP_F(100, 40, 512, 128);
		ML::Box2D src_HP_F(0, 0, 512, 128);//仮
		this->res->imgHPFull->Draw(draw_HP_F, src_HP_F);

		ML::Box2D draw_HP_E2(668, 40, 512, 128);
		ML::Box2D src_HP_E2(512, 0, -512, 128);
		this->res->imgHPEmp->Draw(draw_HP_E2, src_HP_E2);
		ML::Box2D draw_HP_F2(668, 40, 512, 128);
		ML::Box2D src_HP_F2(512, 0, -512, 128);//仮
		this->res->imgHPFull->Draw(draw_HP_F2, src_HP_F2);

		//必殺技ゲージ
		ML::Box2D draw_Gauge_E(100, 584, 256, 48);
		ML::Box2D src_Gauge_E(0, 0, 512, 96);
		this->res->imgGaugeEmp->Draw(draw_Gauge_E, src_Gauge_E);
		ML::Box2D draw_Gauge_F(100, 584, 256, 48);
		ML::Box2D src_Gauge_F(0, 0, 512, 96);//仮
		this->res->imgGaugeFull->Draw(draw_Gauge_F, src_Gauge_F);
		ML::Box2D draw_Gauge_E2(924, 584, 256, 48);
		ML::Box2D src_Gauge_E2(0, 0, 512, 96);
		this->res->imgGaugeEmpR->Draw(draw_Gauge_E2, src_Gauge_E2);
		ML::Box2D draw_Gauge_F2(924, 584, 256, 48);
		ML::Box2D src_Gauge_F2(0, 0, 512, 96);//仮
		this->res->imgGaugeFullR->Draw(draw_Gauge_F2, src_Gauge_F2);

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