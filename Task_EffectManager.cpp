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
		this->imgP1CExpodeEffect = DG::Image::Create("./data/image/ChargeExplodeEffect.png");
		this->imgP2CExpodeEffect = DG::Image::Create("./data/image/ChargeExplodeEffect.png");
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
		this->imgP1CExpodeEffect.reset();
		this->imgP2CExpodeEffect.reset();
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
		this->p1hitEffect.updateEffect(4, 5);
		this->p1guardEffect.updateEffect(4, 5);
		this->p2hitEffect.updateEffect(4, 5);
		this->p2guardEffect.updateEffect(4, 5);
		this->p1FBEffect.updateEffect(4, 3);
		this->p2FBEffect.updateEffect(4, 3);
		this->p1CExpodeEffect.updateEffect(4, 10);
		this->p2CExpodeEffect.updateEffect(4, 10);
		this->p1ScoreEffect.updateEffect(4, 9);
		this->p2ScoreEffect.updateEffect(4, 9);
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//p1 eff
		Object::DrawInfo p1hitEffectDraw = this->hitEffectAnim(this->p1hitEffect);
		if (p1hitEffect.isPlaying)
		{
			p1hitEffectDraw.draw.Offset(p1hitEffect.pos);
			this->res->imgHitEffect->Draw(p1hitEffectDraw.draw, p1hitEffectDraw.src);
		}
		
		Object::DrawInfo guardEffectDraw = this->guardEffectAnim(this->p1guardEffect);
		if (p1guardEffect.isPlaying)
		{
			guardEffectDraw.draw.Offset(p1guardEffect.pos);
			this->res->imgGuardEffect->Draw(guardEffectDraw.draw, guardEffectDraw.src);
		}
		
		//p2 eff
		Object::DrawInfo p2hitEffectDraw = this->hitEffectAnim(this->p2hitEffect);
		if (p2hitEffect.isPlaying)
		{
			p2hitEffectDraw.draw.Offset(p2hitEffect.pos);
			this->res->imgHitEffect->Draw(p2hitEffectDraw.draw, p2hitEffectDraw.src);
		}

		Object::DrawInfo p2guardEffectDraw = this->guardEffectAnim(this->p2guardEffect);
		if (p2guardEffect.isPlaying)
		{
			p2guardEffectDraw.draw.Offset(p2guardEffect.pos);
			this->res->imgGuardEffect->Draw(p2guardEffectDraw.draw, p2guardEffectDraw.src);
		}

		//fireball eff
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
		
		//explode eff
		Object::DrawInfo p1CExpodeEffectDraw = this->p1CExpodeEffectAnim();
		if (p1CExpodeEffect.isPlaying)
		{
			p1CExpodeEffectDraw.draw.Offset(p1CExpodeEffect.pos);
			this->res->imgP1CExpodeEffect->Draw(p1CExpodeEffectDraw.draw, p1CExpodeEffectDraw.src, ML::Color(1, 1, 1, 1));
		}

		Object::DrawInfo p2CExpodeEffectDraw = this->p2CExpodeEffectAnim();
		if (p2CExpodeEffect.isPlaying)
		{
			p2CExpodeEffectDraw.draw.Offset(p2CExpodeEffect.pos);
			this->res->imgP2CExpodeEffect->Draw(p2CExpodeEffectDraw.draw, p2CExpodeEffectDraw.src, ML::Color(1, 0, 0, 1));
		}

		//Score eff
		Object::DrawInfo p1ScoreEffectDraw = this->scoreEffectAnim(this->p1ScoreEffect);
		if (p1ScoreEffect.isPlaying)
		{
			p1ScoreEffectDraw.draw.Offset(p1ScoreEffect.pos);
			this->res->imgScoreEffect->Draw(p1ScoreEffectDraw.draw, p1ScoreEffectDraw.src);
		}

		Object::DrawInfo p2ScoreEffectDraw = this->scoreEffectAnim(this->p2ScoreEffect);
		if (p2ScoreEffect.isPlaying)
		{
			p2ScoreEffectDraw.draw.Offset(p2ScoreEffect.pos);
			this->res->imgScoreEffect->Draw(p2ScoreEffectDraw.draw, p2ScoreEffectDraw.src);
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

	void Object::EffectInfo::playEffect(ML::Box2D pos)
	{
		this->pos.x = pos.x;
		this->pos.y = pos.y;
		this->animCnt = 0;
		this->isPlaying = true;
	}

	void Object::EffectInfo::playEffect(ML::Vec2 pos)
	{
		this->pos.x = pos.x;
		this->pos.y = pos.y;
		this->animCnt = 0;
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

	Object::DrawInfo Object::hitEffectAnim(EffectInfo target)
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
		work = min((target.animCnt / 4), sizeof(imageTable) / sizeof(imageTable[0]) - 1);
		rtv = imageTable[work];
		effectDirection(rtv, target.isFaceRight);
		return rtv;
	}

	Object::DrawInfo Object::guardEffectAnim(EffectInfo target)
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
		work = min((target.animCnt / 4), sizeof(imageTable) / sizeof(imageTable[0]) - 1);
		rtv = imageTable[work];
		effectDirection(rtv, target.isFaceRight);
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

	Object::DrawInfo Object::p1CExpodeEffectAnim()
	{
		Object::DrawInfo imageTable[]
		{
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(0  , 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(96 , 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(192, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(288, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(384, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(480, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(576, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(672, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(768, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(864, 0, 96, 96)},
		};

		Object::DrawInfo rtv;
		int work;
		work = min((this->p1CExpodeEffect.animCnt / 4), sizeof(imageTable) / sizeof(imageTable[0]) - 1);
		rtv = imageTable[work];
		return rtv;
	}

	Object::DrawInfo Object::p2CExpodeEffectAnim()
	{
		Object::DrawInfo imageTable[]
		{
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(0  , 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(96 , 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(192, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(288, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(384, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(480, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(576, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(672, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(768, 0, 96, 96)},
			{ML::Box2D(-128, -128, 256, 256), ML::Box2D(864, 0, 96, 96)},
		};

		Object::DrawInfo rtv;
		int work;
		work = min((this->p2CExpodeEffect.animCnt / 4), sizeof(imageTable) / sizeof(imageTable[0]) - 1);
		rtv = imageTable[work];
		return rtv;
	}

	Object::DrawInfo Object::scoreEffectAnim(EffectInfo target)
	{
		Object::DrawInfo imageTable[]
		{
			{ML::Box2D(-12, -12, 72, 72),ML::Box2D(384, 128, 128, 128)},
			{ML::Box2D(-12, -12, 72, 72),ML::Box2D(256, 128, 128, 128)},
			{ML::Box2D(-12, -12, 72, 72),ML::Box2D(128, 128, 128, 128)},
			{ML::Box2D(-12, -12, 72, 72),ML::Box2D(0  , 128, 128, 128)},
			{ML::Box2D(-12, -12, 72, 72),ML::Box2D(512, 0  , 128, 128)},
			{ML::Box2D(-12, -12, 72, 72),ML::Box2D(384, 0  , 128, 128)},
			{ML::Box2D(-12, -12, 72, 72),ML::Box2D(256, 0  , 128, 128)},
			{ML::Box2D(-12, -12, 72, 72),ML::Box2D(128, 0  , 128, 128)},
			{ML::Box2D(-12, -12, 72, 72),ML::Box2D(0  , 0  , 128, 128)},
		};
		Object::DrawInfo rtv;
		int work;
		work = min((target.animCnt / 4), sizeof(imageTable) / sizeof(imageTable[0]) - 1);
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