#pragma once


// CImageButton

// ReSharper disable once CppClassCanBeFinal
// ReSharper disable once CppInconsistentNaming
class CImageButton : public CButton  // NOLINT(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)
{
	DECLARE_DYNAMIC(CImageButton)

	CImageButton();
	virtual ~CImageButton();

protected:
	DECLARE_MESSAGE_MAP()
	// ReSharper disable once IdentifierTypo
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	void PreSubclassWindow() override;
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
public:

	/**
	 * \brief 设置按钮图片, 支持[BMP,GIF,JPEG,PNG,TIFF]
	 * \param nNormalId 正常状态图片资源id
	 * \param nHoverId 悬停状态图片资源id
	 * \param nPressId 按下状态图片资源id
	 * \param nDisableId 禁用状态图片资源id, 默认为0, 使用nNormalId
	 * \param lpszResourceType 资源类型, 默认_T("PNG")
	 */
	void SetImages(UINT nNormalId, UINT nHoverId, UINT nPressId, UINT nDisableId = 0, LPCTSTR lpszResourceType = _T("PNG"));

	/**
	 * \brief 自动大小
	 */
	void AutoSize();
	
protected:

	/**
	 * \brief 从资源加载图片
	 * \param img 图片对象
	 * \param nId 图片资源id
	 * \param lpszResourceType 资源类型
	 */
	static void LoadImageFromResource(CImage& img, UINT nId, LPCTSTR lpszResourceType);


	/**
	 * \brief 释放图片资源
	 */
	void ReleaseImages();
	
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


