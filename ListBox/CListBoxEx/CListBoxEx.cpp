#include "Controls/ThemeManagement.h"

#include <afxcontrolbarutil.h>
#include <algorithm>
#include <tuple>

#include "CListBoxEx.h"

BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

CListBoxEx::CListBoxEx(bool multilineText /*= true*/)
    : m_multilineText(multilineText)
{}

int CListBoxEx::AddItem(const CString& itemText, COLORREF color, int nIndex /*= -1*/)
{
    // ������ ���� ��������� �������
    int index;

    if (nIndex == -1)
        index = AddString(itemText);
    else
        index = InsertString(nIndex, itemText);

    m_colorsByLines[index] = color;

    // ������ ��� ���������� ��������� �� ��������� �����������
    Invalidate();

    return index;
}

void CListBoxEx::PreSubclassWindow()
{
    // ��������� ����������� �����, �� ���������� ���������� ����� ModifyStyle(�� ����� ��������)
    if (m_multilineText && (!(CListBox::GetStyle() & LBS_HASSTRINGS | LBS_OWNERDRAWVARIABLE)))
        ASSERT(!"� ������ ������ ���� ���������� ����� LBS_OWNERDRAWVARIABLE � LBS_HASSTRINGS ��� ���������� ������!");

    EnableWindowTheme(GetSafeHwnd(), L"ListBox", L"Explorer", NULL);

    CListBox::PreSubclassWindow();
}

void CListBoxEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    // ����� ��������� ����� ����� LBS_OWNERDRAWVARIABLE
    const int nItem = lpMeasureItemStruct->itemID;
    CString sLabel;
    CRect rcLabel;

    CListBox::GetText( nItem, sLabel );
    CListBox::GetItemRect(nItem, rcLabel);

    // ������������ ������ ��������
    CPaintDC dc(this);
    dc.SelectObject(CListBox::GetFont());

    lpMeasureItemStruct->itemHeight = dc.DrawText(sLabel, -1, rcLabel, DT_WORDBREAK | DT_CALCRECT);
}

void CListBoxEx::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
    // ��� ����� � ������ ������� ����� ������ DrawItem � ���������� ����������������
    if (lpDIS->itemID == -1)
        return;

    const auto getColors = [](HWND hwnd)
    {
        HTHEME hTheme = ::OpenThemeData(hwnd, L"LISTBOX");
        ASSERT(hTheme);

        const COLORREF highLightColor = ::GetThemeSysColor(hTheme, COLOR_HIGHLIGHT);
        const COLORREF highLightTextColor = ::GetThemeSysColor(hTheme, COLOR_HIGHLIGHTTEXT);
        ::CloseThemeData(hTheme);

        return std::make_tuple(highLightColor, highLightTextColor);
    };
    const static auto colors = getColors(m_hWnd);
    const COLORREF highLightColor = std::get<0>(colors);
    const COLORREF highLightTextColor = std::get<1>(colors);

    CDC& dc = *CDC::FromHandle(lpDIS->hDC);

    const COLORREF backColor = [&]()
    {
        const auto it = m_colorsByLines.find(lpDIS->itemID);
        if (it == m_colorsByLines.end())
            return dc.GetBkColor();
        return it->second;
    }();

    CString sLabel;
    GetText(lpDIS->itemID, sLabel);

    // item selected
    if ((lpDIS->itemState & ODS_SELECTED) &&
        (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
    {
        // draw label background
        CBrush labelBrush(highLightColor);
        CRect labelRect = lpDIS->rcItem;
        dc.FillRect(&labelRect,&labelBrush);

        // draw label text
        COLORREF colorTextSave = dc.SetTextColor(highLightTextColor);
        COLORREF colorBkSave = dc.SetBkColor(highLightColor);
        dc.DrawText( sLabel, -1, &lpDIS->rcItem, m_multilineText ? DT_WORDBREAK : 0);

        dc.SetTextColor(colorTextSave);
        dc.SetBkColor(colorBkSave);
    }
    // item brought into box
    else if (lpDIS->itemAction & ODA_DRAWENTIRE)
    {
        CBrush brush(backColor);
        CRect rect = lpDIS->rcItem;
        dc.SetBkColor(backColor);
        dc.FillRect(&rect,&brush);
        dc.DrawText( sLabel, -1, &lpDIS->rcItem, m_multilineText ? DT_WORDBREAK : 0);
    }
    // item deselected
    else if (!(lpDIS->itemState & ODS_SELECTED) &&
        (lpDIS->itemAction & ODA_SELECT))
    {
        CRect rect = lpDIS->rcItem;
        CBrush brush(backColor);
        dc.SetBkColor(backColor);
        dc.FillRect(&rect,&brush);
        dc.DrawText( sLabel, -1, &lpDIS->rcItem, m_multilineText ? DT_WORDBREAK : 0);
    }
}

void CListBoxEx::OnSize(UINT nType, int cx, int cy)
{
    if (m_multilineText)
    {
        MEASUREITEMSTRUCT measureStruct;

        // ��� ������� �������� ������������� ������
        for (int item = 0, count = CListBox::GetCount();
             item < count; ++item)
        {
            // ������� ������� �� �������� �� ������
            measureStruct.itemID = item;
            MeasureItem(&measureStruct);

            // ���� ������ ���������� ���� � ��������������
            if (GetItemHeight(item) != measureStruct.itemHeight)
                SetItemHeight(item, measureStruct.itemHeight);
        }

        // ������ ��������� ������(��� ������ ���������) ����� �� ���������� �����������
        Invalidate();
    }

    CListBox::OnSize(nType, cx, cy);
}

BOOL CListBoxEx::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

LRESULT CListBoxEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == LB_RESETCONTENT)
        m_colorsByLines.clear();

    return CListBox::WindowProc(message, wParam, lParam);
}

void CListBoxEx::OnPaint()
{
    CRect drawRect;
    CListBox::GetClientRect(drawRect);

    if (drawRect.IsRectEmpty())
        return;

    CPaintDC dcPaint(this);
    CMemDC memDC(dcPaint, drawRect);
    CDC& dc = memDC.GetDC();

    dc.SaveDC();
    dc.SetWindowOrg(GetScrollPos(SB_HORZ), 0);

    dc.SelectObject(CListBox::GetFont());

    {
        int minPos = 0, maxPos = 0;
        GetScrollRange(SB_HORZ, &minPos, &maxPos);

        CRect visibleRect = drawRect;
        visibleRect.right = std::max<LONG>(visibleRect.right, maxPos);
        dc.FillSolidRect(&visibleRect, GetBkColor(dc.m_hDC));
    }

    const auto getItemUnderCursor = [&]()
    {
        CPoint cursor;
        ::GetCursorPos(&cursor);
        CListBox::ScreenToClient(&cursor);
        BOOL itemUnderCursor;
        int cursorItem = ItemFromPoint(cursor, itemUnderCursor);
        if (itemUnderCursor == FALSE)
            cursorItem = -1;
        return cursorItem;
    };
    const int itemUnderCursor = getItemUnderCursor();
    const bool focused = CListBox::GetFocus() == this;
    const int countItems = CListBox::GetCount();
    if (countItems == 0)
    {
        dc.RestoreDC(-1);
        return;
    }

    DRAWITEMSTRUCT drawStruct;
    drawStruct.CtlType = ODT_LISTBOX;
    drawStruct.CtlID = CListBox::GetDlgCtrlID();
    drawStruct.hwndItem = m_hWnd;
    drawStruct.hDC = dc.m_hDC;
    drawStruct.itemAction = ODA_DRAWENTIRE;
    drawStruct.itemState = CListBox::IsWindowEnabled() == FALSE ? ODS_DISABLED : 0;
    drawStruct.itemID = CListBox::GetTopIndex();

    do
    {
        drawStruct.itemData = CListBox::GetItemData(drawStruct.itemID);
        CListBox::GetItemRect(drawStruct.itemID, &drawStruct.rcItem);

        const auto prevAction = drawStruct.itemAction;
        const auto prevState = drawStruct.itemState;

        const bool selected = CListBox::GetSel(drawStruct.itemID) > 0;
        if (selected)
        {
            drawStruct.itemAction |= ODA_SELECT | (focused ? ODS_FOCUS : ODS_NOFOCUSRECT);
            drawStruct.itemState |= ODS_SELECTED;
        }
        else
            drawStruct.itemAction |= ODS_DEFAULT;

        if (itemUnderCursor)
            drawStruct.itemState |= ODS_HOTLIGHT;

        DrawItem(&drawStruct);

        drawStruct.itemAction = prevAction;
        drawStruct.itemState = prevState;
        if (++drawStruct.itemID == countItems)
            break;
    } while (drawStruct.rcItem.bottom <= drawRect.bottom);

    dc.RestoreDC(-1);
}
