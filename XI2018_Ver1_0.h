#pragma warning(disable:4996)
#pragma once
#include <windows.h>
#pragma comment(lib, "xinput.lib")
#include <XInput.h> 
#define DIRECTINPUT_VERSION	0x0800
#pragma comment(lib,"dxguid")
#pragma comment(lib,"dInput8")
#include <dInput.h>
#include "myLib.h"
#include <string>
#include <vector>

//┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//┃入力制御系機能群							　　┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
namespace XI
{
	struct  ButtonInfo {
		bool  on = false;
		bool  off = true;
		bool  down = false;
		bool  up = false;
		//ボタンをONに書き換える
		void  SetOn() {   on = 1; off = 0; down = 0; up = 0; }
		//ボタンをOFFに書き換える
		void  SetOff() {  on = 0; off = 1; down = 0; up = 0; }
		//ボタンをDOWNに書き換える
		void  SetDown() { on = 1; off = 0; down = 1; up = 0; }
		//ボタンをUPに書き換える
		void  SetUp() {   on = 0; off = 1; down = 0; up = 1; }
	};
	struct  StickInfo {
		float                angleDYP;	//アナログスティックの角度（単位はラジアン）（下が90度とする）
		float                angleUYP;	//アナログスティックの角度（単位はラジアン）（上が90度とする）
		float                volume;	//アナログスティックの強弱0.0f～1.0f
		ML::Vec2             axis;		//軸毎の量-1.0f～+1.0f
		ButtonInfo			 BL;
		ButtonInfo			 BR;
		ButtonInfo			 BU;
		ButtonInfo			 BD;
		StickInfo() {
			this->angleDYP = 0;
			this->angleUYP = 0;
			this->volume = 0;
			this->axis = ML::Vec2(0,0);
			this->BL.SetOff();
			this->BR.SetOff();
			this->BU.SetOff();
			this->BD.SetOff();
		}
	};
	struct  TriggerInfo {
		float				 volumeL;//左トリガーの押された量
		float				 volumeR;//右トリガーの押された量
		ButtonInfo			 L2;//ボタンとしてみる場合
		ButtonInfo			 R2;//ボタンとしてみる場合
		TriggerInfo() {
			this->volumeL = 0;
			this->volumeR = 0;
			this->L2.SetOff();
			this->R2.SetOff();
		}
	};
	//PG上で使用する時のボタンの名前
	enum class VGP : BYTE{
		//xbox=Ａ ps2=×
		B1,
		//xbox=Ｂ ps2=○
		B2,
		//xbox=Ｘ ps2=□
		B3,
		//xbox=Ｙ ps2=△
		B4,
		//xbox=LB  ps2=L1
		L1,
		//xbox=RB  ps2=R1
		R1,
		//xbox=Menue　ps2=SELECT
		SE,
		//xbox=View　ps2=START
		ST,
		//ps2=L3
		L3,
		//ps2=R3
		R3,
		//ps2=←
		HL,
		//ps2=→
		HR,
		//ps2=↑
		HU,
		//ps2=↓
		HD,
		//拡張
		S0, S1, S2, S3, S4, S5, S6, S7, S8, S9,
		KMax		//　ボタン総数
	};

	//「ＵＳＢゲームコントローラーのセットアップ」で表示されるボタンに対応
	//した、物理的なゲームパットのボタンに対応するフラグ
	enum class RGP : BYTE {
		B01, B02, B03, B04, B05, B06, B07, B08,
		B09, B10, B11, B12, B13, B14, B15, B16,
		HSL, HSR, HSU, HSD,		//ハットスイッチ 
		KMax					//　ボタン総数
	};


	struct  AnalogAxisKB {
		int           vKeyL;	//キーボードのバーチャルキー
		int           vKeyR;	//キーボードのバーチャルキー
		int           vKeyU;	//キーボードのバーチャルキー
		int           vKeyD;	//キーボードのバーチャルキー
	};
	struct  AnalogTriggerKB {
		int           vKeyL;	//キーボードのバーチャルキー
		int           vKeyR;	//キーボードのバーチャルキー
	};
	//キー設定キーボード用
	struct  KD_KB {
		int			vKey;	//キーボードのバーチャルキー
		VGP			pKey;	//ボタンの種類（ＰＧ上での名称）
	};
	using KeyDatas_KB = vector<KD_KB>;

	//キー設定ゲームパッド用
	struct  KD_GP {
		RGP			gKey;	//ゲームパッドのボタン
		VGP			pKey;	//ボタンの種類（ＰＧ上での名称）
	};
	using KeyDatas_GP = vector<KD_GP>;

	//例外として投げられるエラー情報
	enum class Err : BYTE {
		NOTHING,		//エラーなし
		CREATE,			//生成失敗
		NOTDEVICE,		//デバイスが取得的ない
		UNNONTYPE,		//不明なデバイス種類
		ENUMDEVICE,		//デバイスの列挙に失敗
		CREATEDEVICE,	//デバイスの生成失敗
		DATAFORMAT,		//データフォーマットの設定失敗
		COOPLEVEL,		//協調レベル設定失敗
		GPAXIS,			//
		KBPROPERTY,
	};
	//仮想コントローラ構造体
	struct  VGamePad {
		StickInfo  LStick;
		StickInfo  RStick;
		TriggerInfo Trigger;
		ButtonInfo  B1;
		ButtonInfo  B2;
		ButtonInfo  B3;
		ButtonInfo  B4;
		ButtonInfo  L1;
		ButtonInfo  R1;
		ButtonInfo  L3;
		ButtonInfo  R3;
		ButtonInfo  SE;
		ButtonInfo  ST;
		ButtonInfo  HL;
		ButtonInfo  HR;
		ButtonInfo  HU;
		ButtonInfo  HD;
		ButtonInfo  S0;
		ButtonInfo  S1;
		ButtonInfo  S2;
		ButtonInfo  S3;
		ButtonInfo  S4;
		ButtonInfo  S5;
		ButtonInfo  S6;
		ButtonInfo  S7;
		ButtonInfo  S8;
		ButtonInfo  S9;
	};
	//GamePad----------------------------------------------------------------
	class  GamePad {
	protected:
		enum  class  ButtonState : BYTE {
			Err,
			On,		//押している間中
			Off,	//離している間中
			Down,	//押した直後（１フレームのみ反応）
			Up,		//離した直後（１フレームのみ反応）
		};
		struct  Stick{
			ML::Vec2             axis;		//軸毎の量-1.0f～+1.0f
			ButtonState			 BL;
			ButtonState			 BR;
			ButtonState			 BU;
			ButtonState			 BD;
			Stick() {
				this->axis = ML::Vec2(0, 0);
				this->BL = ButtonState::Off;
				this->BR = ButtonState::Off;
				this->BU = ButtonState::Off;
				this->BD = ButtonState::Off;
			}
		};
		struct  Trigger{
			float				 volumeL;//左トリガーの押された量
			float				 volumeR;//右トリガーの押された量
			ButtonState			 L2;//ボタンとしてみる場合
			ButtonState			 R2;//ボタンとしてみる場合
			Trigger() {
				this->volumeL = 0;
				this->volumeR = 0;
				this->L2 = ButtonState::Off;
				this->R2 = ButtonState::Off;
			}
		};
	public:
		using SP = shared_ptr<GamePad>;
		using WP = weak_ptr<GamePad>;
	protected:
		SP				link;
		Stick			ls, rs;
		Trigger			tg;
	public:
		GamePad(void);
		static  SP  CreateKB(
			const AnalogAxisKB&			axisL_,
			const AnalogAxisKB&			axisR_,
			const AnalogTriggerKB&		trigger_,
			const KeyDatas_KB&			buttonKB_);
		static  SP  CreateGP(
			const DWORD&				id_,//0～3まで
			const KeyDatas_GP&			buttonGP_);
		//	更新
		void  UpDate();
		virtual  void  B_UpDate() = 0;
		//２つのデバイスを統合する
		//片方のデバイスが生成できていない場合の対応あり
		static  GamePad::SP  Link(SP  a, SP  b);
		//ゲームパッドのボタンの状態を取得する
		VGamePad  GetState();
		virtual  void  SetVibrationL(float  pow_, int  frame_);
		virtual  void  SetVibrationR(float  pow_, int  frame_);
		virtual ButtonState  CheckBT(VGP  kk_) = 0;
	private:
	protected:
		SP  Link(SP  l) {
			if (this == l.get()) { return 0; }
			else { SP w = link; link = l; return link; }
		}
		//ボタン
		ButtonState  UpdateButtonState(bool  nbs_, ButtonState  old_);
		ButtonState  IntegrationKey(ButtonState  a_, ButtonState  b_);
		ButtonInfo  ConvertKS(ButtonState  ks_);

		//アナログスティック
		void  SIUpdate(Stick&  si_, float  ax_, float  ay_);
		StickInfo  StickL();
		StickInfo  StickR();
		void  ConvertSI(StickInfo&  si_, Stick&  s_);

		void  TGUpdate(Trigger&  tg_, float  lt_, float  rt_);
		TriggerInfo  TriggerLR();
		void  ConvertTG(TriggerInfo&  ti_, Trigger&  t_);
	};
	//--------------------------------
	//仮想マウス構造体
	struct  VMouse {
		ML::Point         pos;
		ML::Point         pos_Screen;
		int			  wheel;
		ButtonInfo	  LB;
		ButtonInfo	  RB;
		ButtonInfo	  CB;
		ButtonInfo	  B1;
		ButtonInfo	  B2;
		ButtonInfo	  B3;
		ButtonInfo	  B4;
		ButtonInfo	  B5;
	};
	class Mouse {
	protected:
		enum  class  ButtonState : BYTE {
			Err,
			On,		//押している間中
			Off,	//離している間中
			Down,	//押した直後（１フレームのみ反応）
			Up,		//離した直後（１フレームのみ反応）
		};
		enum  class  MB : BYTE {		//マウスのボタンを表す
			//マウスボタンの種類 8個まで認識可能
			LB, RB, CB, RESERVED1,
			RESERVED2, RESERVED3, RESERVED4, RESERVED5,
			kMax
		};
	private:
		LPDIRECTINPUTDEVICE8 mouse;
		DIMOUSESTATE2 mouseState;
		ML::Point		  pos_Screen;	//スクリーン座標
		ML::Point		  pos;			//クライアント座標
		long          mouseWheel;
		ButtonState      mouseButton[(int)MB::kMax];
		Err			     init(float, float);
		float			 scaleX, scaleY;
	public:
		using SP = shared_ptr<Mouse>;
		using WP = weak_ptr<Mouse>;

	private:
		ButtonState     CheckBT(MB  mb_);
		Mouse();
		ButtonInfo  ConvertKS(ButtonState  ks_);
	public:
		~Mouse();
		//生成
		static SP   Create(float scaleX_ = 1.f, float scaleY_ = 1.f);
		//	更新
		void UpDate();
		//
		VMouse  GetState();
	};
	//---------------------------------------------------------------------------
	//XIオブジェクト
	class  Obj
	{
	public:
		using WP = weak_ptr<Obj>;
		using SP = shared_ptr<Obj>;
	private:
		static  Obj::WP  winst;		//	１つしか存在できないインスタンスのウィークポインタ
		IDirectInput8*  di;	//マウス利用の為残した
		HWND  wnd;

		//各種コンストラクタ封印
		Obj(const Obj&);
		Obj(const Obj&&);
		Obj();

		bool  Initialize(HWND wnd_);

		static  vector<GamePad::WP>  gpads;
		static  vector<Mouse::WP>    mouses;

	public:
		~Obj();
		static  Obj::SP  GetInst();
		//	XI XIライブラリを生成する
		static  Obj::SP  Create(HWND  wnd_);
		HWND  Wnd() { return  wnd; }
		IDirectInput8&  DInput() { return  *di; }
		//毎フレーム呼び出し
		void UpDate();
		
		void Set(GamePad::SP s_);
		void Set(Mouse::SP s_);
	};

	//---------------------------------------------------------------------------
	//ゲームパッドマネージャー
	class  GamePadManager {
	private:
		static  map<string, XI::GamePad::SP>	arr;
		static  bool notObject;
		GamePadManager() { notObject = false; }
	public:
		//登録
		static  void  Set(const string& name_, XI::GamePad::SP sp_);
		//名前で検索
		static  XI::GamePad::SP  Get(const string& name_);
		//単体破棄
		static  void  Clear(const string& name_);
		//すべて破棄
		static  void  AllClear();
	};
	//---------------------------------------------------------------------------
	//マウスマネージャー
	class  MouseManager {
	private:
		static  map<string, XI::Mouse::SP>	arr;
		static  bool notObject;
		MouseManager() { notObject = false; }
	public:
		//登録
		static  void  Set(const string& name_, XI::Mouse::SP sp_);
		//名前で検索
		static  XI::Mouse::SP  Get(const string& name_);
		//単体破棄
		static  void  Clear(const string& name_);
		//すべて破棄
		static  void  AllClear();
	};
}