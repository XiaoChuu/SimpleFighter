#pragma once
#pragma warning(disable:4996)
#pragma once
//-----------------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-----------------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BChara : public BTask
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラクタ共通メンバ変数
	ML::Vec2    pos;		//キャラクタ位置
	ML::Box2D   hitBase;	//あたり判定範囲
	ML::Vec2	moveVec;	//移動ベクトル
	int			moveCnt;	//行動カウンタ
	ML::Box2D   attackBox;
	//向き（2D視点）
	//float angle;
	enum class Angle_LR
	{
		Left,
		Right
	};

	enum class Motion
	{
		Unnon = -1,
		Stand,
		ForwardWalk,
		BackWalk,
		AttackPunch,
		AttackKick,
		AttackCharging,
		AttackChargeAtk,
		NaturalJump,
		ForwardJump,
		BackJump,
		NaturalFall,
		ForwardFall,
		BackFall,
		ForwardDash,
		BackDash,
		NaturalJumpAtk,
		ForwardJumpAtk,
		BackJumpAtk,
		Hurt,
		Guard,
		Down,
		Standing,
		KO,
		Hadouken,
		Shoryuken,
		PowerUP,
	};

	Motion motion;
	int animCnt;
	Angle_LR angle;
	float jumpPow;
	float maxFallSpeed;
	float gravity;
	float maxSpeed;
	float addSpeed;
	float decSpeed;

	bool isAttacking;
	bool isPowerUp;

	const int maxHp = 100;
	int currentHp;
	int attackdmg;
	const float maxPowerUpTime = 600;
	float powerUpTimer = 0;
	const float MaxGauge = 100.0f;
	float currentGauge;

	int downTimer = 0;

	int forwardDashTimer = -1;
	int BackDashTimer = -1;
	bool isForwardDash = false;
	bool isBackDash = false;

	bool isUsedPowerUp;
	bool isUsedShoryu;
	bool isUsedHadouken;

	//HP shake
	int shakeX, shakeY;
	const float shakeMaxTime = 10;
	const float shakeMaxPower = 10;
	int shakeTimer;

	//HP DelayDisplay
	int displayDelayHP;
	const int delayHPMaxTime = 30;
	int delayHPTimer;

	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加する事★★
	BChara()
		: pos(0, 0)
		, hitBase(0, 0, 0, 0)
		, moveVec(0, 0)
		, moveCnt(0)
		, attackBox(0,0,0,0)
		, motion(Motion::Stand)//angle(0.f)
		, angle(Angle_LR::Right)
		, gravity(0)
		, maxSpeed(0)
		, addSpeed(0)
		, decSpeed(0)
		, currentHp(0)
	{
	}
	virtual  ~BChara() {}

	void CheckMove(ML::Vec2& e_);
	bool CheckHit(const ML::Box2D& hit_);
	bool CheckFoot();
	bool UpdateMotion(Motion nm_);

	void takeDamage(int dmg);
	void takeGuardDamage(int dmg, bool isSpecial);
	void takeShoryuDamage(int dmg);
	void takeChargeAtkDamange(int dmg);
	int getAtkDamage(int basedmg);
	void shakeHPBar(int& sx, int& sy);
	void delayDamageHPBarUpdate();

	void powerUpTimerUpdate();
	bool isGaugeBarFull();
	void addGaugeBar(float gaugevalue);
	void useGaugeBar();
	bool isAtkCharging();

	void resetPlayer(int x, int y);
	
	struct DrawInfo
	{
		ML::Box2D draw, src;
	};

protected:
	ML::Vec2 MoveSet(int key);
};
