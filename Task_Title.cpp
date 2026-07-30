//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"
#include "randomLib.h"
#include  "Task_Effect00.h"
#include "sound.h"

#include "Task_Game.h"

namespace  Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Main_Menu_image.png");
		this->TitleLogo01 = DG::Image::Create("./data/image/StartKey.png");
		this->TitleLogo02 = DG::Image::Create("./data/image/Title_Image.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img.reset();
		this->TitleLogo01.reset();
		this->TitleLogo02.reset();
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
		this->controller = ge->in1;
		this->render2D_Priority[1] = 0.5f;
		int RenderTime = 0;
		this->timer = 0;
		this->srcSet = 0;

		ge->stage = 1;


		//デバッグ用フォントの準備
		this->TestFont = DG::Font::Create("ＭＳ ゴシック", 30, 30);

		

		//★タスクの生成
		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = this->controller->GetState();

		int input = 0;
		//							16進		2進	
		if (inp.B1.down) input |= 0x0001;	// 0000000001
		if (inp.B2.down) input |= 0x0002;	// 0000000010
		if (inp.B3.down) input |= 0x0004;	// 0000000100
		if (inp.B4.down) input |= 0x0008;	// 0000001000
		if (inp.ST.down) input |= 0x0100;	// 0100000000

		if (input & 0x0100) {
			ge->StartCounter("test", 45); //フェードは90フレームなので半分の45で切り替え
			ge->CreateEffect(98, ML::Vec2(0, 0));
		}
		if (ge->getCounterFlag("test") == ge->LIMIT) {
			this->Kill();
		}

		if (timer >= 0 && timer < 30) {
			++timer;
			srcSet = 0;
		}
		else if (timer >= 30 && timer < 60) {
			++timer;
			srcSet = 91;
		}
		else {
			timer = 0;
		}

		return;

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw_BG(0, 0, 1280, 720);
		ML::Box2D  src_BG(0, 0, 1280, 720);
		this->res->img->Draw(draw_BG, src_BG);

		ML::Box2D  draw_PRS(342, 380, 596, 91);
		ML::Box2D  src_PRS(0, 0, 596, 91);
		src_PRS.Offset(0, srcSet);
		this->res->TitleLogo01->Draw(draw_PRS, src_PRS);
		ML::Box2D  draw_T(240, -80, 768, 512);
		ML::Box2D  src_T(0, 0, 1536, 1024);
		this->res->TitleLogo02->Draw(draw_T, src_T);
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