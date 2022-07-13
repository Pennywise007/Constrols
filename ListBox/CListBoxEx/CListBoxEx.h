#pragma once

// ����� ������ ��� CListBox, ��������� ������������� ����� ��������� � �������� �� � ��������� �����
#include <afxwin.h>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////
// CListBoxEx
class CListBoxEx : public CListBox
{
public:
    CListBoxEx(bool multilineText = true);

    // �������� ������� � �������� �����
    // ���� nIndex == -1 �������� AddString ����� - ����������� �� ���������� �������
    int AddItem(const CString& itemText, COLORREF color, int nIndex = -1);

    DECLARE_MESSAGE_MAP()

protected:
    virtual void PreSubclassWindow() override;
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS) override;
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
    afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    BOOL OnEraseBkgnd(CDC* pDC);

private:
    const bool m_multilineText;
    std::unordered_map<int, COLORREF> m_colorsByLines;
};
