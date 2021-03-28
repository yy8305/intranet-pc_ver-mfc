// SearchDlg.cpp : ±¸Çö ÆÄÀÏÀÔ´Ï´Ù.
//

#include "stdafx.h"
#include "neo_alert.h"
#include "SearchDlg.h"
#include "afxdialogex.h"


// CSearchDlg ´ëÈ­ »óÀÚÀÔ´Ï´Ù.

IMPLEMENT_DYNAMIC(CSearchDlg, CDialogEx)

CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchDlg::IDD, pParent)
	, m_search_edit(_T(""))
{
}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_search, m_list_search);
	DDX_Text(pDX, IDC_search_edit, m_search_edit);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialogEx)
	ON_BN_CLICKED(IDC_search_btn, &CSearchDlg::OnBnClickedsearchbtn)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSearchDlg ¸Þ½ÃÁö Ã³¸®±âÀÔ´Ï´Ù.


BOOL CSearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ¸®½ºÆ® ÃÊ±âÈ­ 
	m_list_search.DeleteAllItems();

	// ¸®½ºÆ® ½ºÅ¸ÀÏ ¼³Á¤
	m_list_search.SetExtendedStyle(LVS_EX_GRIDLINES);

	// Å¸ÀÌÆ² »ðÀÔ
	m_list_search.InsertColumn(0, _T("Á¦¸ñ"), LVCFMT_CENTER, 300);
	m_list_search.InsertColumn(1, _T("³»¿ë"), LVCFMT_CENTER, 500);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ¿¹¿Ü: OCX ¼Ó¼º ÆäÀÌÁö´Â FALSE¸¦ ¹ÝÈ¯ÇØ¾ß ÇÕ´Ï´Ù.
}


void CSearchDlg::OnBnClickedsearchbtn()
{
	UpdateData(TRUE);
	/* Connect to SQL */

	// ¸®½ºÆ® ÃÊ±âÈ­ 
	m_list_search.DeleteAllItems();

	// mysqlÀÇ ÃÊ±âÈ­¸¦ ´ã´çÇÑ´Ù.
	mysql_init(&mysql);

	if (!mysql_real_connect(&mysql, DB_ADDRESS, DB_ID, DB_PASS, DB_NAME, 3306, 0, 0))
	{
		AfxMessageBox(_T("ÇöÀç DB°¡ ²¨Á® ÀÖ´Â »óÅÂÀÔ´Ï´Ù.\n°ü¸®ÀÚ¿¡°Ô ¹®ÀÇÇÏ¼¼¿ä."), MB_OK);
	}
	else{
		mysql_query(&mysql, "set names euckr"); //ÇÑ±Û ÀÎ½Ä
		//AfxMessageBox(_T("DB ¿¬°á ¼º°ø"), MB_OK);
		//mysql_close(&mysql);

		CString query;
		query.Format("select * from boards where subject like '%%" + m_search_edit + "%%' or message like '%%" + m_search_edit + "%%' order by num desc");

		if (mysql_query(&mysql, query))
		{
			// Äõ¸® ¿äÃ»
			AfxMessageBox(_T("query°¡ ¿Ç¹Ù¸£Áö ¾Ê½À´Ï´Ù."));
			return;
		}
		if ((m_res = mysql_store_result(&mysql)) == NULL)
		{
			// °á°ú¸¦ m_Res¿¡ ÀúÀå
			AfxMessageBox(_T("mysql ¼­¹ö ¿À·ù ÀÔ´Ï´Ù."));
			return;
		}

		int i = 0;
		row = mysql_fetch_row(m_res);

		do{
			if (row == NULL){
				AfxMessageBox(_T("°Ë»ö °á°ú°¡ ¾ø½À´Ï´Ù."));
				break;
			}
			m_list_search.InsertItem(i, _T(row[2]));
			m_list_search.SetItem(i, 1, LVIF_TEXT, _T(row[3]), 0, 0, 0, NULL);
			i++;
		} while ((row = mysql_fetch_row(m_res)) != NULL);
	}

	UpdateData(FALSE);
}

BOOL CSearchDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL || pMsg->wParam == VK_RETURN))
	{
		if (pMsg->wParam == VK_RETURN)
		{
			ProcessReturn();
		}
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}



void CSearchDlg::ProcessReturn()
{
	UpdateData(TRUE);

	CWnd *w;
	w = GetFocus();

	if (w->GetDlgCtrlID() == IDC_search_edit)
	{
		SendMessage(WM_COMMAND, IDC_search_btn, 0);
	}
	UpdateData(FALSE);
}

void CSearchDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	CRect rect;

	if (GetDlgItem(IDC_LIST_search)->GetSafeHwnd() != NULL) //<-À©µµ¿ì¿¡ ¸ÂÃç »çÀÌÁî Á¶Á¤ÇÒ ÄÁÆ®·Ñ
	{
		GetDlgItem(IDC_LIST_search)->SetWindowPos(NULL, 15, 80, cx - 30, cy - 100, SWP_NOZORDER);
	}
	if (GetDlgItem(IDC_search_edit)->GetSafeHwnd() != NULL) //<-À©µµ¿ì Å©±â¿¡ ¯•Ãç À§Ä¡¸¸ Á¶Á¤ÇÒ ÄÁÆ®·Ñ
	{
		//m_chat_edit.GetClientRect(rect);
		GetDlgItem(IDC_search_edit)->SetWindowPos(NULL, 90, 30, cx - 270, 30, SWP_NOZORDER);
	}
	if (GetDlgItem(IDC_search_btn)->GetSafeHwnd() != NULL) //<-À©µµ¿ì Å©±â¿¡ ¯•Ãç À§Ä¡¸¸ Á¶Á¤ÇÒ ÄÁÆ®·Ñ
	{
		//m_chat_edit.GetClientRect(rect);
		GetDlgItem(IDC_search_btn)->SetWindowPos(NULL, cx - 130, 25, 110, 47, SWP_NOZORDER);
	}
}
