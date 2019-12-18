# CImageButton
MFC/WTL CImageButton 支持[`BMP`,`GIF`,`JPEG`,`PNG`,`TIFF`], 支持高DPI

`CImageButton`是一个支持`4种状态`图形显示的按钮控件
+ 正常状态
+ 鼠标放上去的悬停状态
+ 鼠标按下去的状态
+ 按钮被禁用的状态

以上4种状态可分别设置为不同的图片, 支持`PNG`图片透明通道

## 使用方法
### MFC
* 添加`ImageButton.h`和`ImageButton.cpp`到项目中
* `CxxxDlg`类包含头文件 `ImageButton.h`
* 用类向导定义`CImageButton`类型的变量到资源ID
* `OnInitDialog()`虚函数中调用`btn.SetImages()`方法


#### CxxxDlg.h

```cpp
#pragma once

#include "ImageButton.h"


// CMFCDemoDlg 对话框
class CMFCDemoDlg : public CDialogEx
{
    // ...

private:
	CImageButton m_btnTest;
};
```

####  CxxxDlg.cpp

```cpp

BOOL CMFCDemoDlg::OnInitDialog()
{
	
    // ....

	// 设置按钮图片
	m_btnTest.SetImages(IDB_PNG1, IDB_PNG2, IDB_PNG3, IDB_PNG4, _T("PNG"));
	// 开启自动大小, 跟随图片宽高
	m_btnTest.AutoSize();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
```


### WTL

* 添加`ImageButton.h`项目中
* `CMainDlg`类包含头文件 `ImageButton.h`
* 在`CMainDlg`的`BEGIN_MSG_MAP(CMainDlg)`与`END_MSG_MAP()`之间加`REFLECT_NOTIFICATIONS()`用来反射`WM_DRAWITEM`消息, 随后自行加入`COMMAND_HANDLER()`绑定按钮事件
* 添加`CImageButton`类型的成员变量
* `OnInitDialog()`函数中调用`btn.SubclassWindow()`方法与`btn.SetImages()`方法


#### CMainDlg.h

```cpp
#pragma once

#include "ImageButton.h"


class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>, public CMessageFilter, public CIdleHandler
{
    // ...
    	BEGIN_MSG_MAP(CMainDlg)
		COMMAND_HANDLER(IDC_BUTTON_TEST, BN_CLICKED, OnBnClickedButtonTest) // 按钮事件
		REFLECT_NOTIFICATIONS() // 反射 WM_MEASUREITEM 和 WM_DRAWITEM 消息
	END_MSG_MAP()

    // ...
private:
	CImageButton m_btnTest;
};
```

####  CMainDlg.cpp

```cpp

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// ...

    // 子类化控件
	m_btnTest.SubclassWindow(GetDlgItem(IDC_BUTTON_TEST)); 
	// 设置按钮图片
	m_btnTest.SetImages(IDB_PNG1, IDB_PNG2, IDB_PNG3, IDB_PNG4, _T("PNG"));
	// 开启自动大小, 跟随图片宽高
	m_btnTest.AutoSize();

	// ...
	
	return TRUE;
}

// 按钮事件
LRESULT CMainDlg::OnBnClickedButtonTest(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MessageBox(_T("Test"), _T(""), MB_OK | MB_ICONINFORMATION);

	return 0;
}

```


#### 说明

`IDB_PNG1`, `IDB_PNG2`, `IDB_PNG3`, `IDB_PNG4` 均为资源ID, 如何导入资源请去百度上谷歌一下
