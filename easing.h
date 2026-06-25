#pragma once
#include <string>
#include <vector>

namespace easing{
  enum class EQ_STATE { EQ_SET, EQ_START, EQ_PAUSE, EQ_RESTART, EQ_END };

	//利用できるイージング式一覧
	enum EASINGTYPE{
		LINEAR,
		BACKIN,
		BACKOUT,
		BACKINOUT,
		BOUNCEOUT,
		BOUNCEIN,
		BOUNCEINOUT,
		CIRCIN,
		CIRCOUT,
		CIRCINOUT,
		CUBICIN,
		CUBICOUT,
		CUBICINOUT,
		ELASTICIN,
		ELASTICOUT,
		ELASTICINOUT,
		EXPOIN,
		EXPOOUT,
		EXPOINOUT,
		QUADIN,
		QUADOUT,
		QUADINOUT,
		QUARTIN,
		QUARTOUT,
		QUARTINOUT,
		QUINTIN,
		QUINTOUT,
		QUINTINOUT,
		SINEIN,
		SINEOUT,
		SINEINOUT,
	};

	//-----------------------------------------------------------
	//イージングの初期設定を行う。
	//この時点では処理は行われない。
	//name　イージング名
	//type　イージング種類　esing.h参照
	//start	初期値
	//end		終了値
	//duration 持続フレームeasing
	//-----------------------------------------------------------
	void Set(const std::string& name, const EASINGTYPE type, const float start, const float end, const int duration,const std::string& next = "none");
  void Create(const std::string& name, const EASINGTYPE type, const float start, const float end, const int duration,const  std::string& next = "none");
  //-----------------------------------------------------------
	//イージングの開始処理を行う。
	//-----------------------------------------------------------
  void Start(const std::string& name);
	//-----------------------------------------------------------
	//途中経過値の取得
	//-----------------------------------------------------------
	void GetPos(const std::string& name, float& pos);
	float GetPos(const std::string& name);

	//-----------------------------------------------------------
	//イージング処理の更新　-　毎フレーム行うこと。
	//-----------------------------------------------------------
	void UpDate();
	//-----------------------------------------------------------
	//イージング処理の初期化　-　利用前に行うこと。
	//-----------------------------------------------------------
	void Init();
  //-----------------------------------------------------------
  //イージング処理の一時停止
  //-----------------------------------------------------------
  void Pause(const std::string& name);
  //-----------------------------------------------------------
  //イージング処理の再開
  //-----------------------------------------------------------
  void Resume(const std::string& name);
  //-----------------------------------------------------------
  //イージング処理のリセット
  //-----------------------------------------------------------
  void Reset(const std::string& name);
  //-----------------------------------------------------------
  //イージング処理の状態取得
  //-----------------------------------------------------------
  EQ_STATE GetState(const std::string& name);

}


