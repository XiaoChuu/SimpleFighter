//-------------------------------------------------------------------
//エンディング
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"
#include "randomLib.h"

namespace  Ending
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/BG.png");
		this->TitleLogo01 = DG::Image::Create("./data/image/StartKey.png");
		this->win01 = DG::Image::Create("./data/image/1pwin.png");
		this->win02 = DG::Image::Create("./data/image/2pwin.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img.reset();
		this->TitleLogo01.reset();
		this->win01.reset();
		this->win02.reset();
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
		this->timer1 = 0;
		this->timer2 = 0;
		this->srcSet = 0;

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
			auto  nextTask = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		if (inp.ST.down) {
			ge->StartCounter("test", 45); //フェードは90フレームなので半分の45で切り替え
			ge->CreateEffect(99, ML::Vec2(0, 0));
		}
		if (ge->getCounterFlag("test") == ge->LIMIT) {
			this->Kill();
		}

		timer1++;

		if (timer2 >= 0 && timer2 < 30) {
			++timer2;
			srcSet = 0;
		}
		else if (timer2 >= 30 && timer2 < 60) {
			++timer2;
			srcSet = 91;
		}
		else {
			timer2 = 0;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw_BG(0, 0, 1280, 720);
		ML::Box2D  src_BG(0, 0, 1672, 941);
		this->res->img->Draw(draw_BG, src_BG);

		if (timer1 >= 60 && onePWin) {
			ML::Box2D  draw_WIN(360, 180, 560, 248);
			ML::Box2D  src_WIN(0, 0, 280, 124);
			this->res->win01->Draw(draw_WIN, src_WIN);
		}
		if (timer1 >= 60 && twoPWin) {
			ML::Box2D  draw_WIN(360, 180, 560, 248);
			ML::Box2D  src_WIN(0, 0, 292, 124);
			this->res->win02->Draw(draw_WIN, src_WIN);
		}

		if (timer1 >= 150) {
			ML::Box2D  draw_PRS(342, 380, 596, 91);
			ML::Box2D  src_PRS(0, 0, 596, 91);
			src_PRS.Offset(0, srcSet);
			this->res->TitleLogo01->Draw(draw_PRS, src_PRS);
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