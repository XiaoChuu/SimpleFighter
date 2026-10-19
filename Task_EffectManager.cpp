//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EffectManager.h"

namespace  EffectManager
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgHitEffect = DG::Image::Create("./data/image/SmokeEffect_Orange.png");
		this->imgGuardEffect = DG::Image::Create("./data/image/SmokeEffect_Blue.png");
		this->imgP1HadouEffect = DG::Image::Create("./data/image/FireBall.png");
		this->imgP2HadouEffect = DG::Image::Create("./data/image/FireBall.png");
		this->imgScoreEffect = DG::Image::Create("./data/image/VEffect.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgHitEffect.reset();
		this->imgGuardEffect.reset();
		this->imgP1HadouEffect.reset();
		this->imgP2HadouEffect.reset();
		this->imgScoreEffect.reset();
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
		this->render2D_Priority[1] = 0.5f;
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
		this->hitEffect.updateEffect(4, 5);
		this->guardEffect.updateEffect(4, 5);
		this->p1FBEffect.updateEffect(4, 3);
		this->p2FBEffect.updateEffect(4, 3);
		this->scoreEffect.updateEffect(8, 23);
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		Object::DrawInfo hitEffectDraw = this->hitEffectAnim();
		if (hitEffect.isPlaying)
		{
			hitEffectDraw.draw.Offset(hitEffect.pos);
			this->res->imgHitEffect->Draw(hitEffectDraw.draw, hitEffectDraw.src);
		}
		
		Object::DrawInfo guardEffectDraw = this->guardEffectAnim();
		if (guardEffect.isPlaying)
		{
			guardEffectDraw.draw.Offset(guardEffect.pos);
			this->res->imgGuardEffect->Draw(guardEffectDraw.draw, guardEffectDraw.src);
		}
		
		Object::DrawInfo p1HadouEffectDraw = this->p1FbEffectAnim();
		if (p1FBEffect.isPlaying)
		{
			p1HadouEffectDraw.draw.Offset(p1FBEffect.pos);
			this->res->imgP1HadouEffect->Draw(p1HadouEffectDraw.draw, p1HadouEffectDraw.src, ML::Color(1, 1, 1, 1));
		}

		Object::DrawInfo p2HadouEffectDraw = this->p2FbEffectAnim();
		if (p2FBEffect.isPlaying)
		{
			p2HadouEffectDraw.draw.Offset(p2FBEffect.pos);
			this->res->imgP2HadouEffect->Draw(p2HadouEffectDraw.draw, p2HadouEffectDraw.src, ML::Color(1, 0, 0, 1));
		}
		
		Object::DrawInfo scoreEffectDraw = this->scoreEffectAnim();
		if (scoreEffect.isPlaying)
		{
			scoreEffectDraw.draw.Offset(scoreEffect.pos);
			this->res->imgScoreEffect->Draw(scoreEffectDraw.draw, scoreEffectDraw.src);
		}
	}

	void Object::EffectInfo::playEffect(ML::Box2D pos, bool isFaceRight)
	{
		this->pos.x = pos.x;
		this->pos.y = pos.y;
		this->animCnt = 0;
		this->isFaceRight = isFaceRight;
		this->isPlaying = true;
	}

	void Object::EffectInfo::playEffect(ML::Vec2 pos, bool isFaceRight)
	{
		this->pos.x = pos.x;
		this->pos.y = pos.y;
		this->animCnt = 0;
		this->isFaceRight = isFaceRight;
		this->isPlaying = true;
	}

	void Object::EffectInfo::updateEffect(int speed, int picCnt)
	{
		if (!this->isPlaying)
		{
			return;
		}

		this->animCnt++;
		if (this->animCnt >= speed * picCnt)
		{
			this->isPlaying = false;
		}
	}

	void Object::effectDirection(Object::DrawInfo& rtv, bool isFaceRight)
	{
		if (isFaceRight == false)
		{
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}
	}

	Object::DrawInfo Object::hitEffectAnim()
	{
		Object::DrawInfo imageTable[]
		{
			{ML::Box2D(-32, -96, 64, 192), ML::Box2D(0  , 0, 128, 128)},
			{ML::Box2D(-32, -96, 64, 192), ML::Box2D(128, 0, 128, 128)},
			{ML::Box2D(-32, -96, 64, 192), ML::Box2D(256, 0, 128, 128)},
			{ML::Box2D(-32, -96, 64, 192), ML::Box2D(384, 0, 128, 128)},
			{ML::Box2D(-32, -96, 64, 192), ML::Box2D(512, 0, 128, 128)},
		};

		Object::DrawInfo rtv;
		int work;
		work = min((this->hitEffect.animCnt / 4), sizeof(imageTable) / sizeof(imageTable[0]) - 1);
		rtv = imageTable[work];
		effectDirection(rtv, this->hitEffect.isFaceRight);
		return rtv;
	}

	Object::DrawInfo Object::guardEffectAnim()
	{
		Object::DrawInfo imageTable[]
		{
			{ML::Box2D(-32, -96, 64, 192), ML::Box2D(0  , 0, 128, 128)},
			{ML::Box2D(-32, -96, 64, 192), ML::Box2D(128, 0, 128, 128)},
			{ML::Box2D(-32, -96, 64, 192), ML::Box2D(256, 0, 128, 128)},
			{ML::Box2D(-32, -96, 64, 192), ML::Box2D(384, 0, 128, 128)},
			{ML::Box2D(-32, -96, 64, 192), ML::Box2D(512, 0, 128, 128)},
		};

		Object::DrawInfo rtv;
		int work;
		work = min((this->guardEffect.animCnt / 4), sizeof(imageTable) / sizeof(imageTable[0]) - 1);
		rtv = imageTable[work];
		effectDirection(rtv, this->guardEffect.isFaceRight);
		return rtv;
	}

	Object::DrawInfo Object::p1FbEffectAnim()
	{
		Object::DrawInfo imageTable[]
		{
			{ML::Box2D(-64, -64, 128, 128), ML::Box2D(320, 0, 64, 64)},
			{ML::Box2D(-64, -64, 128, 128), ML::Box2D(384, 0, 64, 64)},
			{ML::Box2D(-64, -64, 128, 128), ML::Box2D(448, 0, 64, 64)},
		};

		Object::DrawInfo rtv;
		int work;
		work = min((this->p1FBEffect.animCnt / 4), sizeof(imageTable) / sizeof(imageTable[0]) - 1);
		rtv = imageTable[work];
		effectDirection(rtv, this->p1FBEffect.isFaceRight);
		return rtv;
	}

	Object::DrawInfo Object::p2FbEffectAnim()
	{
		Object::DrawInfo imageTable[]
		{
			{ML::Box2D(-64, -64, 128, 128), ML::Box2D(320, 0, 64, 64)},
			{ML::Box2D(-64, -64, 128, 128), ML::Box2D(384, 0, 64, 64)},
			{ML::Box2D(-64, -64, 128, 128), ML::Box2D(448, 0, 64, 64)},
		};

		Object::DrawInfo rtv;
		int work;
		work = min((this->p2FBEffect.animCnt / 4), sizeof(imageTable) / sizeof(imageTable[0]) - 1);
		rtv = imageTable[work];
		effectDirection(rtv, this->p2FBEffect.isFaceRight);
		return rtv;
	}

	Object::DrawInfo Object::scoreEffectAnim()
	{
		Object::DrawInfo imageTable[]
		{
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 0   , 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 256 , 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 512 , 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 768 , 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 1024, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 1280, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 1536, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 1792, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 2048, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 2304, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 2560, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 2816, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 3072, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 3328, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 3584, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 3840, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 4096, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 4352, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 4608, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 4864, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 5120, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 5376, 256, 256)},
			{ML::Box2D(-32, -32, 64, 64),ML::Box2D(0, 5632, 256, 256)},
		};
		Object::DrawInfo rtv;
		int work;
		work = min((this->scoreEffect.animCnt / 8), sizeof(imageTable) / sizeof(imageTable[0]) - 1);
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