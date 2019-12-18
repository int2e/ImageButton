// ImageButton.cpp: 实现文件
//

#include "pch.h"
#include "ImageButton.h"
#include <VersionHelpers.h>

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
	int nDpi = 0;
	// 如果是win10或更高版本
	if (IsWindows10OrGreater())
	{
		nDpi = GetDpiForWindow(hWnd);
	}
	else
	{
		// GetDeviceCaps()支持win2000+操作系统
		const auto hDc = GetDC(hWnd);
		nDpi = GetDeviceCaps(hDc, LOGPIXELSX);
		// GetDeviceCaps(hDc, LOGPIXELSY);
		ReleaseDC(hWnd, hDc);
	}

	return nDpi;
}

// CImageButton

IMPLEMENT_DYNAMIC(CImageButton, CButton)

CImageButton::CImageButton()
	: m_images()
	, m_bMouseTrack(TRUE)
{

}

CImageButton::~CImageButton()
{
	ReleaseImages();
}


BEGIN_MESSAGE_MAP(CImageButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CImageButton 消息处理程序



// ReSharper disable once CppInconsistentNaming
// ReSharper disable once IdentifierTypo
// ReSharper disable once CppMemberFunctionMayBeStatic
BOOL CImageButton::OnEraseBkgnd(CDC* pDC)
{
	// 处理背景擦除消息, 实现完全透明
	return TRUE; // CButton::OnEraseBkgnd(pDC);
}


void CImageButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 鼠标按下状态
	m_state = Press;
	Invalidate();

	CButton::OnLButtonDown(nFlags, point);
}


void CImageButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// 鼠标弹起设置为悬停状态, 因为鼠标还在按钮上
	m_state = Hover;
	Invalidate();

	CButton::OnLButtonUp(nFlags, point);
}


void CImageButton::OnMouseMove(UINT nFlags, CPoint point)
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

	CButton::OnMouseMove(nFlags, point);
}


void CImageButton::OnMouseHover(UINT nFlags, CPoint point)
{
	// 鼠标悬停状态
	m_state = Hover;
	Invalidate();

	CButton::OnMouseHover(nFlags, point);
}


void CImageButton::OnMouseLeave()
{
	// 鼠标离开状态
	m_state = Normal;

	// 继续追踪鼠标
	m_bMouseTrack = TRUE;

	Invalidate();

	CButton::OnMouseLeave();
}


void CImageButton::PreSubclassWindow()
{
	// 设置自绘模式
	ModifyStyle(0, BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}


void CImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	auto pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// 按钮是否被禁用
	// if (IsWindowEnabled())
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
		pDC->DrawText(_T("未设置图像"), &lpDrawItemStruct->rcItem, DT_CENTER | DT_VCENTER);
		return;
	}

	const auto nDpi = GetDpi(lpDrawItemStruct->hwndItem);

	// 图片宽度
	const auto nWidth = DPI(image.GetWidth(), nDpi);

	// 图片高度
	const auto nHeight = DPI(image.GetHeight(), nDpi);

	// 创建双缓冲的内存dc
	memDc.CreateCompatibleDC(pDC);
	// 创建内存dc兼容的位图
	bmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
	// 内存dc选择位图
	memDc.SelectObject(&bmp);

	// 绘制到内存dc
	// image.StretchBlt(memDc, 0, 0, nWidth, nHeight, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
	image.Draw(memDc, 0, 0, nWidth, nHeight, 0, 0, image.GetWidth(), image.GetHeight());

	// 使用透明通道贴图
	BLENDFUNCTION bf;
	bf.AlphaFormat = AC_SRC_ALPHA;
	bf.BlendFlags = 0;
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = 255;
	pDC->AlphaBlend(0, 0, nWidth, nHeight, &memDc, 0, 0, nWidth, nHeight, bf);
}

void CImageButton::SetImages(UINT nNormalId, UINT nHoverId, UINT nPressId, UINT nDisableId, LPCTSTR lpszResourceType)
{
	ReleaseImages();
	LoadImageFromResource(m_images[Normal], nNormalId, lpszResourceType);
	LoadImageFromResource(m_images[Hover], nHoverId, lpszResourceType);
	LoadImageFromResource(m_images[Press], nPressId, lpszResourceType);
	nDisableId = nDisableId == 0 ? nNormalId : nDisableId;
	LoadImageFromResource(m_images[Disable], nDisableId, lpszResourceType);
	// AutoSize();
}

void CImageButton::LoadImageFromResource(CImage& img, UINT nId, LPCTSTR lpszResourceType)
{
	const static auto hInstance = AfxGetInstanceHandle();

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

void CImageButton::AutoSize()
{
	auto&& image = m_images[Normal];
	if (image.IsNull())
	{
		return;
	}

	const auto nDpi = GetDpi(m_hWnd);

	SetWindowPos(nullptr, -1, -1, DPI(image.GetWidth(), nDpi), DPI(image.GetHeight(), nDpi), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
}

void CImageButton::ReleaseImages()
{
	for (auto&& image : m_images)
	{
		image.Destroy();
	}
}

