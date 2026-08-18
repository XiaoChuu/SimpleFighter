
#define _USE_MATH_DEFINES
#include <cmath>			//M_PI
#include <algorithm>
#include <map>
#include "easing.h"

// floatの円周率
const float m_pi = static_cast<float>(M_PI);

// 値を [min_value, max_value] にする
template <typename T>
T clamp(const T& value, const T& min_value, const T& max_value)
{
	return std::max(std::min(value, max_value), min_value);
}


//
// さまざまなイージング関数
// t = 経過時間 b = 始点 c = 終点 - 始点 d = 持続時間
//
static float EasingLinear(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);
	return c * t / d + b;
}


static float EasingBackIn(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	float s = 1.70158f;
	t /= d;
	return c * t * t * ((s + 1) * t - s) + b;
}

static float EasingBackOut(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	float s = 1.70158f;
	t = t / d - 1;
	return c * (t * t * ((s + 1) * t + s) + 1) + b;
}

static float EasingBackInOut(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	float s = 1.70158f * 1.525f;
	if ((t /= d / 2) < 1) return c / 2 * (t * t * ((s + 1) * t - s)) + b;
	t -= 2;
	return c / 2 * (t * t * ((s + 1) * t + s) + 2) + b;
}


static float EasingBounceOut(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	if ((t /= d) < (1 / 2.75f)) {
		return c * (7.5625f * t * t) + b;
	}
	else if (t < (2 / 2.75f)) {
		t -= (1.5f / 2.75f);
		return c * (7.5625f * t * t + .75f) + b;
	}
	else if (t < (2.5f / 2.75f)) {
		t -= (2.25f / 2.75f);
		return c * (7.5625f * t * t + .9375f) + b;
	}
	else {
		t -= (2.625f / 2.75f);
		return c * (7.5625f * t * t + .984375f) + b;
	}
}

static float EasingBounceIn(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);
	return c - EasingBounceOut(d - t, 0.0f, c, d) + b;
}

static float EasingBounceInOut(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	if (t < d / 2) return EasingBounceIn(t * 2, 0.0f, c, d) * .5f + b;
	else         return EasingBounceOut(t * 2 - d, 0.0f, c, d) * .5f + c * .5f + b;
}


static float EasingCircIn(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	t /= d;
	return -c * (std::sqrt(1 - t * t) - 1) + b;
}

static float EasingCircOut(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	t = t / d - 1;
	return c * std::sqrt(1 - t * t) + b;
}

static float EasingCircInOut(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	if ((t /= d / 2) < 1) return -c / 2 * (std::sqrt(1 - t * t) - 1) + b;
	t -= 2;
	return c / 2 * (std::sqrt(1 - t * t) + 1) + b;
}


static float EasingCubicIn(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	t /= d;
	return c * t * t * t + b;
}

static float EasingCubicOut(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	t = t / d - 1;
	return c * (t * t * t + 1) + b;
}

static float EasingCubicInOut(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	if ((t /= d / 2) < 1) return c / 2 * t * t * t + b;
	t -= 2;
	return c / 2 * (t * t * t + 2) + b;
}


static float EasingElasticIn(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	if (t == 0)      return b;
	if ((t /= d) == 1) return b + c;
	float p = d * 0.3f;

	float a = c;
	float s = p / 4;
	t -= 1;
	return -(a * std::pow(2.0f, 10.0f * t) * std::sin((t * d - s) * (2.0f * m_pi) / p)) + b;
}

static float EasingElasticOut(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	if (t == 0)      return b;
	if ((t /= d) == 1) return b + c;
	float p = d * 0.3f;

	float a = c;
	float s = p / 4;
	return (a * std::pow(2.0f, -10.0f * t) * std::sin((t * d - s) * (2.0f * m_pi) / p) + c + b);
}

static float EasingElasticInOut(float t, const float b, const float c, const float d)
{
	t = clamp(t, 0.0f, d);

	if (t == 0) return b;
	if ((t /= d / 2) == 2) return b + c;
	float p = d * (0.3f * 1.5f);

	float a = c;
	float s = p / 4;
	if (t < 1)
	{
		t -= 1;
		return -0.5f * (a * std::pow(2.0f, 10.0f * t) * std::sin((t * d - s) * (2.0f * m_pi) / p)) + b;
	}
	t -= 1;
	return a * std::pow(2.0f, -10.0f * t) * std::sin((t * d - s) * (2.0f * m_pi) / p) * 0.5f + c + b;
}

static float EasingExpoIn(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);
	return (t == 0) ? b : c * std::pow(2.0f, 10.0f * (t / d - 1.0f)) + b;
}

static float EasingExpoOut(float t, const float b, const float c, const float d) {
	return (t == d) ? b + c : c * (-std::pow(2.0f, -10.0f * t / d) + 1.0f) + b;
}

static float EasingExpoInOut(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);

	if (t == 0)         return b;
	if (t == d)         return b + c;
	if ((t /= d / 2) < 1) return c / 2 * std::pow(2.0f, 10.0f * (t - 1.0f)) + b;
	return c / 2 * (-std::pow(2.0f, -10.0f * --t) + 2.0f) + b;
}


static float EasingQuadIn(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);

	t /= d;
	return c * t * t + b;
}

static float EasingQuadOut(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);

	t /= d;
	return -c * t * (t - 2) + b;
}

static float EasingQuadInOut(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);

	if ((t /= d / 2) < 1) return c / 2 * t * t + b;
	--t;
	return -c / 2 * (t * (t - 2) - 1) + b;
}


static float EasingQuartIn(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);

	t /= d;
	return c * t * t * t * t + b;
}

static float EasingQuartOut(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);

	t = t / d - 1;
	return -c * (t * t * t * t - 1) + b;
}

static float EasingQuartInOut(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);

	if ((t /= d / 2) < 1) return c / 2 * t * t * t * t + b;
	t -= 2;
	return -c / 2 * (t * t * t * t - 2) + b;
}


static float EasingQuintIn(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);

	t /= d;
	return c * t * t * t * t * t + b;
}

static float EasingQuintOut(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);

	t = t / d - 1;
	return c * (t * t * t * t * t + 1) + b;
}

static float EasingQuintInOut(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);

	if ((t /= d / 2) < 1) return c / 2 * t * t * t * t * t + b;
	t -= 2;
	return c / 2 * (t * t * t * t * t + 2) + b;
}


static float EasingSineIn(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);
	return -c * std::cos(t / d * (m_pi / 2)) + c + b;
}

static float EasingSineOut(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);
	return c * std::sin(t / d * (m_pi / 2)) + b;
}

static float EasingSineInOut(float t, const float b, const float c, const float d) {
	t = clamp(t, 0.0f, d);
	return -c / 2 * (std::cos(m_pi * t / d) - 1) + b;
}

typedef float(*EasingEquationsFunction)(float t, const float b, const float c, const float d);

EasingEquationsFunction eeqfunc[] =
{
	EasingLinear,
	EasingBackIn,
	EasingBackOut,
	EasingBackInOut,
	EasingBounceOut,
	EasingBounceIn,
	EasingBounceInOut,
	EasingCircIn,
	EasingCircOut,
	EasingCircInOut,
	EasingCubicIn,
	EasingCubicOut,
	EasingCubicInOut,
	EasingElasticIn,
	EasingElasticOut,
	EasingElasticInOut,
	EasingExpoIn,
	EasingExpoOut,
	EasingExpoInOut,
	EasingQuadIn,
	EasingQuadOut,
	EasingQuadInOut,
	EasingQuartIn,
	EasingQuartOut,
	EasingQuartInOut,
	EasingQuintIn,
	EasingQuintOut,
	EasingQuintInOut,
	EasingSineIn,
	EasingSineOut,
	EasingSineInOut,
};


typedef struct tagEasingEquations
{
	easing::EQ_STATE	eq_state;	//計算終了フラグ
	easing::EASINGTYPE	type;		//イージングの種類
	float		pos;		//現在位置
	float		start;		//開始
	float		end;		//終了
	int			frame;		//現在のフレーム
	int			duration;	//持続フレーム

	std::string nextname;
}EasingEquations;

static std::map<std::string, EasingEquations> easingList;

//-----------------------------------------------------------
//イージングの初期設定を行う。
//この時点では処理は行われない。Startで開始になる
//name　イージング名
//type　イージング種類　esing.h参照
//start	初期値
//end		終了値
//duration 持続フレーム
//-----------------------------------------------------------
void easing::Set(const std::string& name, easing::EASINGTYPE type, float start, float end, int duration, const std::string& next)
{
	EasingEquations	easing = {};
	easing.start = start;
	easing.end = end;
	easing.duration = duration;
	easing.eq_state = EQ_STATE::EQ_SET;
	easing.type = type;
	//初期値を代入
	easing.pos = eeqfunc[easing.type]((float)easing.frame, easing.start, easing.end - easing.start, (float)easing.duration);
	easing.nextname = next;
	easingList[name] = easing;
}

//-----------------------------------------------------------
//イージングの初期設定を行う。
//この時点では処理は行われない。Startで開始になる
//name　イージング名
//type　イージング種類　esing.h参照
//start	初期値
//end		終了値
//duration 持続フレーム
//-----------------------------------------------------------
void easing::Create(const std::string& name, easing::EASINGTYPE type, float start, float end, int duration, const std::string& next)
{
	EasingEquations	easing = {};
	easing.start = start;
	easing.end = end;
	easing.duration = duration;
	easing.eq_state = EQ_STATE::EQ_SET;
	easing.type = type;
	//初期値を代入
	easing.pos = eeqfunc[easing.type]((float)easing.frame, easing.start, easing.end - easing.start, (float)easing.duration);
	easing.nextname = next;
	easingList[name] = easing;
}

//-----------------------------------------------------------
//イージングの開始処理を行う。
//-----------------------------------------------------------
void easing::Init() {
	easingList.clear();
}

//-----------------------------------------------------------
//途中経過値の取得
//-----------------------------------------------------------
void easing::GetPos(const std::string& name, float& pos) {
	auto& it = easingList[name];
	if (it.eq_state == EQ_STATE::EQ_START ||
		it.eq_state == EQ_STATE::EQ_END) {
		pos = it.pos;
		return;
	}
}

//-----------------------------------------------------------
//途中経過値の取得
//-----------------------------------------------------------
float easing::GetPos(const std::string& name) {
	//  auto &it = easingList.find(name);
	auto& it = easingList[name];
	return it.pos;
}

//-----------------------------------------------------------
//イージング処理の更新　-　毎フレーム一回行うこと。
//-----------------------------------------------------------
void easing::UpDate()
{
	for (auto& it : easingList) {
		auto& esing = it.second;
		if (esing.eq_state == EQ_STATE::EQ_START) {
			esing.frame++;
			esing.pos = eeqfunc[esing.type]((float)esing.frame, esing.start, esing.end - esing.start, (float)esing.duration);
			if (esing.duration == esing.frame) {
				esing.eq_state = EQ_STATE::EQ_END;
				if (esing.nextname != "none")
				{
					easing::Reset(esing.nextname);
					easing::Start(esing.nextname);
				}
			}
		}
	}
}

//-----------------------------------------------------------
//イージングの開始処理を行う。
//-----------------------------------------------------------
void easing::Start(const std::string& name) {
	auto it = easingList.find(name);
	//見つかったら
	if (it != easingList.end()) {
		it->second.eq_state = EQ_STATE::EQ_START;
	}
}

//-----------------------------------------------------------
//イージングの一時停止処理を行う。
//-----------------------------------------------------------
void easing::Pause(const std::string& name) {
	auto it = easingList.find(name);
	//見つかったら
	if (it != easingList.end()) {
		it->second.eq_state = EQ_STATE::EQ_PAUSE;
	}
}

//-----------------------------------------------------------
//イージングの一時停止処理を行う。
//-----------------------------------------------------------
void easing::Resume(const std::string& name) {
	auto it = easingList.find(name);
	//見つかったら
	if (it != easingList.end()) {
		it->second.eq_state = EQ_STATE::EQ_START;
	}
}

//-----------------------------------------------------------
//イージングの一時停止処理を行う。
//-----------------------------------------------------------
void easing::Reset(const std::string& name) {
	auto it = easingList.find(name);
	//見つかったら
	if (it != easingList.end()) {
		it->second.eq_state = EQ_STATE::EQ_SET;
		it->second.frame = 0;
		it->second.pos = it->second.start;
	}
}

//-----------------------------------------------------------
//イージングの状態取得処理を行う。
//-----------------------------------------------------------
easing::EQ_STATE easing::GetState(const std::string& name) {
	auto it = easingList.find(name);
	//見つかったら
	if (it != easingList.end()) {
		return it->second.eq_state;
	}
	return EQ_STATE::EQ_END;
}



