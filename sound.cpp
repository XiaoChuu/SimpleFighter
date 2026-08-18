//----------------------------------------------
//音楽再生ライブラリ
// DirectSound DirectShow 汎用関数
//      k.koyanagi
//----------------------------------------------
// 2022.07.21 バグ修正版
//----------------------------------------------
#pragma	comment(lib,"winmm")
#pragma	comment(lib,"dinput8.lib")
#pragma comment(lib,"strmiids")
#pragma	comment(lib,"dsound.lib")

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <string>

#include "sound.h"
#include "unordered_map"

//-------------------------------------------------------
//BGM再生関連
struct DxShow{
	IGraphBuilder	*pBuilder;
	IMediaControl	*pMediaControl;
	IMediaEvent		*pMediaEvent;
	IMediaSeeking	*pMediaSeeking;
	IBasicAudio		*pBasicAudio;

	int				use;
};
//BGM管理構造体
struct BgmObj
{
	DxShow				bgm;
	std::string		filename;
};
std::unordered_map< std::string, BgmObj>	BgmTable;

//-------------------------------------------------------
//SE再生関連
// DirectSound8
LPDIRECTSOUND8          lpDS;
// プライマリサウンドバッファ
LPDIRECTSOUNDBUFFER		lpPrimary;

//同時発音できる最大数	
enum					{ DSNONE, DSSTART, DSPLAY, DSSTOP, DSEND };
struct PlayBuffer{
	LPDIRECTSOUNDBUFFER     pBuffer;		// プライマリサウンドバッファ
	int						ID;				//ベースになったバッファのID
	WORD					BufferSize;		//ベースになったバッファのサイズ
	int						State;			//バッファの状態
	bool					Loop;			//ループフラグ
};

const int				DSPLAYMAX = 10;
struct SoundBuffer{
	LPDIRECTSOUNDBUFFER     sBuffer;	//セカンダリバッファ
	WORD										BufferSize;		//バッファのサイズ
	DWORD										Fre;
	struct PlayBuffer	PlayBuffer[DSPLAYMAX];
};
struct SeObj
{
	SoundBuffer		se;
	std::string		filename;
};

std::unordered_map< std::string, SeObj>		SeTable;



//--------------------------------------------------------------------------------------------
// DirectShowの初期化
//--------------------------------------------------------------------------------------------
void ReleaseObj(DxShow& bgm){
	bgm.pMediaControl->Release();
	bgm.pMediaEvent->Release();
	bgm.pMediaSeeking->Release();
	bgm.pBuilder->Release();
	bgm.pBasicAudio->Release();
}

void	bgm::LoadFile(const std::string& resname, const std::string& filename)
{
	//既に読み込まれているリソース名なら、エラー
	if (BgmTable.count(filename) != 0){
		assert(!"同じ名前のリソースが登録済みです。名前の確認をしてください。");
	}
	//for (const auto& bgm: BgmTable){
	//	if (bgm.second.filename == filename){
	//		return;
	//	}
	//}
	BgmObj bgmObj;
	GUID format = TIME_FORMAT_FRAME;
	wchar_t name[256];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)filename.c_str(), -1, name, 256);

	HRESULT	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
																IID_IGraphBuilder, (void **)&bgmObj.bgm.pBuilder);
	if (FAILED(hr)){
		assert(!"DShow:GraphBuilderオブジェクトの生成に失敗しました");
		goto error;
	}

	if (FAILED(bgmObj.bgm.pBuilder->RenderFile(name, NULL))){
		assert(!"DShow\nファイル読み込みに失敗しました\nファイル名、コーデックの確認をお願いします。");
		goto error;
	}
	if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IMediaControl,
		(void**)&bgmObj.bgm.pMediaControl))){
		assert(!"DShow\nMediaControlの設定に失敗しました");
		goto error;
	}
	if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IMediaEvent,
		(void**)&bgmObj.bgm.pMediaEvent))){
		assert(!"DShow\nMediaEventの設定に失敗しました");
		goto error;
	}
	if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IMediaSeeking,
		(void**)&bgmObj.bgm.pMediaSeeking))){
		assert(!"DShow\nMediaSeekingの設定に失敗しました");
		goto error;
	}
	if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IBasicAudio,
		(void**)&bgmObj.bgm.pBasicAudio))){
		assert(!"DShow\nBasicAudioの設定に失敗しました");
		goto error;
	}

	bgmObj.bgm.pMediaSeeking->SetTimeFormat(&format);
	bgmObj.bgm.use = 1;
	bgmObj.filename = filename;
	BgmTable.insert(std::unordered_map<std::string, BgmObj>::value_type(resname, bgmObj));
	return;

error:
	ReleaseObj(bgmObj.bgm);

}
//--------------------------------------------------------------------------------------------
// DirectShowの後始末
//--------------------------------------------------------------------------------------------
void	bgm::Del()
{
	bgm::AllStop();
	for (auto& bgm : BgmTable){
		if (bgm.second.bgm.use == 1){
			ReleaseObj(bgm.second.bgm);
			bgm.second.bgm.use = 0;
		}
	}
	CoUninitialize();
}

//--------------------------------------------------------------------------------------------
// GraphBuilderの生成
//--------------------------------------------------------------------------------------------
void bgm::Init()
{
	CoInitialize(NULL);
}

//--------------------------------------------------------------------------------------------
// ファイルの再生
//--------------------------------------------------------------------------------------------
bool	bgm::Play(const std::string& resname)
{
	if (BgmTable.at(resname).bgm.use == 0) return false;
	HRESULT	hr = BgmTable.at(resname).bgm.pMediaControl->Run();
	if (FAILED(hr)){
		assert(!"DShow:再生に失敗しました1");
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------
// 再生速度の再生
//--------------------------------------------------------------------------------------------
void bgm::RateControl(const std::string& resname, float rate)
{
	if (BgmTable.at(resname).bgm.use == 0) return;
	if (rate < 0)	return;
	BgmTable.at(resname).bgm.pMediaSeeking->SetRate(rate);
}
//--------------------------------------------------------------------------------------------
// 再生音量の設定
// 0から100で設定　0は無音 100は最大 
//--------------------------------------------------------------------------------------------
void bgm::VolumeControl(const std::string& resname, int volume)
{
	if (BgmTable.at(resname).bgm.use == 0) return;
	if (volume < 0 || volume > 100)	return;
	//0から-10000で設定 0が最大　-10000は無音
	//100で割った値がデシベル
	long vol = (long)(-10000 + (volume * 100));
	BgmTable.at(resname).bgm.pBasicAudio->put_Volume(vol);
}
//--------------------------------------------------------------------------------------------
// 停止
//--------------------------------------------------------------------------------------------
void bgm::Stop(const std::string& resname)
{
	if (BgmTable.at(resname).bgm.use == 0) return;
	LONGLONG	start = 0;
	BgmTable.at(resname).bgm.pMediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning,
																											 NULL, AM_SEEKING_NoPositioning);
	BgmTable.at(resname).bgm.pMediaControl->Stop();
}

//--------------------------------------------------------------------------------------------
// 一時停止
//--------------------------------------------------------------------------------------------
void bgm::Pause(const std::string& resname)
{
	if (BgmTable.at(resname).bgm.use == 0) return;
	if (BgmTable.at(resname).bgm.use == 1){
		BgmTable.at(resname).bgm.pMediaControl->Stop();
	}
}

//--------------------------------------------------------------------------------------------
// 全停止
//--------------------------------------------------------------------------------------------
void bgm::AllStop()
{
	for (auto& bgm : BgmTable){
		if (bgm.second.bgm.use == 1){
			bgm.second.bgm.pMediaControl->Stop();
		}
	}
}

//--------------------------------------------------------------------------------------------
// 終了確認
//--------------------------------------------------------------------------------------------
void bgm::EndCheck()
{
	for (auto& bgm : BgmTable){
		if (bgm.second.bgm.use == 1){
			//			long code;
			//再生位置取得
			LONGLONG endPos = bgm::GetEndPosition(bgm.first);
			LONGLONG nowPos = bgm::GetCurrentPos(bgm.first);
			//ブロッキングするようなのでやめ
			//bgm.second.bgm.pMediaEvent->WaitForCompletion(0, &code);
			//再生終了時はループを行う。
			//			if(code == EC_COMPLETE){
			if (endPos <= nowPos){
				LONGLONG	start = 0;
				bgm.second.bgm.pMediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning,
																									 NULL, AM_SEEKING_NoPositioning);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
// 再生終了位置取得
//--------------------------------------------------------------------------------------------
LONGLONG bgm::GetEndPosition(const std::string& resname)
{
	if (BgmTable.at(resname).bgm.use == 0) return -1L;
	LONGLONG	end;
	//終了位置の取得
	BgmTable.at(resname).bgm.pMediaSeeking->GetStopPosition(&end);
	return end;
}

//--------------------------------------------------------------------------------------------
// 再生位置取得
//--------------------------------------------------------------------------------------------
LONGLONG bgm::GetCurrentPos(const std::string& resname)
{
	if (BgmTable.at(resname).bgm.use == 0) return -1L;
	LONGLONG	Current;
	BgmTable.at(resname).bgm.pMediaSeeking->GetCurrentPosition(&Current);
	return Current;
}

//--------------------------------------------------------------------------------------------
// 再生位置初期化
//--------------------------------------------------------------------------------------------
void bgm::SetStartPos(const std::string& resname)
{
	if (BgmTable.at(resname).bgm.use == 0) return;
	LONGLONG	start = 0;
	BgmTable.at(resname).bgm.pMediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning,
																											 NULL, AM_SEEKING_NoPositioning);
}


//********************************************************************//
//
//				BGM再生　関連関数 ここまで
//
//********************************************************************//

//********************************************************************//
//
//				SE再生　関連関数
//
//********************************************************************//
//--------------------------------------------------------------------------------------------
// DirectSoundの初期化
//--------------------------------------------------------------------------------------------
void se::Init(HWND hwnd_)
{
	if (se::Create(hwnd_)){
		if (!CreatePrimaryBuffer()){
			assert(!"プライマリバッファの生成に失敗しました");
		}
		CreateSecondaryBuffer();
	}
	else{
		assert(!"DSoundの生成に失敗しました");
	}

}

//--------------------------------------------------------------------------------------------
// DirectSoundの作成
//--------------------------------------------------------------------------------------------
bool se::Create(HWND hwnd_)
{
	HRESULT ret;

	// DirectSound8を作成
	ret = DirectSoundCreate8(NULL, &lpDS, NULL);
	if (FAILED(ret)) {
		assert(!"サウンドオブジェクト作成失敗\n");
		return false;
	}

	// 強調モード
	ret = lpDS->SetCooperativeLevel(hwnd_, DSSCL_EXCLUSIVE | DSSCL_PRIORITY);
	if (FAILED(ret)) {
		assert(!"強調レベル設定失敗");
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------
// バッファの作成
//--------------------------------------------------------------------------------------------
void se::CreateSecondaryBuffer()
{
	SeTable.clear();
}

//--------------------------------------------------------------------------------------------
// サウンド再生
//--------------------------------------------------------------------------------------------
void se::Play(const std::string& resname)
{
	int i;
	auto& se = SeTable.at(resname).se;
	for (i = 0; i < DSPLAYMAX; i++){
		if (se.PlayBuffer[i].State == DSNONE){
			//再生開始
			//			se.PlayBuffer[i].ID = SoundNo;
			se.PlayBuffer[i].State = DSPLAY;
			se.PlayBuffer[i].pBuffer->Play(0, 0, 0);
            se.PlayBuffer[i].Loop = 0;
            se.PlayBuffer[i].pBuffer->SetCurrentPosition(0);
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------
// サウンドループ再生
//--------------------------------------------------------------------------------------------
void se::PlayLoop(const std::string& resname)
{
	int i;
	auto& se = SeTable.at(resname).se;
	for (i = 0; i < DSPLAYMAX; i++){
		if (se.PlayBuffer[i].State == DSNONE){
			//再生開始
			//			se.PlayBuffer[i].ID = SoundNo;
			se.PlayBuffer[i].State = DSPLAY;
			se.PlayBuffer[i].pBuffer->Play(0, 0, 0);
			se.PlayBuffer[i].Loop = 1;
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------
// サウンド停止
//	同じ効果音をすべて停止する
//--------------------------------------------------------------------------------------------
void se::Stop(const std::string& resname)
{
	auto& se = SeTable.at(resname).se;
	for (int j = 0; j < DSPLAYMAX; j++){
		if (se.PlayBuffer[j].State == DSPLAY){
			se.PlayBuffer[j].pBuffer->Stop();
			se.PlayBuffer[j].State = DSNONE;
		}
	}
}

//--------------------------------------------------------------------------------------------
// すべてのサウンド停止
//--------------------------------------------------------------------------------------------
void se::AllStop()
{
	for (auto& seObj : SeTable){
		for (int j = 0; j < DSPLAYMAX; j++){
			if (seObj.second.se.PlayBuffer[j].State == DSPLAY){
				seObj.second.se.PlayBuffer[j].pBuffer->Stop();
				seObj.second.se.PlayBuffer[j].State = DSNONE;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
// 終了チェック
//--------------------------------------------------------------------------------------------
void se::EndCheck()
{
	DWORD Status;
	int a = 0;
	for (auto& seObj : SeTable){
		for (int j = 0; j < DSPLAYMAX; j++){
			if (seObj.second.se.PlayBuffer[j].State == DSPLAY){		//現在再生中の複製バッファ
				//終了と認識する
				seObj.second.se.PlayBuffer[j].pBuffer->GetStatus(&Status);
				if ((Status & DSBSTATUS_PLAYING) != 1){
					if (seObj.second.se.PlayBuffer[j].Loop){
						seObj.second.se.PlayBuffer[j].pBuffer->SetCurrentPosition(0);
						seObj.second.se.PlayBuffer[j].pBuffer->Play(0, 0, 0);
					}
					else{
						seObj.second.se.PlayBuffer[j].State = DSNONE;
						seObj.second.se.PlayBuffer[j].BufferSize = 0;
						seObj.second.se.PlayBuffer[j].pBuffer->Stop();
					}
				}
			}
		}
	}
}
//--------------------------------------------------------------------------------------------
// プライマリサウンドバッファの作成
//--------------------------------------------------------------------------------------------
bool se::CreatePrimaryBuffer(void)
{
	HRESULT ret;
	WAVEFORMATEX wf;

	// プライマリサウンドバッファの作成
	DSBUFFERDESC dsdesc;
	ZeroMemory(&dsdesc, sizeof(DSBUFFERDESC));
	dsdesc.dwSize = sizeof(DSBUFFERDESC);
	dsdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsdesc.dwBufferBytes = 0;
	dsdesc.lpwfxFormat = NULL;
	ret = lpDS->CreateSoundBuffer(&dsdesc, &lpPrimary, NULL);
	if (FAILED(ret)) {
		assert(!"プライマリサウンドバッファ作成失敗\n");
		return false;
	}

	// プライマリバッファのステータスを決定
	wf.cbSize = sizeof(WAVEFORMATEX);
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = 2;
	wf.nSamplesPerSec = 44100;
	wf.wBitsPerSample = 16;
	wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	ret = lpPrimary->SetFormat(&wf);
	if (FAILED(ret)) {
		assert(!"プライマリバッファのステータス失敗\n");
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------
//DSoundの後始末
//--------------------------------------------------------------------------------------------
bool se::Del(void)
{
	for (auto& seObj : SeTable){
		if (seObj.second.se.BufferSize != 0){
			seObj.second.se.sBuffer->Release();
		}
	}
	if (lpPrimary) {
		lpPrimary->Release();
		lpPrimary = NULL;
	}
	if (lpDS) {
		lpDS->Release();
		lpDS = NULL;
	}

	return true;
}

//--------------------------------------------------------------------------------------------
// サウンドバッファの作成
//--------------------------------------------------------------------------------------------
void se::LoadFile(const std::string& resname, const std::string& filename)
{
	HRESULT ret;
	MMCKINFO mSrcWaveFile;
	MMCKINFO mSrcWaveFmt;
	MMCKINFO mSrcWaveData;
	LPWAVEFORMATEX wf;

	//既に読み込まれているリソース名なら、エラー
	if (SeTable.count(filename) != 0){
		assert(!"DSound:同じ名前のリソースが登録済みです。名前の確認をしてください。");
	}
	for (const auto& se : SeTable){
		if (se.second.filename == filename){
			assert(!"DSound:同じファイル名のリソースが登録済みです。名前の確認をしてください。");
		}
	}

	// WAVファイルをロード
	HMMIO hSrc;
	hSrc = mmioOpen((LPSTR)filename.c_str(), NULL, MMIO_ALLOCBUF | MMIO_READ | MMIO_COMPAT);
	if (!hSrc) {
		assert(!"WAVファイルロードエラー\n");
		return;
	}

	// 'WAVE'チャンクチェック
	ZeroMemory(&mSrcWaveFile, sizeof(mSrcWaveFile));

	mSrcWaveFile.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	ret = mmioDescend(hSrc, &mSrcWaveFile, NULL, MMIO_FINDRIFF);
	if (ret != MMSYSERR_NOERROR) {
		assert(!"WAVEチャンクチェックエラー");
		mmioClose(hSrc, 0);
		return;
	}

	// 'fmt 'チャンクチェック
	ZeroMemory(&mSrcWaveFmt, sizeof(mSrcWaveFmt));
	mSrcWaveFmt.ckid = mmioFOURCC('f', 'm', 't', ' ');
	ret = mmioDescend(hSrc, &mSrcWaveFmt, &mSrcWaveFile, MMIO_FINDCHUNK);
	if (ret != MMSYSERR_NOERROR) {
		assert(!"fmt チャンクチェックエラー");
		mmioClose(hSrc, 0);
		return;
	}


	// ヘッダサイズの計算
	int iSrcHeaderSize = mSrcWaveFmt.cksize;
	if (iSrcHeaderSize<sizeof(WAVEFORMATEX))
		iSrcHeaderSize = sizeof(WAVEFORMATEX);

	// ヘッダメモリ確保
	wf = (LPWAVEFORMATEX)malloc(iSrcHeaderSize);
	if (!wf) {
		assert(!"メモリ確保エラー");
		mmioClose(hSrc, 0);
		return;
	}
	ZeroMemory(wf, iSrcHeaderSize);

	// WAVEフォーマットのロード
	ret = mmioRead(hSrc, (char*)wf, mSrcWaveFmt.cksize);
	if (FAILED(ret)) {
		assert(!"WAVEフォーマットロードエラー");
		free(wf);
		mmioClose(hSrc, 0);
		return;
	}

	// fmtチャンクに戻る
	mmioAscend(hSrc, &mSrcWaveFmt, 0);

	// dataチャンクを探す
	while (1) {
		// 検索
		ret = mmioDescend(hSrc, &mSrcWaveData, &mSrcWaveFile, 0);
		if (FAILED(ret)) {
			assert(!"dataチャンクが見つからない");
			free(wf);
			mmioClose(hSrc, 0);
			return;
		}
		if (mSrcWaveData.ckid == mmioStringToFOURCC("data", 0)){
			break;
		}
		// 次のチャンクへ
		ret = mmioAscend(hSrc, &mSrcWaveData, 0);
	}

	SeObj seObj;
	// サウンドバッファの作成
	DSBUFFERDESC dsdesc;
	ZeroMemory(&dsdesc, sizeof(DSBUFFERDESC));
	dsdesc.dwSize = sizeof(DSBUFFERDESC);
	dsdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STATIC | DSBCAPS_LOCDEFER | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
	dsdesc.dwBufferBytes = mSrcWaveData.cksize;
	dsdesc.lpwfxFormat = wf;
	dsdesc.guid3DAlgorithm = DS3DALG_DEFAULT;
	ret = lpDS->CreateSoundBuffer(&dsdesc, &seObj.se.sBuffer, NULL);
	seObj.se.BufferSize = (WORD)dsdesc.dwBufferBytes;
	if (FAILED(ret)) {
		assert(!"サウンドバッファの作成エラー");
		free(wf);
		mmioClose(hSrc, 0);
		return;
	}

	// ロック開始
	LPVOID pMem1, pMem2;
	DWORD dwSize1, dwSize2;
	ret = (seObj.se.sBuffer)->Lock(0, mSrcWaveData.cksize, &pMem1, &dwSize1, &pMem2, &dwSize2, 0);
	seObj.se.sBuffer->GetFrequency(&seObj.se.Fre);
	if (FAILED(ret)) {
		assert(!"ロック失敗");
		free(wf);
		mmioClose(hSrc, 0);
		return;
	}

	// データ書き込み
	mmioRead(hSrc, (char*)pMem1, dwSize1);
	mmioRead(hSrc, (char*)pMem2, dwSize2);

	// ロック解除
	(seObj.se.sBuffer)->Unlock(pMem1, dwSize1, pMem2, dwSize2);
	// ヘッダ用メモリを開放
	free(wf);
	// WAVを閉じる
	mmioClose(hSrc, 0);

	seObj.se.sBuffer->SetVolume(-50);

	//複製開始
	for (int t = 0; t < 10; t++){
		if (lpDS->DuplicateSoundBuffer(seObj.se.sBuffer,
			&(seObj.se.PlayBuffer[t].pBuffer)) != DS_OK){
			assert(!"再生用バッファ作成失敗");
			break;
		}
		else{
			seObj.se.PlayBuffer[t].State = DSNONE;
			seObj.se.PlayBuffer[t].BufferSize = seObj.se.BufferSize;
			seObj.se.PlayBuffer[t].Loop = 0;
		}
	}
	SeTable.insert(std::unordered_map<std::string, SeObj>::value_type(resname, seObj));
}


//--------------------------------------------------------------------------------------------
// 周波数変更 再生速度が変わる
//--------------------------------------------------------------------------------------------
void se::SetFrequency(const std::string& resname, int Fre)
{
	auto se = SeTable.at(resname).se;
	se.sBuffer->SetFrequency(se.Fre + Fre);
}

void se::SetVolume(const std::string& resname, int Vol)
{
	long volume = DSBVOLUME_MIN;
	if (Vol < 0) {
		Vol = 0;
	}
	if (Vol >= 100) {
		Vol = 100;
	}
	volume = (long)(1000.0 * log10((Vol) / 100.0));
	if (volume < DSBVOLUME_MIN) {
		volume = DSBVOLUME_MIN;
	}
	else {
		if (volume > DSBVOLUME_MAX) {
			volume = DSBVOLUME_MAX;
		}
	}
	auto se = SeTable.at(resname).se;
	for (auto& s : se.PlayBuffer)
	{
		s.pBuffer->SetVolume(volume);
	}
}
//********************************************************************//
//
//				SE再生　関連関数 ここまで
//
//********************************************************************//


