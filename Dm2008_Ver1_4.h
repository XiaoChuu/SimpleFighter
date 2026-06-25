
#pragma comment(lib,"dxguid")
#pragma once

#include <windows.h>
#include <string>
#include <dsound.h>
#include "myLib.h"




namespace DM 
{
	const  DWORD  kSTANDERDSIZE = 88200;	//	基準サイズ（バッファサイズの半分に等しい）
	const  DWORD  kVOLUME_MIN = (DSBVOLUME_MIN + 5000);		//	DSBVOLUME_MINが小さすぎるため、自分で管理

																//	エラー番号
	namespace  Err {
		enum Type
		{
			WRG,		//	ファイルが異常です
			CCIM,		//	DirectMusicの作成に失敗
			CCIS,		//	DirectSoundの作成に失敗
			INIT,		//	Initializeに失敗
			SCL,		//	SetCooperativeLevelに失敗
			CSB,		//	バッファ作成に失敗
			QI,			//	QueryInterfaceに失敗
			SFMT,		//	フォーマット設定に失敗
			IA,			//	InitAudioに失敗
			CCILOADER,	//	ローダー作成に失敗
			MBTWC,		//	unicode変換に失敗
			SSD,		//	ディレクトリ設定に失敗
			PSE,		//	セグメントデータを再生できない
			INVALID,	//	ファイルが読み込めない
			DL,			//	ダウンロードできない
			UNLD,		//	アンロードできない
			INVOPEN,	//	ファイルが開けない
			RIFF,		//	RIFFチャンクが読み込めない
			FMT,		//	fmtチャンクが読み込めない
			NOTPCM,		//	読み込み対象のWAVEはサポートされていない形式
			DATA,		//	dataチャンクは存在しない
			READERROR,	//	読み込み失敗
			NOTHING		//	エラーナシ
		};
	}

	//	エフェクトフラグ番号
	namespace Effect {
		enum Type
		{
			NOTHING = 0,	//	エフェクトはなし
			FADEIN,			//	フェードイン
			FADEOUT,		//	フェードアウト
			STOP,			//	停止中
		};
	}


	//	ストリーミングサウンド情報
	struct MYSOUNDINFO
	{
		//	ストリーム情報
		char					szFileName[MAX_PATH];	//	再生対象のファイル名
		DWORD					dwReadPosition;			//	読みこんでいるファイルポジション
		DWORD					dwHeaderSize;			//	読みこんでいるWAVEヘッダのサイズ
		int						iStreamPos;				//	書き込みの対象を表す（0なら後半、1なら前半）
		bool					bStream;				//	ストリーミング再生を許可するかどうか
														//	ループ情報
		bool					bLoop;					//	ループさせるかどうか
		DWORD					dwReStart;				//	ループさせるとき、どこまで戻るか
		DWORD					dwLoop;					//	どこまできたらループさせるか
		DWORD					dwCnt;					//	何回ループを繰り返すか
														//	フェードインアウト処理
		Effect::Type			effectflag;		//	エフェクトの種類
		int						iFade;			//	フェード量
		long int				iVolume;		//	ボリューム
		IDirectSoundBuffer8*	sec_ptr;	//	セカンダリ（メモリ上の）バッファ
	};

	class Sound
	{
		Sound();
		bool  Initialize();
	public:
		enum class KIND { eStream, eSE };
		MYSOUNDINFO sinfo;
		using  WP = weak_ptr<Sound>;
		using SP = shared_ptr<Sound>;
		~Sound();
		//BGM等長い曲を再生する場合
		static  Sound::SP  CreateStream(const char*  szFileName);
		//効果音（ＳＥ）等短い曲を再生する場合
		static  Sound::SP  CreateSE(const char*  szFileName);
		HRESULT SetVolume(long);			//	ボリューム調整を行います
		HRESULT SetFrequency(DWORD);		//	周波数を変更します
		HRESULT SetPan(int);				//	パン設定を行います
		int GetVolume();					//	現在のボリューム値を取得します
		void FadeOut(int);				//	フェードアウトさせます
		void SetLoopInfo(int, int iLoopCnt = 0);	//	ループ情報を設定します
		HRESULT Play_Normal(bool bLoop);	//	バッファ内のデータを再生します
		HRESULT Play_FadeIn(DWORD, int);	//	バッファ内のデータをフェードインして再生します
		void Check_Stream(DWORD  dwBufferSize_);
		HRESULT Stop();					//	再生中のバッファを停止させます

	private:
		bool  CreateStream_Sub(const char*);
		bool  CreateSE_Sub(const char*);
		void LoadStream(int iWriteOffset, int iStreamPos);
		Err::Type WriteWaveStream(const char* szFileName,
			BYTE* data_ptr, int iOffset, DWORD dwSize, int iWriteOffset);
		void WriteSoundData(BYTE* data_ptr, int iOffset, int iSize);
		void WriteMute(int iSize);
		Err::Type LoadWaveStreamFirst(const char* szFileName,
			BYTE** data_ptr, DWORD dwSize, WAVEFORMATEX* wf_ptr);
		DWORD GetStatus();					//	バッファの状態を取得します
		DWORD GetCurrentPosition();
		Err::Type CreateSoundBuffer(WAVEFORMATEX* wf_ptr, DWORD dwBufbyte);

		///////////////////////////////////////////////////////////////////////////////////////
		Err::Type CreateSoundBuffer(int iNumber, WAVEFORMATEX* wf_ptr, DWORD dwBufbyte);
	};
	std::string GetErrorMessage(Err::Type number);// エラー番号から文字列を取得します

	class Obj
	{
	public:
		using WP = weak_ptr<Obj>;
		using SP = shared_ptr<Obj>;
	private:
		HWND								wnd;
		vector<Sound::WP>					streams;
		static  Obj::WP  winst;		//	１つしか存在できないインスタンスのウィークポインタ
		Obj();
		bool  Initialize(HWND wnd_, DWORD dwBufferSize);

	public:
		IDirectSound8*			ds_ptr;		//	DirectSoundオブジェクト
		IDirectSoundBuffer*		pri_ptr;	//	プライマリ（可聴）バッファ
											//	サウンド環境情報
		DSCAPS					caps;			//	デバイス能力を格納
		DWORD					dwBufferSize;	//	用意するバッファのサイズ
		~Obj();
		//	DIライブラリを生成する
		static  Obj::SP  Create(HWND  wnd_, DWORD dwBufferSize = kSTANDERDSIZE);
		static  Obj::SP  GetInst();
		HWND  Wnd() { return  wnd; }
		IDirectSound8&  DSound() { return  *ds_ptr; }
		//仮
		//static  Err::Type InitDirectSound(HWND, DWORD dwBufferSize = kSTANDERDSIZE);	//	サウンド環境を初期化します
		void GetDeviceCaps(DSCAPS*);			//	デバイスの情報を取得します

		//	ストリーミング再生を監視します(毎フレーム必ず呼ぶ)
		void UpDate(void);
		//
		void Set(Sound::SP  s_);
	};
	//---------------------------------------------------------------------------
	//サウンドマネージャー
	class  SoundManager {
	private:
		static  map<string, DM::Sound::SP>	arr;
		static  bool notObject;
		SoundManager() { notObject = false; }
	public:
		//登録
		static  void  Set(const string name_, DM::Sound::SP sp_);
		//名前で検索
		static  DM::Sound::SP  Get(const string name_);
		//単体破棄
		static  void  Clear(const string name_);
		//すべて破棄
		static  void  AllClear();
	};
}