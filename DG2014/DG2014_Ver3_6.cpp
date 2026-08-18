#include "DG2014_Ver3_6.h"
#include <map>

namespace  DG
{
	//-----------------------------------------------------------------------
	//Image系
	//-----------------------------------------------------------------------
	unordered_map<string, DG::Image::SP>	ImageManager::arr;
	bool ImageManager::notObject;
	DG::Image::SP  ImageManager::Set(const string name_, DG::Image::SP sp_)
	{
		arr[name_] = sp_;
		return sp_;
	}
	//-----------------------------------------------------------------------
	DG::Image::SP  ImageManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("ImageManager 対象が存在しません"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  ImageManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void ImageManager::AllClear()
	{
		arr.clear();
	}

	//-----------------------------------------------------------------------
	//Font系
	//-----------------------------------------------------------------------
	unordered_map<string, DG::Font::SP>	FontManager::arr;
	bool FontManager::notObject;
	DG::Font::SP  FontManager::Set(const string name_, DG::Font::SP sp_)
	{
		arr[name_] = sp_;
		return sp_;
	}
	//-----------------------------------------------------------------------
	DG::Font::SP  FontManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("FontManager 対象が存在しません"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  FontManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void FontManager::AllClear()
	{
		arr.clear();
	}
	//-----------------------------------------------------------------------
	//Mesh系
	//-----------------------------------------------------------------------
	unordered_map<string, DG::Mesh::SP>	MeshManager::arr;
	bool MeshManager::notObject;
	DG::Mesh::SP  MeshManager::Set(const string name_, DG::Mesh::SP sp_)
	{
		arr[name_] = sp_;
		return sp_;
	}
	//-----------------------------------------------------------------------
	DG::Mesh::SP  MeshManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("MeshManager 対象が存在しません"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  MeshManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void MeshManager::AllClear()
	{
		arr.clear();
	}
	//-----------------------------------------------------------------------
	//Mesh系
	//-----------------------------------------------------------------------
	unordered_map<string, DG::Texture::SP>	TextureManager::arr;
	bool TextureManager::notObject;
	DG::Texture::SP  TextureManager::Set(const string name_, DG::Texture::SP sp_)
	{
		arr[name_] = sp_;
		return  sp_;
	}
	//-----------------------------------------------------------------------
	DG::Texture::SP  TextureManager::Get(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			if (notObject == false) {
				MessageBox(nullptr, name_.c_str(), _T("TextureManager 対象が存在しません"), MB_OK);
			}
			notObject = true;
			return nullptr;
		}
		return it->second;
	}
	//-----------------------------------------------------------------------
	void  TextureManager::Clear(const string name_)
	{
		auto it = arr.find(name_);
		if (it == arr.end()) {
			return;
		}
		arr.erase(it);
	}
	//-----------------------------------------------------------------------
	void TextureManager::AllClear()
	{
		arr.clear();
	}


	//void  Line_Initialize();
	//void  Line_Finalize();

	//D3D10_PRIMITIVE_TOPOLOGY	line_priTopo;		//プリミティブトポロジー
	//DG::InputLayout::SP		line_inputLayout;	//頂点情報の入力レイアウト
	//DWORD						line_tecNum;		//テクニック番号
	//DG::VertexBuffer::SP		line_vPos;

	////-----------------------------------------------------------------------
	//void  Line_Draw(
	//	const ML::Vec3&	posS_,
	//	const ML::Vec3&	posE_)
	//{
	//	//バーテックスバッファの生成
	//	ML::Vec3	pos[] = { posS_, posE_ };
	//	auto  vPos = DG::VertexBuffer::Create((BYTE*)pos, sizeof(ML::Vec3), _countof(pos));

	//	//描画パラメータを定数バッファに反映させる
	//	obj->EffectState().Update_Params();

	//	//テクニック選択（呼び出されるシェーダを決定する）
	//	obj->ETechnique().GetPassByIndex(line_tecNum)->Apply(0);
	//	//頂点インプットレイアウトをセット
	//	line_inputLayout->IASet();
	//	//プリミティブ・トポロジーをセット
	//	obj->Device().IASetPrimitiveTopology(line_priTopo);

	//	//頂点バッファを各スロットに接続（素材投入口選択）
	//	UINT  ofsPos = 0 * sizeof(ML::Vec3);
	//	obj->Device().IASetVertexBuffers(0, 1, &vPos->buf, &vPos->stride, &ofsPos);
	//	//レンダリング開始
	//	obj->Device().Draw(vPos->num, 0);
	//}
	//void  Line_Initialize()
	//{
	//	//バーテックスシェーダ生成＆頂点インプットレイアウト生成
	//	//頂点インプットレイアウトを定義	
	//	D3D10_INPUT_ELEMENT_DESC layout[] = {
	//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	//	};
	//	//頂点インプットレイアウトの生成
	//	line_tecNum = 2;
	//	line_inputLayout = DG::InputLayout::Create(line_tecNum, layout, _countof(layout));
	//	//プリミティブ・トポロジーを選択
	//	line_priTopo = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
	//}
	//void  Line_Finalize()
	//{
	//}
}