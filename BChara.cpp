//-----------------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-----------------------------------------------------------------------------
#include "BChara.h"
#include  "MyPG.h"

ML::Vec2 BChara::MoveSet(int key)
{
	//0, 1, 2,  3,  4,  5,  6, 7, 8, 9,10,
	int table[] = { -1,90,270,-1,180,135,225,-1, 0,45,315 };
	float spd = 10.5f;
	ML::Vec2 est;
	//0は特殊条件
	if (key == 0)
	{
		est.x = 0.f;
		est.y = 0.f;
	}
	else
	{
		est.x = (float)cos(table[key] * D3DX_PI / 180.f) * spd;
		est.y = (float)-sin(table[key] * D3DX_PI / 180.f) * spd;
	}
	return est;
}

bool BChara::UpdateMotion(Motion nm_)
{
	if (nm_ == this->motion)
	{
		return false;
	}
	else
	{
		this->motion = nm_;
		this->moveCnt = 0;
		this->animCnt = 0;
		return true;
	}
}

void BChara::CheckMove(ML::Vec2& e_)
{
	while (e_.x != 0) 
	{
		float  preX = this->pos.x;
		if (e_.x >= 1) 
		{
			this->pos.x += 1;	
			e_.x -= 1; 
		}
		else if (e_.x <= -1) 
		{ 
			this->pos.x -= 1;	
			e_.x += 1; 
		}
		else 
		{ 
			this->pos.x += e_.x;
			e_.x = 0; 
		}
		ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
		if (true == CheckHit(hit)) 
		{
			this->pos.x = preX;		//移動をキャンセル
			break;
		}
	}
	//縦軸に対する移動
	while (e_.y != 0) 
	{
		float  preY = this->pos.y;
		if (e_.y >= 1) 
		{ 
			this->pos.y += 1;	
			e_.y -= 1; 
		}
		else if (e_.y <= -1)
		{
			this->pos.y -= 1;	
			e_.y += 1; 
		}
		else 
		{ 
			this->pos.y += e_.y;
			e_.y = 0;
		}
		ML::Box2D hit = this->hitBase.OffsetCopy(this->pos);
		if (true == CheckHit(hit)) 
		{
			this->pos.y = preY;		//移動をキャンセル
			break;
		}
	}

}

bool BChara::CheckHit(const ML::Box2D& hit_)
{
	ML::Box2D yuka(-10, 550, 1300, 200);
	return yuka.Hit(hit_);
}

bool BChara::CheckFoot()
{
	ML::Box2D foot(this->hitBase.x, this->hitBase.y + this->hitBase.h, this->hitBase.w, 1);
	foot.Offset(this->pos);

	return this->CheckHit(foot);
}

void BChara::takeDamage(int dmg)
{
	addGaugeBar(5.0f);
	this->currentHp -= dmg;
	if (currentHp <= 0)
	{
		currentHp = 0;
	}
	
	this->UpdateMotion(Motion::Hurt);
}

int BChara::getAtkDamage(int basedmg)
{
	if (this->isPowerUp == true)
	{
		return basedmg * 2;
	}
	return basedmg;
}

void BChara::powerUpTimerUpdate() 
{
	if (this->powerUpTimer > 0)
	{
		this->powerUpTimer--;

		if (this->powerUpTimer <= 0)
		{
			this->isPowerUp = false;
		}
	}
}

bool BChara::isGaugeBarFull()
{
	return this->currentGauge >= this->MaxGauge;
}

void BChara::addGaugeBar(float gaugevalue)
{
	this->currentGauge += gaugevalue;

	if (this->currentGauge > this->MaxGauge)
	{
		this->currentGauge = this->MaxGauge;
	}
}

void BChara::useGaugeBar()
{
	this->currentGauge = 0.0f;
}
