#pragma once
#include <windows.h>
#include <functional>
#pragma	comment(lib,"winmm")	//	マルチメディア拡張機能を使用するために必要
#include "XI2018_Ver1_0.h"
#include "DG2014/DG2014_Ver3_6.h"
#include "DM2008_Ver1_4.h"
#include "myLib.h"
#include <vector>
#include <unordered_map>
#include <map>

//タスクリソース基本クラス
class BResource
{
protected:
	BResource() {}
public:
	~BResource() {}
	virtual bool  Initialize() = 0;
	virtual bool  Finalize() = 0;
	using SP = shared_ptr<BResource>;
	using WP = weak_ptr<BResource>;
};
//タスク基本クラス
class BTask
{
public:
	using SP = shared_ptr<BTask>;
	using WP = weak_ptr<BTask>;
	enum class State : DWORD{	Active,		//通常
								Stop,		//停止（ステップ実行停止）
								Kill,		//消滅
								Suspend,	//サスペンド（一時無効）
								StateNon = 0xFFFFFFFF};
private:
	static DWORD uKeyManager;	//ユニークキーマネージャ
	DWORD		uKey;			//タスク毎に割り当てられるユニークキー
	DWORD		groupKey;		//グループ名から自動で生成されるキー
	bool		detectible;		//GetTask系メソッドで検出できる
	int			taskStateCnt_Stop;
	int			taskStateCnt_Suspend;
	int			taskStateCnt_Kill;
protected:
public:
	string		groupName;		//タスクに割り当てるグループ名（検索に使用）
	string		name;			//タスクに割り当てる名前（検索に使用）
	bool		render2D_Active[2];	//描画の有無
	bool		render3D_Active[4];	//描画の有無
	float		render2D_Priority[2];	//２Ｄ用プライオリティ
	float		render3D_Priority[4];	//3D用プライオリティ
	bool		render3D_Alpha;		//3D用透過の有無
	bool        nextTaskCreate;		//finalize処理の中でタスクを生成するか否か

	DWORD  UKey( ){			return uKey; }
	DWORD  GroupKey( ){		return groupKey; }
	bool   Detectible( ){	return detectible;}
	int		TaskStateCnt_Stop( ){		return taskStateCnt_Stop;}
	int		TaskStateCnt_Suspend( ){	return taskStateCnt_Suspend;}
	int		TaskStateCnt_Kill( ){		return taskStateCnt_Kill;}

protected:
	BTask( )
	:groupName(),name(),render3D_Alpha(false),detectible( )
	{
		this->uKey = uKeyManager++;
		this->groupKey = 0;
	}
	bool Initialize(const  string&  gn_,  const  string&  n_,  bool   det_ )
	{
		this->groupName  =  gn_;
		this->name  =  n_;
		memcpy(&this->groupKey, gn_.c_str( ), min(4, gn_.length( )));
		this->detectible  =  det_;

		this->taskStateCnt_Kill = 0;
		this->taskStateCnt_Stop = 0;
		this->taskStateCnt_Suspend = 0;

		render2D_Priority[0]=0;	render2D_Active[0]=true;
		render2D_Priority[1]=0;	render2D_Active[1]=true;

		render3D_Priority[0]=0;	render3D_Active[0]=true;
		render3D_Priority[1]=0;	render3D_Active[1]=true;
		render3D_Priority[2]=0;	render3D_Active[2]=true;
		render3D_Priority[3]=0;	render3D_Active[3]=true;
		nextTaskCreate = true;
		return true;
	}
public:
	virtual ~BTask( ){ 
		//親と縁を切る
		this->SetParent(nullptr);
	}
	void T_Step( ){			
		if (this->taskStateCnt_Kill > 0 ||
			this->taskStateCnt_Suspend > 0) {
		}
		else if (this->taskStateCnt_Stop > 0) {
			UpDate_Stop();
		}
		else{
			UpDate();
		}
	}
	void T_UpDate3DPriority() {
		if (this->taskStateCnt_Kill > 0 ||
			this->taskStateCnt_Stop > 0 ||
			this->taskStateCnt_Suspend > 0) {
			return;
		}
		UpDate3DPriority();
	}
	void T_Draw2D(DWORD ln_ ){
		if(		this->taskStateCnt_Kill > 0  ||
				this->taskStateCnt_Suspend > 0){ return;}	
		switch(ln_){
			case 0:	 Render2D_BF( );	break;
			case 1:	 Render2D_AF( );	break;
		}
	}
	void T_Draw3D(DWORD ln_ ){
		if(		this->taskStateCnt_Kill > 0  ||
				this->taskStateCnt_Suspend > 0){ return;}	
		switch(ln_){
		case 0:	 Render3D_L0( );	break;
		case 1:	 Render3D_L1( );	break;
		case 2:	 Render3D_L2( );	break;
		case 3:	 Render3D_L3( );	break;
		}
	}
	virtual string DebugOut( )
	{
		stringstream ss;
		if(	this->taskStateCnt_Kill > 0 ){		ss << "消滅準備" << this->taskStateCnt_Kill << ":"; }
		if(	this->taskStateCnt_Stop > 0 ){		ss << "停止" << this->taskStateCnt_Stop << ":"; }
		if(	this->taskStateCnt_Suspend > 0 ){	ss << "表示もOFF" << this->taskStateCnt_Suspend << ":"; }
		if(	this->taskStateCnt_Kill >= 0  &&
			this->taskStateCnt_Stop >= 0  &&
			this->taskStateCnt_Suspend >= 0){ ss << "通常";}	
		return ss.str( );
	}
	virtual void UpDate() = 0;
	virtual void UpDate_Stop(){};
	virtual void Render2D_BF( ){render2D_Active[0]=false;};
	virtual void Render2D_AF( ){render2D_Active[1]=false;};

	virtual void Render3D_L0( ){render3D_Active[0]=false;};
	virtual void Render3D_L1( ){render3D_Active[1]=false;};
	virtual void Render3D_L2( ){render3D_Active[2]=false;};
	virtual void Render3D_L3( ){render3D_Active[3]=false;};
	virtual void UpDate3DPriority() { };

	//状態カウンタ増加or減少
	//消滅させる
	void Kill(bool  nt_ = true)
	{
		this->nextTaskCreate = nt_;
		this->taskStateCnt_Kill++;
		//子供を巻き込む
		for (auto it = this->children.begin(); it != this->children.end(); it++){
			if (auto  c = (*it).lock()) {
				c->Kill(nt_);
			}
		}
	}
	//サスペンドにする
	void Suspend(bool  f_=true)
	{
		if(f_){		this->taskStateCnt_Suspend++;}
		else{		this->taskStateCnt_Suspend = max(this->taskStateCnt_Suspend-1, 0);}
		//子供を巻き込む
		for (auto it = this->children.begin(); it != this->children.end(); it++){
			if (auto  c = (*it).lock()) {
				c->Suspend(f_);
			}
		}
	}
	//停止状態にする
	void Stop(bool  f_=true)
	{
		if(f_){		this->taskStateCnt_Stop++;}
		else{		this->taskStateCnt_Stop = max(this->taskStateCnt_Stop-1, 0);}
		//子供を巻き込む
		for (auto it = this->children.begin(); it != this->children.end(); it++){
			if (auto  c = (*it).lock()) {
				c->Stop(f_);
			}
		}
	}
	//状態を調べる（複合時は優先度の高いものが帰る）
	State  CheckState( ){
		if(			this->taskStateCnt_Kill > 0){		return  State::Kill;}
		else if(	this->taskStateCnt_Suspend > 0){	return  State::Suspend;}
		else if(	this->taskStateCnt_Stop > 0){		return  State::Stop;}
		else{											return  State::Active;}
	}
public:
	WP				me;
	WP				root;		//ルート
	WP				parent;		//親
	vector<WP>		children;	//子

	void   SetParent(SP  parent_)
	{
		//現在の親が設定されている場合、縁切りを行う
		if (auto  pa = this->parent.lock()){
			//親に子供としての縁を切らせる
			auto  m = this->me.lock();
			for (auto it = pa->children.begin();
				it != pa->children.end();
				it++){
				auto c = (*it).lock( );
				if (c == m){
					pa->children.erase(it);
					break;
				}
			}
			//子に親を捨てさせる
			this->parent.reset( );
			this->root = this->me;
		}

		//親子関係を登録する
		if (parent_){
			this->root = parent_->root;
			this->parent = parent_;
			parent_->children.push_back(this->me);
		}
	}
	void   SetParent(WP  parent_)
	{
		SetParent(parent_.lock());
	}
};

//	ゲームエンジン
class GameEngine
{
	static  const  DWORD    vrDef  = 32;//タスク検出時のベクターサイズ初期値

public:
	DWORD			frameCnt;			
	DWORD			screenWidth;		//	画面サイズ
	DWORD			screenHeight;		//	画面サイズ
	float			viewScaleW;			//	表示倍率
	float			viewScaleH;			//	表示倍率
	DWORD			multiSample;		//	マルチサンプル
	bool			screenMode;			//	フルスクリーンモード
	DWORD			screen2DWidth;		//	2D描画用解像度
	DWORD			screen2DHeight;		//	2D描画用解像度
	const TCHAR*	windowTitle;		//	ウインドウタイトル
	const TCHAR*	windowClassName;	//	ウィンドウクラス名
	bool			quitRequest;		//	GameMainに終了要請する時trueにする
	bool			layerEnable2D[2];	//	レイヤーのON・OFF
	bool			layerEnable3D[4];	//	レイヤーのON・OFF
	bool			debugMode;
	DG::Font::SP	debugFont;
	DG::DGObject::SP	dgi;
	XI::Obj::SP			dii;
	DM::Obj::SP			dmi;

public:
	//アップデートとレンダーの呼び出しに対する設定
	//双方が同値の場合、双方が１フレームに１回呼び出される
	//例えば、２：１で設定すると、アップデート２回に対してレンダーは１回呼び出されるようになる
	WORD		updateCall;
	WORD		renderCall;
protected:
	WORD		updateCnt;
	WORD		renderCnt;

private:
	vector<pair<DWORD, BTask::SP>>	tasks_detect;	//現在登録されている全タスク(検出有効な）
	vector<BTask::SP>  tasks_undetect;		//現在登録されている全タスク(検出無効な）
	vector<BTask::SP>  add_Tasks;	//現在登録を待っている全タスク
public:
  bool QuitFlag() { return quitRequest; };
	GameEngine(
		DWORD				sw_,	//	画面サイズ
		DWORD				sh_,	//	画面サイズ
		float				vs_,	//	表示倍率
		DWORD				ms_,	//	マルチサンプルアンチエイリアス（1,2,4)
		bool				sm_,	//	フルスクリーンモード
		DWORD				sw2D_,	//	2D描画用解像度
		DWORD				sh2D_,	//	2D描画用解像度
		const TCHAR*		wtn_,	//	ウインドウタイトル
		const TCHAR*		wcn_,	//	ウィンドウクラス名
		WORD				ucall_=1,	//
		WORD			    rcall_=1)	//
	:screenWidth(sw_)
	,screenHeight(sh_)
	,viewScaleW(vs_)
	,viewScaleH(vs_)
	,multiSample(ms_)
	,screenMode(sm_)
	,screen2DWidth(sw_)
	,screen2DHeight(sh_)
	,windowTitle(wtn_)
	,windowClassName(wcn_)
	,quitRequest(false)
	,frameCnt(0)
	, updateCall(ucall_)
	, renderCall(rcall_)
	, updateCnt(0)
	,renderCnt(0)

	{
		//擬似フルスクリーンモードによる変更
		if(screenMode == true)
		{
			//現在のスクリーンサイズを使用する
			RECT dtr;
			if(GetWindowRect(GetDesktopWindow( ), &dtr)){
				this->viewScaleW = (float)dtr.right  / (float)screenWidth;
				this->viewScaleH = (float)dtr.bottom / (float)screenHeight;	
			}
		}
	}
	//	ゲーム処理
	void B_Step(HWND wnd_);
		void  B_StepSub_CallTaskUpDate( );
		void  B_UpDateSub_CallTaskUpDate3DPriority();
		void  B_StepSub_CallTaskRender2D_BF( );
		void  B_StepSub_CallTaskRender3D_4( );
		void  B_StepSub_CallTaskRender2D_AF( );
		void  B_StepSub_DebugOut( );
		void  B_StepSub_DeleteTask( );
		void  B_StepSub_Entry( );
	//	初期化
	bool B_Initialize(HWND wnd_);
	//	解放
	virtual ~GameEngine( );
	//	タスクの登録
	void PushBack(const BTask::SP& t_);
	//	同名のタスクを全て削除する
	bool KillAll_G(const string& gn_, bool  nt_ = true);
	bool KillAll_GN(const string& gn_,  const string& n_, bool  nt_ = true);
	//	同名のタスクを全て停止する
	bool StopAll_G(const string& gn_, bool  m_ = true);
	bool StopAll_GN(const string& gn_,  const string& n_, bool  m_ = true);
	//	同名のタスクを全て一時無効にする
	bool SuspendAll_G(const string& gn_, bool  m_ = true);
	bool SuspendAll_GN(const string& gn_,  const string& n_, bool  m_ = true);
	//	レイヤーの有効無効を設定
	void Layer_2D(DWORD ln_, bool st_){	if(ln_>=0 && ln_<2){layerEnable2D[ln_] = st_;}}
	bool Layer_2D(DWORD ln_){				if(ln_>=0 && ln_<2){return layerEnable2D[ln_];}else{ return false;}}
	void Layer_3D(DWORD ln_, bool st_){	if(ln_>=0 && ln_<4){layerEnable3D[ln_] = st_;}}
	bool Layer_3D(DWORD ln_){			if(ln_>=0 && ln_<4){return layerEnable3D[ln_];}else{ return false;}}

	void Layer_2D(bool s0_, bool s1_){layerEnable2D[0] = s0_;	layerEnable2D[1] = s1_;}
	void Layer_3D(bool s0_, bool s1_, bool s2_, bool s3_){layerEnable3D[0] = s0_;	layerEnable3D[1] = s1_;	layerEnable3D[2] = s2_;	layerEnable3D[3] = s3_;}
	DWORD FrameCnt(){return frameCnt;};			

	//デバッグ機能のON/OFF
	void DebugMode(bool dm_){debugMode = dm_;}

	//┌───────────────────────────────────────────┐
	//｜同名で最初に登場するタスクを返す（対象が単体の時しか使えない）						  ｜
	//｜履歴		：2014/02/05	須賀康之	変更										　｜
	//｜履歴		：2015/02/10	須賀康之	変更										　｜
	//｜履歴		：2017/01/23	須賀康之	変更										　｜
	//｜履歴		：2020/01/23	須賀康之	変更	(教科書カラー化に伴いメソッド名を元に戻した）									　｜
	//└───────────────────────────────────────────┘
	template <class T>
	shared_ptr<T>  GetTask(
			 const  string&  gn_	//グループ名
			,const  string&  n_		//固有名
	){
		DWORD  gKey = 0;
		memcpy(&gKey, gn_.c_str( ), min(4, gn_.length( )));
		for(auto it = tasks_detect.begin( );  it != tasks_detect.end( );  it++){
			if( (*it).second  ==  nullptr ){				continue;}
			if( (*it).second->TaskStateCnt_Kill( )  >  0 ){	continue;}
			if( (*it).second->GroupKey( ) !=  gKey ){		continue;}
			if( (*it).second->groupName != gn_){			continue;}
			if( (*it).second->name != n_){					continue;}
			return  static_pointer_cast<T>((*it).second);
		}
		//登録待ちタスクに対しても処理を行う(2017/01/23)
		for (auto it = add_Tasks.begin(); it != add_Tasks.end(); it++) {
			if ((*it) == nullptr) {							continue; }
			if ((*it)->Detectible() == false) {				continue; }
			if ((*it)->TaskStateCnt_Kill()  >  0) {			continue; }
			if ((*it)->GroupKey() != gKey) {				continue; }
			if ((*it)->groupName != gn_) {					continue; }
			if ((*it)->name != n_) {						continue; }
			return  static_pointer_cast<T>((*it));
		}
		return  nullptr;
	}
	//┌───────────────────────────────────────────┐
	//｜同名で最初に登場するタスクを返す（対象が単体の時しか使えない）						  ｜
	//｜履歴		：2014/02/05	須賀康之	変更										　｜
	//｜履歴		：2015/02/10	須賀康之	変更										　｜
	//｜履歴		：2017/01/23	須賀康之	変更										　｜
	//｜履歴		：2020/01/23	須賀康之	変更	(教科書カラー化に伴いメソッド名を元に戻した）									　｜
	//└───────────────────────────────────────────┘
	template <class T>
	shared_ptr<T> GetTask(
		const  string&  gn_		//グループ名
	){
		DWORD  gKey = 0;
		memcpy(&gKey, gn_.c_str( ), min(4, gn_.length( )));
		for(auto it = tasks_detect.begin( );  it != tasks_detect.end( );  it++){
			if( (*it).second  ==  nullptr ){				continue;}
			if( (*it).second->TaskStateCnt_Kill( )  >  0 ){	continue;}
			if( (*it).second->GroupKey( ) !=  gKey ){		continue;}
			if( (*it).second->groupName != gn_){			continue;}
			return  static_pointer_cast<T>((*it).second);
		}
		//登録待ちタスクに対しても処理を行う(2017/01/23)
		for (auto it = add_Tasks.begin(); it != add_Tasks.end(); it++) {
			if ((*it) == nullptr) {							continue; }
			if ((*it)->Detectible() == false) {				continue; }
			if ((*it)->TaskStateCnt_Kill()  >  0) {			continue; }
			if ((*it)->GroupKey() != gKey) {				continue; }
			if ((*it)->groupName != gn_) {					continue; }
			return  static_pointer_cast<T>((*it));
		}

		return  nullptr;
	}
	//┌───────────────────────────────────────────┐
	//｜同グループ名の対象を全て検出し、そのベクターを返す									  ｜
	//｜履歴		：2014/02/05	須賀康之	変更										　｜
	//｜履歴		：2015/02/10	須賀康之	変更										　｜
	//｜履歴		：2017/01/23	須賀康之	変更										　｜
	//｜履歴		：2020/01/23	須賀康之	変更	(教科書カラー化に伴いメソッド名を元に戻した）									　｜
	//└───────────────────────────────────────────┘
	template <class T>
	shared_ptr<vector<shared_ptr<T>>>  GetTasks(
			const  string&  gn_,
			const  string&  n_,
			DWORD  vectorReserve_ = vrDef,
			DWORD  exclusionKey_ = 0)
	{
		shared_ptr<vector<shared_ptr<T>>>  w  =  
			shared_ptr<vector<shared_ptr<T>>>(new vector<shared_ptr<T>>( ));
		w->reserve(vectorReserve_);
		DWORD  gKey = 0;
		memcpy(&gKey, gn_.c_str( ), min(4, gn_.length( )));

		for(auto it = tasks_detect.begin( );  it != tasks_detect.end( );  it++){
			if( (*it).second  ==  nullptr ){				continue;}
			if( (*it).second->TaskStateCnt_Kill( )  >  0 ){	continue;}
			if( (*it).second->UKey( ) == exclusionKey_){	continue;}
			if( (*it).second->GroupKey( ) !=  gKey ){		continue;}
			if( (*it).second->groupName != gn_){			continue;}
			if( (*it).second->name != n_){					continue;}
			w->push_back( static_pointer_cast<T>((*it).second) );
		}
		//登録待ちタスクに対しても処理を行う(2017/01/23)
		for (auto it = add_Tasks.begin(); it != add_Tasks.end(); it++) {
			if ((*it) == nullptr) {							continue; }
			if ((*it)->Detectible() == false) {				continue; }
			if ((*it)->TaskStateCnt_Kill()  >  0) {			continue; }
			if ((*it)->GroupKey() != gKey) {				continue; }
			if ((*it)->groupName != gn_) {					continue; }
			if ((*it)->name != n_) {						continue; }
			w->push_back(static_pointer_cast<T>((*it)));
		}
		return w;
	}

	//┌───────────────────────────────────────────┐
	//｜同グループ名の対象を全て検出し、そのベクターを返す									  ｜
	//｜履歴		：2014/02/05	須賀康之	変更										　｜
	//｜履歴		：2015/02/10	須賀康之	変更										　｜
	//｜履歴		：2017/01/23	須賀康之	変更										　｜
	//｜履歴		：2020/01/23	須賀康之	変更	(教科書カラー化に伴いメソッド名を元に戻した）									　｜
	//└───────────────────────────────────────────┘
	template <class T>
	shared_ptr<vector<shared_ptr<T>>>  GetTasks(
			const  string&  gn_,
			DWORD  vectorReserve_ = vrDef,
			DWORD  exclusionKey_ = 0)
	{
		shared_ptr<vector<shared_ptr<T>>>  w  =  
			shared_ptr<vector<shared_ptr<T>>>(new vector<shared_ptr<T>>( ));
		w->reserve(vectorReserve_);
		DWORD  gKey = 0;
		memcpy(&gKey, gn_.c_str( ), min(4, gn_.length( )));

		for(auto it = tasks_detect.begin( );  it != tasks_detect.end( );  it++){
			if( (*it).second  ==  nullptr ){				continue;}
			if( (*it).second->TaskStateCnt_Kill( )  >  0 ){	continue;}
			if( (*it).second->UKey( ) == exclusionKey_){	continue;}
			if( (*it).second->GroupKey( ) !=  gKey ){		continue;}
			if( (*it).second->groupName != gn_){			continue;}
			w->push_back( static_pointer_cast<T>((*it).second) );
		}
		//登録待ちタスクに対しても処理を行う(2017/01/23)
		for (auto it = add_Tasks.begin(); it != add_Tasks.end(); it++) {
			if ((*it) == nullptr) {							continue; }
			if ((*it)->Detectible() == false) {				continue; }
			if ((*it)->TaskStateCnt_Kill()  >  0) {			continue; }
			if ((*it)->GroupKey() != gKey) {				continue; }
			if ((*it)->groupName != gn_) {					continue; }
			w->push_back(static_pointer_cast<T>((*it)));
		}
		return w;
	}
	virtual bool Initialize(HWND wnd_)=0;
	virtual void UpDate( )=0;

	//3DPG1対応により追加
	//2D描画環境のデフォルトパラメータ設定
	virtual void Set2DRenderState(DWORD l_)=0;
	//3D描画環境のデフォルトパラメータ設定
	virtual void Set3DRenderState(DWORD l_)=0;
};




