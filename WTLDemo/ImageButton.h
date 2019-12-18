#pragma once
#include <atlwin.h>
#include <atlimage.h>

// CImageButton

// ReSharper disable once CppClassCanBeFinal
// ReSharper disable once CppInconsistentNaming
class CImageButton // NOLINT(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)
	: public CWindowImpl<CImageButton, CButton> 
	, public COwnerDraw<CImageButton>
{
public:
	CImageButton()
		: m_images()
		, m_bMouseTrack(TRUE)
	{
		
	}
	
	virtual ~CImageButton()
	{
		ReleaseImages();
	}

	BEGIN_MSG_MAP(CImageButton)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CImageButton>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()


	/**
	 * \brief 计算给定数值的dpi数值
	 * \param xy 数值
	 * \param dpi 当前dpi
	 */
#define DPI(xy, dpi) MulDiv(xy, dpi, 96)  // NOLINT(cppcoreguidelines-macro-usage)

	 /**
	  * \brief 获取dpi
	  * \param hWnd 窗口句柄
	  * \return dpi x
	  */
	int GetDpi(HWND hWnd)
	{
		// GetDeviceCaps()支持win2000+操作系统
		const auto hDc = GetDC();
		const auto	nDpi = GetDeviceCaps(hDc, LOGPIXELSX);
		// GetDeviceCaps(hDc, LOGPIXELSY);
		ReleaseDC(hDc);
		return nDpi;
	}
	
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		CDC dc = lpDrawItemStruct->hDC;

		// 按钮是否被禁用
		if (lpDrawItemStruct->itemState & ODS_DISABLED)
		{
			m_state = Disable;
		}

		CDC memDc;
		CBitmap bmp;

		// 匹配图片
		auto&& image = m_images[m_state];
		if (image.IsNull())
		{
			dc.DrawText(_T("未设置图像"), _tcslen(_T("未设置图像")), &lpDrawItemStruct->rcItem, DT_CENTER | DT_VCENTER);
			return;
		}

		const auto nDpi = GetDpi(lpDrawItemStruct->hwndItem);

		// 图片宽度
		const auto nWidth = DPI(image.GetWidth(), nDpi);

		// 图片高度
		const auto nHeight = DPI(image.GetHeight(), nDpi);

		// 创建双缓冲的内存dc
		memDc.CreateCompatibleDC(dc);
		// 创建内存dc兼容的位图
		bmp.CreateCompatibleBitmap(dc, nWidth, nHeight);
		// 内存dc选择位图
		memDc.SelectBitmap(bmp);

		// 绘制到内存dc
		// image.StretchBlt(memDc, 0, 0, nWidth, nHeight, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
		image.Draw(memDc, 0, 0, nWidth, nHeight, 0, 0, image.GetWidth(), image.GetHeight());

		// 使用透明通道贴图
		BLENDFUNCTION bf;
		bf.AlphaFormat = AC_SRC_ALPHA;
		bf.BlendFlags = 0;
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = 255;
		dc.AlphaBlend(0, 0, nWidth, nHeight, memDc, 0, 0, nWidth, nHeight, bf);
		
		memDc.RestoreDC(-1);
	}

protected:
	
	// ReSharper disable once CppMemberFunctionMayBeStatic
	// ReSharper disable once IdentifierTypo
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// 处理背景擦除消息, 实现完全透明
		bHandled = TRUE;
		return 0;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// 鼠标按下状态
		m_state = Press;
		Invalidate();

		bHandled = FALSE;
		return TRUE;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// 鼠标弹起设置为悬停状态, 因为鼠标还在按钮上
		m_state = Hover;
		Invalidate();
		bHandled = FALSE;
		return TRUE;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// 追踪鼠标
		if (m_bMouseTrack)
		{
			TRACKMOUSEEVENT eventTrack;
			eventTrack.cbSize = sizeof(eventTrack);
			eventTrack.dwFlags = TME_LEAVE | TME_HOVER;
			eventTrack.hwndTrack = m_hWnd;					// 指定要追踪的窗口
			eventTrack.dwHoverTime = 1;						// 鼠标在按钮上停留超过1ms, 才认为状态为HOVER
			_TrackMouseEvent(&eventTrack);					// 开启Windows的WM_MOUSELEAVE, WM_MOUSEHOVER事件支持
			m_bMouseTrack = FALSE;							// 若已经追踪, 则停止追踪
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnMouseHover(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// 鼠标悬停状态
		m_state = Hover;
		Invalidate();

		bHandled = FALSE;
		return 0;
	}
	
	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// 鼠标离开状态
		m_state = Normal;

		// 继续追踪鼠标
		m_bMouseTrack = TRUE;

		Invalidate();

		bHandled = FALSE;
		return 0;
	}
		
public:

	BOOL SubclassWindow(HWND hWnd)
	{
		const auto bRet = CWindowImpl<CImageButton, CButton>::SubclassWindow(hWnd);

		if (bRet )
		{
			ModifyStyle(0, BS_OWNERDRAW);
			SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW);
		}

		return bRet;
	}
	
	/**
	 * \brief 设置按钮图片, 支持[BMP,GIF,JPEG,PNG,TIFF]
	 * \param nNormalId 正常状态图片资源id
	 * \param nHoverId 悬停状态图片资源id
	 * \param nPressId 按下状态图片资源id
	 * \param nDisableId 禁用状态图片资源id, 默认为0, 使用nNormalId
	 * \param lpszResourceType 资源类型, 默认_T("PNG")
	 */
	void SetImages(UINT nNormalId, UINT nHoverId, UINT nPressId, UINT nDisableId = 0, LPCTSTR lpszResourceType = _T("PNG"))
	{
		ReleaseImages();
		LoadImageFromResource(m_images[Normal], nNormalId, lpszResourceType);
		LoadImageFromResource(m_images[Hover], nHoverId, lpszResourceType);
		LoadImageFromResource(m_images[Press], nPressId, lpszResourceType);
		nDisableId = nDisableId == 0 ? nNormalId : nDisableId;
		LoadImageFromResource(m_images[Disable], nDisableId, lpszResourceType);
		// AutoSize();

	}

	/**
	 * \brief 自动大小
	 */
	void AutoSize()
	{
		auto&& image = m_images[Normal];
		if (image.IsNull())
		{
			return;
		}

		const auto nDpi = GetDpi(m_hWnd);

		SetWindowPos(nullptr, -1, -1, DPI(image.GetWidth(), nDpi), DPI(image.GetHeight(), nDpi), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
	}
	
protected:

	/**
	 * \brief 从资源加载图片
	 * \param img 图片对象
	 * \param nId 图片资源id
	 * \param lpszResourceType 资源类型
	 */
	static void LoadImageFromResource(CImage& img, UINT nId, LPCTSTR lpszResourceType)
	{
		const static auto hInstance = ModuleHelper::GetResourceInstance(); // ModuleHelper::GetModuleInstance();

		// 查找资源
		const auto hResource = ::FindResource(hInstance, MAKEINTRESOURCE(nId), lpszResourceType);
		if (hResource == nullptr)
		{
			return;
		}

		// 加载资源
		const auto hResData = LoadResource(hInstance, hResource);
		if (hResData == nullptr)
		{
			return;
		}

		// 锁定资源并得到资源内容指针
		const auto lpResource = LockResource(hResData);
		if (lpResource == nullptr)
		{
			return;
		}

		// 资源大小
		const auto dwResourceSize = SizeofResource(hInstance, hResource);

		// 为流申请资源
		const auto hMem = GlobalAlloc(GMEM_FIXED, dwResourceSize);
		if (hMem == nullptr)
		{
			return;
		}

		const auto lpMem = static_cast<LPBYTE>(GlobalLock(hMem));
		if (lpMem == nullptr)
		{
			GlobalFree(hMem);
			return;
		}

		// 拷贝资源数据到流
		memcpy(lpMem, lpResource, dwResourceSize);

		// 从内存创建流
		const auto pStream = SHCreateMemStream(lpMem, dwResourceSize);
		if (pStream != nullptr)
		{
			if (img.Load(pStream) == S_OK && _tcsicmp(lpszResourceType, _T("PNG")) == 0 && img.GetBPP() == 32)
			{
				// 预处理PNG的Alpha通道
				for (auto i = 0; i < img.GetWidth(); i++)
				{
					for (auto j = 0; j < img.GetHeight(); j++)
					{
						const auto pColor = reinterpret_cast<LPBYTE>(img.GetPixelAddress(i, j));
						pColor[0] = pColor[0] * pColor[3] / 255;
						pColor[1] = pColor[1] * pColor[3] / 255;
						pColor[2] = pColor[2] * pColor[3] / 255;
					}
				}
			}
		}

		GlobalUnlock(lpMem);
		GlobalFree(hMem);
	}


	/**
	 * \brief 释放图片资源
	 */
	void ReleaseImages()
	{
		for (auto&& image : m_images)
		{
			image.Destroy();
		}
	}
	
	/**
	 * \brief 按钮状态
	 */
	enum State
	{
		/**
		 * \brief 正常状态
		 */
		Normal = 0,

		/**
		 * \brief 悬停状态
		 */
		Hover,

		/**
		 * \brief 按下状态
		 */
		Press,

		/**
		 * \brief 禁用状态
		 */
		Disable,
	};
	

	/**
	 * \brief 4种按钮状态图片
	 */
	CImage		m_images[4];

	/**
	 * \brief 按钮状态
	 */
	State		m_state = Normal;
	
	/**
	 * \brief 鼠标追踪
	 */
	BOOL        m_bMouseTrack;
};


