#pragma once

#include <afxtoolbarimages.h>

#include "afxcmn.h"
#include "afxwin.h"

#include <memory>
//*************************************************************************************************
constexpr auto kUseImageSize = -1;
//*************************************************************************************************
// ������������ ����� �������� ������ �� ��������
enum Alignment
{
    LeftCenter = 0,	// ����� �� ������
    CenterTop,		// �� ������ ������
    CenterCenter,	// �� ������
    CenterBottom,	// �� ������ �����
    RightCenter		// ������ �� ������
};
//*************************************************************************************************
class CIconButton :	public CButton
{
public:	//*****************************************************************************************
    CIconButton();
public:	//*****************************************************************************************
    // loading image for the button, Alignments - flag of image binding relative to the button
    // IconWidth - icon width, if USE_ICON_SIZE then icon width will be used
    // IconHeight - icon height, if USE_ICON_SIZE then icon height will be used
    HICON SetIcon(_In_ UINT uiImageID,	_In_opt_ Alignment Alignment = CenterTop,
                  _In_opt_ int IconWidth = kUseImageSize, _In_opt_ int IconHeight = kUseImageSize);
    HICON SetIcon(_In_ HICON hIcon,		_In_opt_ Alignment Alignment = CenterTop,
                  _In_opt_ int IconWidth = kUseImageSize, _In_opt_ int IconHeight = kUseImageSize);
    // loading image for the button, Alignments - flag of image binding relative to the button
    // ColorMask - mask loaded from BITMAP
    // bUseColorMask - mask usage flag
    // IconWidth - icon width, if USE_ICON_SIZE then icon width will be used
    // IconHeight - icon height, if USE_ICON_SIZE then icon height will be used
    void SetBitmap(_In_ UINT uiBitmapID, _In_opt_ Alignment Alignment = CenterTop,
                   _In_opt_ int BitmapWidth = kUseImageSize, _In_opt_ int BitmapHeight = kUseImageSize,
                   _In_opt_ bool useTransparentColor = false,
                   _In_opt_ COLORREF transparentColor = RGB(0, 0, 0));
    void SetBitmap(_In_ CBitmap& hBitmap, _In_opt_ Alignment Alignment = CenterTop,
                   _In_opt_ int BitmapWidth = kUseImageSize, _In_opt_ int BitmapHeight = kUseImageSize,
                   _In_opt_ bool useTransparentColor = false,
                   _In_opt_ COLORREF transparentColor = RGB(0, 0, 0));
    //*********************************************************************************************
    void SetImageOffset	(_In_ long lOffset);		// ��������� ������� ����������� �� ������ ������
    void SetTextColor	(_In_ COLORREF Color);		// ��������� ����� ������
    void SetBkColor		(_In_ COLORREF BkColor);	// ��������� ���� ��� ������
    // ��������� ����� ������������� ������������ ���� ��� ��������� �������������
    void UseDefaultBkColor(_In_opt_ bool bUseStandart = true);
    //*********************************************************************************************
    void SetTooltip(_In_ CString Tooltip);
protected://***************************************************************************************
    void RepositionItems();		// ������������ ������� ������� ������
protected://***************************************************************************************
    long m_lOffset;				// ������ �� ������ �������� ��� ��������� ������
    CSize m_IconSize;			// ������ ������
    CPoint m_IconPos;			// �������������� �������� ������ ���� ������
    Alignment m_Alignment;		// �������� ������
    CMFCToolBarImages m_image;  // ������
    //*********************************************************************************************
    bool m_bUseCustomBkColor;	// ���� ������������� ����������������� ���� ��� ������
    COLORREF m_TextColor;		// ���� ������
    COLORREF m_BkColor;			// ���� ����
    CString m_ButtonText;		// ����� �� ������
    CRect m_TextRect;			// ������ ������
    bool m_bNeedCalcTextPos;	// ���� ������������� ����������� ������� ������� ��� �����
    //*********************************************************************************************
    std::unique_ptr<CToolTipCtrl> m_ptooltip;	// ������
public:	//*****************************************************************************************
    DECLARE_MESSAGE_MAP()
    virtual void PreSubclassWindow();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};	//*********************************************************************************************