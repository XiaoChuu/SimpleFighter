//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Effect00.h"

namespace  Effect00
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		//デフォルト
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
		this->render2D_Priority[1] = 0.f;

		//移動量
		this->pos = ML::Vec2(0, 0);
		this->moveVec = ML::Vec2(0,0);
		this->graVec = ML::Vec2(0, 0);

		//角度
		this->angle = 0.f;//ML::ToRadian((float)(rand() % 360));
		this->addAngle = 0.f;

		//α
		this->alpha = 1.f;
		this->addAlpha = 0.f;

		//かくだいりつ　
		this->scale = 1.f;
		this->addScale = 0.f;

		//カウント
		this->count = 0;
		this->countMax = 30;

		//リソースサイズ
		this->resSizeX = 256;
		this->resSizeY = 256;
		this->drawSizeX = 64;
		this->drawSizeY = 64;

		//アニメ
		this->animno = 0;
		this->addAnim = 0.f;
		this->animMax = 0;
		this->animResNum = 1;

		this->filename = "";

		this->AddRender = false;

		this->fade = false;
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
//		this->img.reset();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		if (fade == true) {
			alpha = sin(ML::ToRadian(angle));
			angle += addAngle;
			if (angle >= 180.f) {
				this->Kill();
			}
		}
		else {

			//移動
			pos += moveVec;		//移動
			moveVec += graVec;	//重力加算

			//角度
			angle += addAngle;
			//透明度
			alpha += addAlpha;
			//拡大率
			scale += addScale;

			//消滅トリガーはカウント、アニメのみ
			//カウント
			//カウント消化で消滅
			this->count++;
			//カウントはアニメがある場合、消滅条件にならない
			if (animMax == 0) {
				if (count == countMax) {
					this->Kill();
				}
			}
			//アニメーション
			//アニメ終了で消滅
			this->animno += this->addAnim;

			//アニメは0コマの場合は消滅条件にならない
			if (animMax != 0) {
				if ((int)animno >= animMax) {
					this->Kill();
				}
			}
			//----------
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//指定されたファイルのロード
//
		if (this->count == 0) {
			if (!this->filename.empty()) {
				this->img = DG::Image::Create(filename);
			}
			else {
				this->img = DG::Image::Create("./data/image/星-01.png");
			}
		}

		//Box2D定義のint x,y,w,hをfloatにすること
		ML::Box2D  draw((int)(-drawSizeX / 2 * scale) ,(int)( -drawSizeY / 2 * scale),
			(int)(drawSizeX * scale),(int)( drawSizeY * scale));
		draw.Offset(this->pos);
		//
		if (this->animResNum <= 0) this->animResNum = 1;
		ML::Box2D  src(((int)this->animno % this->animResNum) * resSizeX,
			((int)this->animno / this->animResNum) * resSizeY,
			resSizeX, resSizeY);

		//スクロール対応
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		//フェードの際には利用無
		if (fade != true) {
			this->img->Rotation(this->angle, ML::Vec2(draw.w / 2.0f, draw.h / 2.0f));
		}
		if (this->AddRender) {
			//現在登録されているバッファ情報をレンダリングする。
			ge->dgi->Flush2D();
			ge->dgi->EffectState().BS_AlphaAdd();
		}
		this->img->Draw(draw, src, ML::Color(this->alpha, 1, 1, 1));

		this->img->Rotation(0, ML::Vec2(draw.w / 2.0f, draw.h / 2.0f));

		if (this->AddRender) {
			//もう一度レンダリング
			ge->dgi->Flush2D();
			ge->dgi->EffectState().BS_Alpha();
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
				//（メソッド名が変なのは旧バージョンのコピーによるバグを回避するため
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