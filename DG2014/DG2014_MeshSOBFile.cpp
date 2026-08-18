#include "DG2014_Ver3_6.h"
#pragma warning(disable:4316)

//OBJファイルは変換を行い、.SOB形式に変換したファイルを読み込むものとなった
namespace MeshLoad_SOB
{
	struct FileHeader
	{
		char  kind[32];
		bool Check( )
		{
			string  k =  kind;
			string  v =  "3DMesh YS Ver0.2";
			return  k == v;
		}
	};
	//マテリアルコンテナ
	class MaterialContainerS{
	public:
		char					name[64];	//マテリアル名
		ML::Color				diffuse;	//マテリアル情報:デフューズ
		ML::Color				ambient;	//マテリアル情報:アンビエント
		ML::Color				speculer;	//マテリアル情報:スペキュラ
		ML::Color				emissive;	//マテリアル情報:エミッシブ
		char					texName[64];//マテリアル名
		char					bw[320];//未使用エリア（合計512バイト）
	};
	class MeshGroupS{
	public:
		DWORD					mcNumber;	//対応するマテリアル番号
		DWORD					indexNum;	//インデックス数
		vector<DWORD>			vIndex;		//インデックス
	};
}

namespace DG
{
	vector<MeshLoad_SOB::MaterialContainerS>	mateConts;
	vector<MeshLoad_SOB::MeshGroupS>			meshGroups;

	//SOBファイルからメッシュを生成する
	Mesh::SP  Mesh::Create_FromSOBFileSub(	const string&			fpath_)
	{
		if( Mesh::SP sp = Mesh::SP( new Mesh( ) ) )
		{
			if( sp->Initialize_FromSOBFile(fpath_) ){
				return sp;
			}
		}
		return nullptr;
	}
	//SOBファイルからメッシュを生成する
	bool Mesh::Initialize_FromSOBFile(	const string&		fpath_)
	{
		//ファイルを開いて内容を読み込む
		FILE* fp=NULL;
		fopen_s(&fp, fpath_.c_str( ),"rb");
		if(fp == nullptr){
			MessageBox(nullptr, "ファイルが開けません。", fpath_.c_str( ), MB_OK);
			return false;
		}
		//ファイルチェック
		MeshLoad_SOB::FileHeader  wfh;
		fread(&wfh, sizeof(MeshLoad_SOB::FileHeader), 1, fp);
		if(!wfh.Check( )){	
			MessageBox(nullptr, "ファイルバージョンが一致しません。対応バージョンはSOB Ver0.2です", fpath_.c_str( ), MB_OK);
			return false;
		}


		//マテリアル情報読み込み
		DWORD  mateNum;
		fread(&mateNum, sizeof(DWORD), 1, fp);
		vector<MeshLoad_SOB::MaterialContainerS>  mateContS(mateNum);
		fread(&mateContS[0], sizeof(MeshLoad_SOB::MaterialContainerS), mateContS.size( ), fp);

		//メッシュグループ読み込み
		DWORD  mesnGNum;
		fread(&mesnGNum, sizeof(DWORD), 1, fp);
		vector<MeshLoad_SOB::MeshGroupS>  meshG(mesnGNum);
		for(DWORD  c=0; c<mesnGNum; c++){
			DWORD  ww;
			fread(&ww, sizeof(DWORD), 1, fp);
			meshG[c].mcNumber = ww;
			fread(&ww, sizeof(DWORD), 1, fp);
			meshG[c].indexNum = ww;
			//fread(&meshG[c].mcNumber, sizeof(DWORD), 1, fp);
			//fread(&meshG[c].indexNum, sizeof(DWORD), 1, fp);
			meshG[c].vIndex.resize( meshG[c].indexNum );
			fread(&meshG[c].vIndex[0], sizeof(DWORD), meshG[c].indexNum, fp);
		}

		//頂点情報関連の読み出し
		DWORD  vCoordNum;
		vector<ML::Vec3>  vCoord;
		DWORD  vUVNum;
		vector<ML::Vec2>  vUV;
		DWORD  vNormalNum;
		vector<ML::Vec3>  vNormal;

		while(1)
		{
			char  dataTitle[16];
			if(16 !=  fread(dataTitle, sizeof(char), 16, fp))
			{
				break;
			}
			if(		0 == strcmp(dataTitle, "POSITION") ){
				fread(&vCoordNum, sizeof(DWORD), 1, fp);
				vCoord.resize(vCoordNum);
				fread(&vCoord[0], sizeof(ML::Vec3), vCoord.size( ), fp);
			}
			else if(0 == strcmp(dataTitle, "TEX_UV") ){
				fread(&vUVNum, sizeof(DWORD), 1, fp);
				vUV.resize(vUVNum);
				fread(&vUV[0], sizeof(ML::Vec2), vUV.size( ), fp);
			}
			else if(0 == strcmp(dataTitle, "NORMAL") ){
				fread(&vNormalNum, sizeof(DWORD), 1, fp);
				vNormal.resize(vNormalNum);
				fread(&vNormal[0], sizeof(ML::Vec3), vNormal.size( ), fp);
			}
		}
		//ファイルから必要なデータはすべて読み込んだよ
		fclose(fp);	

	//コンテナに放り込む
		this->vbc = DG::VB_Container::Create(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		bool rtv;
		rtv = vbc->PushBack((BYTE*)&vCoord[0], sizeof(D3DXVECTOR3), (int)vCoord.size( ));
		if(!rtv){ return false; }
		rtv = vbc->PushBack((BYTE*)&vUV[0], sizeof(D3DXVECTOR2), (int)vUV.size( ));
		if(!rtv){ return false; }
		rtv = vbc->PushBack((BYTE*)&vNormal[0], sizeof(D3DXVECTOR3), (int)vNormal.size( ));
		if(!rtv){ return false; }

	//メッシュグループ（インデックスとマテリアルIDで構成するグループ）
		for(auto  it = meshG.begin( );  it != meshG.end( );  it++){
			MeshGroup  wMeshGroup;
			wMeshGroup.mcNumber = it->mcNumber;
			wMeshGroup.index = DG::IndexBuffer::Create((BYTE*)&it->vIndex[0], sizeof(DWORD), (int)it->vIndex.size( ));
			//メッシュグループに設定
			meshGroups.push_back(wMeshGroup);
		}
	//マテリアルの生成
		for(auto  it = mateContS.begin( );  it != mateContS.end( );  it++){
			Material  wMateCon;
			wMateCon.ambient  = it->ambient;
			wMateCon.diffuse  = it->diffuse;
			wMateCon.speculer = it->speculer;
			wMateCon.emissive = it->emissive;
			wMateCon.name     = it->name;
			if(it->texName[0] != '\0'){
				wMateCon.tex  = DG::Texture::Create(ML::CreateFilePath(fpath_, it->texName));
			}
			mateConts.push_back(wMateCon);
		}

	//===========================================
		//バーテックスシェーダ生成＆頂点インプットレイアウト生成
		//頂点インプットレイアウトを定義	
		D3D10_INPUT_ELEMENT_DESC layout[] =	{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		1, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,	2, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
		};
		//頂点インプットレイアウトの生成
		this->tecName = "P0";
		this->inputLayout  = DG::InputLayout::Create(this->tecName, layout, _countof(layout));
		if(!this->inputLayout ){return false;}

		return true;
	}
}
