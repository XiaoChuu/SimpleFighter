#include "dm2008_Ver1_4.h"

#define kREL(x) if(x){x->Release();x=nullptr;}
#define verCloseHandle(h) {\
	if((h != INVALID_HANDLE_VALUE) || (h != nullptr))\
	{\
		CloseHandle(h);\
		h = nullptr;\
	}\
}
#define verDelete_Scalar(p) {\
	if(p)\
	{\
		delete p;\
		p = nullptr;\
	}\
}
#define verDelete_Vector(p) {\
	if(p)\
	{\
		delete[] p;\
		p = nullptr;\
	}\
}
namespace  DM
{

	Obj::WP Obj::winst;	//	１つしか存在できないインスタンス
	Obj::Obj()
	{
		this->wnd = 0;
		this->ds_ptr = nullptr;
		this->dwBufferSize = 0;
		this->pri_ptr = 0;
		ZeroMemory(&this->caps, sizeof(DSCAPS));
	}
	bool Obj::Initialize(HWND wnd_, DWORD dwBufferSize)
	{
		wnd = wnd_;


		//	DirectSoundの初期化
		if (FAILED(CoCreateInstance(CLSID_DirectSound8, NULL, CLSCTX_INPROC,
			IID_IDirectSound8, (void **)&this->ds_ptr)))
		{
			return false;	// return Err::CCIS;
		}
		if (FAILED(this->ds_ptr->Initialize(NULL)))
		{
			return false;	// return Err::INIT;
		}
		if (FAILED(this->ds_ptr->SetCooperativeLevel(wnd, DSSCL_PRIORITY)))
		{
			return false;	// return Err::SCL;
		}
		//	プライマリサウンドバッファを作成
		DSBUFFERDESC desc =
		{
			sizeof(DSBUFFERDESC)
		};

		desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
		if (FAILED(this->ds_ptr->CreateSoundBuffer(&desc, &this->pri_ptr, NULL)))
		{
			return false;	// return Err::CSB;
		}
		//	情報の初期化
		Obj::ds_ptr->GetCaps(&this->caps);
		Obj::dwBufferSize = dwBufferSize;
		//return Err::NOTHING;
		return true;
	}

	Obj::~Obj()
	{
		//	DirectSound
		kREL(this->pri_ptr);
		kREL(this->ds_ptr);
	}
	//	DMライブラリを生成する
	Obj::SP Obj::Create(HWND		hw_, DWORD dwBufferSize)	//	ウィンドウハンドル
	{
		if (winst.expired()){
			Obj::SP sp = Obj::SP(new Obj());
			if (sp->Initialize(hw_, dwBufferSize)){
				winst = sp;
				return sp;
			}
		}
		return 0;
	}
	Obj::SP Obj::GetInst()
	{
		if (!winst.expired()){
			return winst.lock();
		}
		return 0;
	}



	//--------------------------------------------------------------------------------------------
	void Obj::Set(shared_ptr<class  Sound>  s_)
	{
		this->streams.push_back(s_);
	}
	void Obj::UpDate(void)
	{
		class cNullCheck{
		public:
			// タスクが既に解放されているならtrueを返す
			bool operator()(Sound::WP t_) const {
				bool rtv = t_.expired();
				return  rtv;
			}
		};
		auto endIt = remove_if(this->streams.begin(), this->streams.end(), cNullCheck());
		this->streams.erase(endIt, this->streams.end());	//既に消失しているものを全てを削除する

		//	読み込みタイミングを調べる
		for (auto it = this->streams.begin();
			it != this->streams.end();
			it++)
		{
			auto sp = it->lock();
			sp->Check_Stream(this->dwBufferSize);
		}

	}
	//--------------------------------------------------------------------------------------------
	void Obj::GetDeviceCaps(DSCAPS* caps_ptr)
	{
		memcpy(caps_ptr, &this->caps, sizeof(DSCAPS));
	}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------
	Err::Type Sound::CreateSoundBuffer(WAVEFORMATEX* wf_ptr, DWORD dwBufbyte)
	{
		DSBUFFERDESC desc;
		LPDIRECTSOUNDBUFFER lpTempBuff = NULL;
		Err::Type hRet = Err::NOTHING;		//	関数戻り値

		//	サウンドデータ
		if (!wf_ptr)
		{
			return Err::SFMT;
		}
		//	サウンドバッファの設定
		ZeroMemory(&desc, sizeof(desc));
		desc.lpwfxFormat = wf_ptr;
		desc.dwSize = sizeof(DSBUFFERDESC);
		desc.dwFlags = DSBCAPS_STATIC | DSBCAPS_LOCDEFER |
			DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN;
		desc.dwBufferBytes = dwBufbyte;	//	バッファサイズ
		//	DirectSoundBuffer生成
		auto  dm = Obj::GetInst();
		if (FAILED(dm->ds_ptr->CreateSoundBuffer(&desc, &lpTempBuff, NULL)))
		{
			hRet = Err::CSB;
			goto end;
		}
		//	DirectSoundBuffer8取得
		if (FAILED(lpTempBuff->QueryInterface(IID_IDirectSoundBuffer8, (void**)&this->sinfo.sec_ptr)))
		{
			hRet = Err::QI;
			goto end;
		}
	end:
		kREL(lpTempBuff);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	Sound::SP  Sound::CreateSE(const char* szFileName)
	{
		auto sp = SP(new Sound());
		if (sp){
			bool  rtv;
			rtv = sp->CreateSE_Sub(szFileName);
			if (rtv){
				auto  ob = Obj::GetInst();
				return sp;
			}
		}
		return 0;
	}
	//--------------------------------------------------------------------------------------------
	Sound::SP  Sound::CreateStream(const char* szFileName)
	{
		auto sp = SP(new Sound());
		if (sp){
			bool  rtv;
			rtv = sp->CreateStream_Sub(szFileName);
			if (rtv){
				auto  ob = Obj::GetInst();
				ob->Set(sp);
				return sp;
			}
		}
		return 0;
	}
	//--------------------------------------------------------------------------------------------
	bool Sound::CreateSE_Sub(const char* szFileName)
	{
		WAVEFORMATEX wf;
		BYTE* data_ptr = NULL;
		bool hRet = true;		//	関数戻り値

		kREL(this->sinfo.sec_ptr);
		ZeroMemory(&wf, sizeof(WAVEFORMATEX));
		//	DirectSoundBuffer設定
		if (Err::NOTHING != this->LoadWaveStreamFirst(szFileName, &data_ptr, 0, &wf))
		{
			hRet = false;// = Err::WRG;
			goto end;
		}
		//	バッファの生成
		if (Err::NOTHING != this->CreateSoundBuffer(&wf, this->sinfo.dwReadPosition - this->sinfo.dwHeaderSize))
		{
			goto end;
		}
		//	バッファロック
		this->WriteSoundData(data_ptr, 0, this->sinfo.dwReadPosition - this->sinfo.dwHeaderSize);
		//	サウンド情報を格納
		strcpy_s(this->sinfo.szFileName, MAX_PATH, szFileName);	//	サウンドファイル名を格納
		this->sinfo.iStreamPos = 0;
		this->sinfo.bLoop = false;
		this->sinfo.bStream = false;
	end:
		verDelete_Vector(data_ptr);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	bool Sound::CreateStream_Sub(const char* szFileName)
	{
		BYTE* data_ptr = NULL;
		WAVEFORMATEX wf;
		bool  hRet = true;// Err::NOTHING;		//	関数戻り値

		auto  ds = Obj::GetInst();


		kREL(this->sinfo.sec_ptr);
		//	DirectSoundBuffer設定
		if (Err::NOTHING != this->LoadWaveStreamFirst(szFileName, &data_ptr, ds->dwBufferSize, &wf))
		{
			hRet = false;// Err::WRG;
			goto end;
		}
		//	バッファの生成
		if (Err::NOTHING != this->CreateSoundBuffer(&wf, ds->dwBufferSize * 2))
		{
			goto end;
		}
		//	バッファロック
		this->WriteSoundData(data_ptr, 0, ds->dwBufferSize);
		//	サウンド情報を格納
		strcpy_s(this->sinfo.szFileName, MAX_PATH, szFileName);	//	サウンドファイル名を格納
		this->sinfo.iStreamPos = 0;
		this->sinfo.bStream = true;
	end:
		verDelete_Vector(data_ptr);
		return hRet;
	}

	//--------------------------------------------------------------------------------------------
	Sound::Sound()
	{
		this->sinfo.sec_ptr = nullptr;
		this->sinfo.bLoop = false;
		this->sinfo.bStream = false;
		this->sinfo.dwCnt = 0;
		this->sinfo.dwHeaderSize = 0;
		this->sinfo.dwLoop = 0;
		this->sinfo.dwReadPosition = 0;
		this->sinfo.dwReStart = 0;
		this->sinfo.iVolume = DSBVOLUME_MAX;
	}
	//--------------------------------------------------------------------------------------------
	Sound::~Sound()
	{
		kREL(this->sinfo.sec_ptr);
	}
	//--------------------------------------------------------------------------------------------
	DWORD Sound::GetCurrentPosition()
	{
		return this->sinfo.dwReadPosition;
	}
	//--------------------------------------------------------------------------------------------
	DWORD Sound::GetStatus( )
	{
		DWORD dwStatus;

		this->sinfo.sec_ptr->GetStatus(&dwStatus);
		return dwStatus;
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::Play_FadeIn(DWORD dwPlayFlag, int iValue)
	{
		HRESULT hRet;

		//	初期化されていない場合は再生しない
		if (this->sinfo.sec_ptr == NULL)
		{
			return DSERR_INVALIDCALL;
		}
		hRet = this->sinfo.sec_ptr->Play(0, 0, dwPlayFlag);
		this->SetVolume(kVOLUME_MIN);
		this->sinfo.iVolume = kVOLUME_MIN;
		this->sinfo.effectflag = Effect::FADEIN;
		this->sinfo.iFade = iValue;
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::Play_Normal(bool bLoop)
	{
		HRESULT hRet;
		DWORD dwPlayFlag = 0;

		//	初期化されていない場合は再生しない
		if (this->sinfo.sec_ptr == NULL)
		{
			return DSERR_INVALIDCALL;
		}
		//	再生
		this->sinfo.sec_ptr->SetCurrentPosition(0);
		if (bLoop || this->sinfo.bStream)
		{
			dwPlayFlag = DSBPLAY_LOOPING;
		}
		hRet = this->sinfo.sec_ptr->Play(0, 0, dwPlayFlag);
		//	ボリュームやエフェクトの初期化
		this->SetVolume(this->sinfo.iVolume);
		this->sinfo.bLoop = bLoop;
		this->sinfo.effectflag = Effect::NOTHING;
		return hRet;
	}

	//--------------------------------------------------------------------------------------------
	Err::Type Sound::LoadWaveStreamFirst(const char* szFileName,
		BYTE** data_ptr, DWORD dwSize, WAVEFORMATEX* wf_ptr)
	{
		HANDLE hFile = NULL;		//	Waveファイルハンドル
		Err::Type hRet = Err::NOTHING;	//	戻り値を格納します
		DWORD dwResult;			//	ReadFileの結果を格納する
		DWORD dwRiffSize;		//	RIFFチャンクの大きさを取得
		DWORD dwFmtSize;		//	fmtチャンクの大きさを取得
		DWORD dwDataSize;		//	dataチャンクの大きさを取得
		char szChunk[4] = "";	//	チャンクの文字列を格納します（\0は保存しません）
		bool bLoop;

		//	ファイルを開く
		hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hRet = Err::INVOPEN;
			goto end;
		}
		//	RIFFチャンクの読み込み
		ReadFile(hFile, szChunk, 4, &dwResult, NULL);
		//	読み込み失敗、もしくはRIFFではなかった場合
		if ((dwResult != 4) || (memcmp("RIFF", szChunk, 4)))
		{
			hRet = Err::RIFF;
			goto end;
		}
		ReadFile(hFile, &dwRiffSize, sizeof(DWORD), &dwResult, NULL);
		ReadFile(hFile, szChunk, 4, &dwResult, NULL);
		//	読み込み失敗、もしくはWAVEではなかった場合
		if ((dwResult != 4) || (memcmp("WAVE", szChunk, 4)))
		{
			hRet = Err::RIFF;
			goto end;
		}
		//	Foramtチャンク
		bLoop = true;
		while (bLoop)
		{
			ReadFile(hFile, szChunk, 4, &dwResult, NULL);
			//	読み込み失敗、もしくは不正なデータ形式だった場合
			if (dwResult != 4)
			{
				hRet = Err::DATA;
				goto end;
			}
			//	目的のチャンクだった場合
			if (memcmp("fmt", szChunk, 3) == 0)
			{
				bLoop = false;
			}
			else
			{
				//	チャンク違い
				ReadFile(hFile, &dwDataSize, 4, &dwResult, NULL);
				SetFilePointer(hFile, dwDataSize, 0, FILE_CURRENT);
			}
		}
		//	fmtデータサイズ
		ReadFile(hFile, &dwFmtSize, sizeof(DWORD), &dwResult, NULL);
		//	フォーマット情報取得
		ReadFile(hFile, wf_ptr, dwFmtSize, &dwResult, NULL);
		if ((wf_ptr->wFormatTag != WAVE_FORMAT_PCM))
		{
			hRet = Err::NOTPCM;
			goto end;
		}
		wf_ptr->cbSize = 0;
		//	Dataチャンク検索
		bLoop = true;
		while (bLoop)
		{
			ReadFile(hFile, szChunk, 4, &dwResult, NULL);
			//	読み込み失敗、もしくは不正なデータ形式だった場合
			if (dwResult != 4)
			{
				hRet = Err::DATA;
				goto end;
			}
			//	Dataチャンクがみつかった
			if (memcmp("data", szChunk, 4) == 0)
			{
				bLoop = false;
			}
			else
			{
				//	チャンク違い
				ReadFile(hFile, &dwDataSize, 4, &dwResult, NULL);
				SetFilePointer(hFile, dwDataSize, 0, FILE_CURRENT);
			}
		}
		//	データサイズ読み込み
		ReadFile(hFile, &dwDataSize, 4, &dwResult, NULL);
		//	WAVヘッダサイズを保存しておく（予期しないチャンクデータ対策）
		this->sinfo.dwHeaderSize = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
		//	WAVEデータを読み込む
		if (dwSize == 0)
		{
			dwSize = dwDataSize;
		}
		(*data_ptr) = new BYTE[dwSize];
		//	SetFilePointer(hFile, Sound::sinfo[iNumber].dwHeaderSize+dwOffset, NULL, FILE_BEGIN);
		ReadFile(hFile, *data_ptr, dwSize, &dwResult, NULL);
		//	指定したサイズと読み込んだサイズが違うとき
		if (dwResult != dwSize)
		{
			hRet = Err::READERROR;
			goto end;
		}
		//	ファイルのカレントポジションを取得しておく
		this->sinfo.dwReadPosition = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	end:
		verCloseHandle(hFile);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	void Sound::SetLoopInfo(int iReStartPos, int iLoopCnt)
	{
		this->sinfo.dwReStart = iReStartPos;
		this->sinfo.dwCnt = iLoopCnt;
	}
	//--------------------------------------------------------------------------------------------
	void Sound::FadeOut(int iValue)
	{
		this->sinfo.effectflag = Effect::FADEOUT;
		this->sinfo.iFade = iValue;
	}
	//--------------------------------------------------------------------------------------------
	int Sound::GetVolume( )
	{
		long iVolume = 0;

		this->sinfo.sec_ptr->GetVolume(&iVolume);
		return iVolume;
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::SetPan(int iPan)
	{
		return this->sinfo.sec_ptr->SetPan(iPan);
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::SetFrequency(DWORD dwFreq)
	{
		return this->sinfo.sec_ptr->SetFrequency(dwFreq);
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::SetVolume(long int iVolume)
	{
		this->sinfo.iVolume = iVolume;
		return this->sinfo.sec_ptr->SetVolume(iVolume);
	}
	//--------------------------------------------------------------------------------------------
	void Sound::WriteMute(int iSize)
	{
		LPVOID lpSoundData1 = NULL, lpSoundData2 = NULL;
		DWORD size1, size2;

		this->sinfo.sec_ptr->Lock(0, iSize, &lpSoundData1, &size1,
			&lpSoundData2, &size2, DSBLOCK_ENTIREBUFFER);
		// サウンドデータ読み込み
		memset(lpSoundData1, 0, iSize);
		// ロック解除
		this->sinfo.sec_ptr->Unlock(lpSoundData1, size1, lpSoundData2, size2);
	}
	//--------------------------------------------------------------------------------------------
	HRESULT Sound::Stop( )
	{
		auto  ds = Obj::GetInst();
		if (!this->sinfo.sec_ptr)
		{
			return HRESULT(0);
		}
		//	Sound::sinfo[iNumber].bStream = false;
		if (this->sinfo.bStream) {
			WriteMute(ds->dwBufferSize);
			this->sinfo.dwReadPosition = this->sinfo.dwHeaderSize;			//	読みこんでいるファイルポジション
			this->sinfo.iStreamPos = 0;
		}
		return this->sinfo.sec_ptr->Stop();
	}
	//--------------------------------------------------------------------------------------------
	void Sound::WriteSoundData(BYTE* data_ptr, int iOffset, int iSize)
	{
		LPVOID lpSoundData1 = NULL, lpSoundData2 = NULL;
		DWORD size1, size2;

		this->sinfo.sec_ptr->Lock(iOffset, iSize, &lpSoundData1, &size1,
			&lpSoundData2, &size2, DSBLOCK_ENTIREBUFFER);
		// サウンドデータ読み込み
		memcpy(lpSoundData1, data_ptr, iSize);
		// ロック解除
		this->sinfo.sec_ptr->Unlock(lpSoundData1, size1, lpSoundData2, size2);
	}
	//--------------------------------------------------------------------------------------------
	Err::Type Sound::WriteWaveStream(const char* szFileName,
		BYTE* data_ptr, int iOffset, DWORD dwSize, int iWriteOffset)
	{
		HANDLE hFile = NULL;
		DWORD dwResult;
		DWORD dwRead;	//	実際にファイルを読み込んだバイト数
		Err::Type hRet = Err::NOTHING;

		//	ファイルを開く
		hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hRet = Err::INVOPEN;
			goto end;
		}
		//	データを読み込む
		SetFilePointer(hFile, iOffset, NULL, FILE_BEGIN);
		ReadFile(hFile, data_ptr, dwSize, &dwResult, NULL);
		//	指定したサイズと読み込んだサイズが違うとき
		if (dwResult != dwSize)
		{
			//	EOFだったと仮定し、処理を行う
			//	読み込み先のポイントをデータの先頭に設定する
			dwRead = SetFilePointer(hFile, 0, NULL, FILE_CURRENT) - this->sinfo.dwReadPosition;
			if (!this->sinfo.bLoop)
			{
				ZeroMemory(&data_ptr[dwRead], dwSize - dwRead);
				this->sinfo.effectflag = Effect::STOP;
				goto end;
			}
			//	読み込みポジションを設定する
			if (this->sinfo.dwReStart != 0)	//	リスタート地点が設定されているならば
			{
				this->sinfo.dwReadPosition = this->sinfo.dwReStart + this->sinfo.dwHeaderSize;
			}
			else
			{
				this->sinfo.dwReadPosition = this->sinfo.dwHeaderSize;
			}
			//	バッファにデータを書きこむ
			verCloseHandle(hFile);
			this->WriteSoundData(data_ptr, iWriteOffset, dwRead);
			this->WriteWaveStream(szFileName, &data_ptr[dwRead],
				this->sinfo.dwReadPosition, dwSize - dwRead, iWriteOffset + dwRead);
		}
		else
		{
			//	ファイルのカレントポジションを取得しておく
			this->sinfo.dwReadPosition = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
			this->WriteSoundData(data_ptr, iWriteOffset, dwSize);
		}
	end:
		verCloseHandle(hFile);
		return hRet;
	}
	//--------------------------------------------------------------------------------------------
	void Sound::LoadStream(int iWriteOffset, int iStreamPos)
	{
		BYTE* data_ptr = NULL;	//	サウンドデータ

		auto  dm = Obj::GetInst();
		data_ptr = new BYTE[dm->dwBufferSize];
		WriteWaveStream(this->sinfo.szFileName, data_ptr,
			this->sinfo.dwReadPosition, dm->dwBufferSize, iWriteOffset);
		this->sinfo.iStreamPos = iStreamPos;
		verDelete_Vector(data_ptr);
	}
	//--------------------------------------------------------------------------------------------
	void Sound::Check_Stream(DWORD  dwBufferSize_)
	{
		DWORD dwStatus;	//	再生フラグを格納
		DWORD dwPos;	//	再生ポジションを格納
		//	初期化が終わっていない場合
		if (!this->sinfo.sec_ptr)
		{
			return;
		}
		//	ストリーミング再生が許可されていない
		if (!this->sinfo.bStream)
		{
			return;
		}
		//	バッファは再生中かどうかを調べる
		this->sinfo.sec_ptr->GetStatus(&dwStatus);
		if (!(dwStatus & DSBSTATUS_PLAYING))
		{
			return;
		}
		//	読み込みタイミングかどうかをチェック
		this->sinfo.sec_ptr->GetCurrentPosition(&dwPos, NULL);
		//	再生ポジションフラグが0で、かつ0-11025なら22050-44100のデータを更新する。
		if ((this->sinfo.iStreamPos == 0) && (dwPos > 0) && (dwPos < (dwBufferSize_ / 2)))
		{
			this->LoadStream(dwBufferSize_, 1);
		}
		//	再生ポジションフラグが1で、かつ22050-33075なら0-22050のデータを更新。
		if ((this->sinfo.iStreamPos == 1) && (dwPos > dwBufferSize_))
		{
			this->LoadStream(0, 0);
		}
		//	エフェクトを管理
		if (this->sinfo.effectflag == Effect::FADEOUT)
		{
			this->sinfo.iVolume -= this->sinfo.iFade;//kVALUE_FADE;
			if (this->sinfo.iVolume < kVOLUME_MIN)
			{
				this->Stop();
				this->sinfo.effectflag = Effect::NOTHING;
			}
			this->SetVolume(this->sinfo.iVolume);
		}
		if (this->sinfo.effectflag == Effect::FADEIN)
		{
			this->sinfo.iVolume += this->sinfo.iFade;//kVALUE_FADE;
			if (this->sinfo.iVolume > DSBVOLUME_MAX)
			{
				this->sinfo.iVolume = DSBVOLUME_MAX;
				this->sinfo.effectflag = Effect::NOTHING;
			}
			this->SetVolume(this->sinfo.iVolume);
		}
		if (this->sinfo.effectflag == Effect::STOP)
		{
			this->Stop( );
			this->sinfo.effectflag = Effect::NOTHING;
		}
	}



	//--------------------------------------------------------------------------------------------
	//	関数名		：	GetErrorMessage
	//	作成者		：	植山沙欧
	//	作成日		：	2002.06.24
	//	機能		：	エラーメッセージを取得します
	//	機能効果	：	「number」に対応する文字列を「szMsg」にコピーします
	//					「iLength」にはコピー先のバッファサイズを指定してください
	//	引数		：	＜入力＞number		参照するエラーナンバー
	//					＜入力＞szMsg		メッセージの格納先
	//					＜入力＞iLength		格納先の長さ
	//	使用変数	：	char szWork		文字列の位置時格納先
	//	使用関数	：	strcpy_s	文字列コピー
	//					memcpy			メモリコピー
	//	戻り値		：	ありません
	//	更新履歴	：	2002.06.24	Ver1.00		植山沙欧	機能の実現
	//					2007.02.09	Ver1.01		植山沙欧	コピー方法の変更
	//--------------------------------------------------------------------------------------------

	std::string GetErrorMessage(Err::Type number)
	{
		std::string szMsg;

		switch (number)
		{
		case Err::CCIM: szMsg = "DirectMusicの作成に失敗"; break;
		case Err::CCIS: szMsg = "DirectSoundの作成に失敗"; break;
		case Err::INIT: szMsg = "Initializeに失敗"; break;
		case Err::SCL: szMsg = "SetCooperativeLevelに失敗"; break;
		case Err::CSB: szMsg = "バッファ作成に失敗"; break;
		case Err::QI: szMsg = "QueryInterfaceに失敗"; break;
		case Err::SFMT: szMsg = "フォーマット設定に失敗"; break;
		case Err::IA: szMsg = "InitAudioに失敗"; break;
		case Err::CCILOADER: szMsg = "ローダー作成に失敗"; break;
		case Err::MBTWC: szMsg = "unicode変換に失敗"; break;
		case Err::SSD: szMsg = "ディレクトリ設定に失敗"; break;
		case Err::PSE: szMsg = "セグメントデータを再生できない"; break;
		case Err::INVALID: szMsg = "ファイルが読み込めない"; break;
		case Err::DL: szMsg = "ダウンロードできない"; break;
		case Err::UNLD: szMsg = "アンロードできない"; break;
		case Err::INVOPEN: szMsg = "ファイルが開けない"; break;
		case Err::RIFF: szMsg = "RIFFチャンクが読み込めない"; break;
		case Err::FMT: szMsg = "fmtチャンクが読み込めない"; break;
		case Err::NOTPCM: szMsg = "読み込み対象のWAVEはサポートされていない形式"; break;
		case Err::DATA: szMsg = "dataチャンクは存在しない"; break;
		case Err::READERROR: szMsg = "読み込み失敗"; break;
		case Err::NOTHING: szMsg = "エラーはありません"; break;
		default: szMsg = "定義されていないエラー"; break;
		}
		return szMsg;
	}
	//-----------------------------------------------------------------------
	//Mouse系
	//-----------------------------------------------------------------------
	map<string, DM::Sound::SP>	SoundManager::arr;
	bool SoundManager::notObject;
	void  SoundManager::Set(const string name_, DM::Sound::SP sp_)
	{
		arr[name_] = sp_;
	}
	//-----------------------------------------------------------------------
	DM::Sound::SP  SoundManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("SoundManager 対象が存在しません"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  SoundManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void SoundManager::AllClear()
	{
		arr.clear();
	}
	//---------------------------------------------------------------------------
}

