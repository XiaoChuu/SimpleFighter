#pragma warning(disable:4996)
#pragma once

#include "DG2014_DGObject.h"

#include "DG2014_IndexBuffer.h"
#include "DG2014_InputLayout.h"
#include "DG2014_VertexBuffer.h"
#include "DG2014_VertexContainer.h"

#include "DG2014_Texture.h"
#include "DG2014_Mesh.h"
#include "DG2014_Image.h"
#include "DG2014_Font.h"
#include "DG2014_Effect.h"

namespace DG
{
	const UINT spriteBufferMax = 4096;
	//---------------------------------------------------------------------------
	//画像マネージャー
	class  ImageManager {
	private:
		static  unordered_map<string, DG::Image::SP>	arr;
		static  bool notObject;
		ImageManager() { notObject = false; }
	public:
		//登録
		static  DG::Image::SP  Set(const string name_, DG::Image::SP sp_);
		//名前で検索
		static  DG::Image::SP  Get(const string name_);
		//単体破棄
		static  void  Clear(const string name_);
		//すべて破棄
		static  void  AllClear();
	};
	//---------------------------------------------------------------------------
	//フォントマネージャー
	class  FontManager {
	private:
		static  unordered_map<string, DG::Font::SP>	arr;
		static  bool notObject;
		FontManager() { notObject = false; }
	public:
		//登録
		static  DG::Font::SP  Set(const string name_, DG::Font::SP sp_);
		//名前で検索
		static  DG::Font::SP  Get(const string name_);
		//単体破棄
		static  void  Clear(const string name_);
		//すべて破棄
		static  void  AllClear();
	};
	//---------------------------------------------------------------------------
	//メッシュマネージャー
	class  MeshManager {
	private:
		static  unordered_map<string, DG::Mesh::SP>	arr;
		static  bool notObject;
		MeshManager() { notObject = false; }
	public:
		//登録
		static  DG::Mesh::SP  Set(const string name_, DG::Mesh::SP sp_);
		//名前で検索
		static  DG::Mesh::SP  Get(const string name_);
		//単体破棄
		static  void  Clear(const string name_);
		//すべて破棄
		static  void  AllClear();
	};
	//---------------------------------------------------------------------------
	//テクスチャマネージャー
	class  TextureManager {
	private:
		static  unordered_map<string, DG::Texture::SP>	arr;
		static  bool notObject;
		TextureManager() { notObject = false; }
	public:
		//登録
		static  DG::Texture::SP  Set(const string name_, DG::Texture::SP sp_);
		//名前で検索
		static  DG::Texture::SP  Get(const string name_);
		//単体破棄
		static  void  Clear(const string name_);
		//すべて破棄
		static  void  AllClear();
	};
	//-----------------------------------------------------------------------
	//void  Line_Draw(
	//	const ML::Vec3&	posS_,
	//	const ML::Vec3&	posE_);
}