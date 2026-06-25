#pragma warning(disable:4996)
#pragma once
#pragma warning( disable : 4005 )
#include <d3dx10.h>
#include <d3dCompiler.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma warning( default : 4005 )


#include <string>

namespace DG
{
	//-----------------------------------------------------------------------
	//2D文字描画クラス
	class Font
	{
	public:
		static  bool  dt;	//文字描画を行った際はtrue、
	//-----------------------------------------------------------------------
	public:
		using SP = shared_ptr<Font>;
		using WP = weak_ptr<Font>;
	//-----------------------------------------------------------------------
	private:
		Font( );
	//-----------------------------------------------------------------------
	private:
		bool  Initialize(
				const string&		fname_,
				WORD				width_,
				WORD				height_,
				WORD				weight_,
				BYTE				charSet_);
	//-----------------------------------------------------------------------
	private:
		ID3DX10Font*	font;
	//-----------------------------------------------------------------------
	public:
		~Font( );
	//-----------------------------------------------------------------------
	public:
		//生成する
		static SP Create(	
				const string&	fname_,
				WORD			width_,
				WORD			height_, 
				WORD			weight_ = 500,
				BYTE			charSet_ = SHIFTJIS_CHARSET);
		//意図的に破棄する
		static void Destroy(SP sp_);
	//-----------------------------------------------------------------------
	private:
		//描画
		void DrawS(
				const ML::Box2D&		draw_,
				const string&			tex_,
				ML::Color				color_,
				UINT					uFormat_);
	//-----------------------------------------------------------------------
	public:
		//描画
		void Draw(
				const ML::Box2D&		draw_,	//	描画先
				const string&			tex_,
				ML::Color				color_ = ML::Color(1,1,1,1),
				UINT					uFormat_ = DT_LEFT);
		//	フレーム付き文字列を表示する
		enum FRAME{ x1, x2, x4};
		void DrawF(
				const ML::Box2D&		draw_,	//	描画先
				const string&			tex_,
				FRAME					mode_,
				ML::Color				color_ = ML::Color(1,1,1,1),
				ML::Color				fColor_ = ML::Color(1,0,0,0),
				UINT					uFormat_ = DT_LEFT);
		//デバッグ用機能
		template<class  v>
		void  Dbg_DrawNumber(int x_, int y_, v  num_, 
								ML::Color  color_ = ML::Color(1, 1, 0, 0))
		{
			ML::Box2D  draw(x_, y_, 200, 50);
			string  num = to_string(num_);
			Draw(draw, num, color_);
		}
	//-----------------------------------------------------------------------
	private:
		static  unordered_map<string, WP>   rep;
	};
}
