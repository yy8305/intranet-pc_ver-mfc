// SearchDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "neo_alert.h"
#include "SearchDlg.h"
#include "afxdialogex.h"


// CSearchDlg 대화 상자입니다.

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


// CSearchDlg 메시지 처리기입니다.


BOOL CSearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 리스트 초기화 
	m_list_search.DeleteAllItems();

	// 리스트 스타일 설정
	m_list_search.SetExtendedStyle(LVS_EX_GRIDLINES);

	// 타이틀 삽입
	m_list_search.InsertColumn(0, _T("제목"), LVCFMT_CENTER, 300);
	m_list_search.InsertColumn(1, _T("내용"), LVCFMT_CENTER, 500);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSearchDlg::OnBnClickedsearchbtn()
{
	UpdateData(TRUE);
	/* Connect to SQL */

	// 리스트 초기화 
	m_list_search.DeleteAllItems();

	// mysql의 초기화를 담당한다.
	mysql_init(&mysql);

	if (!mysql_real_connect(&mysql, DB_ADDRESS, DB_ID, DB_PASS, DB_NAME, 3306, 0, 0))
	{
		AfxMessageBox(_T("현재 DB가 꺼져 있는 상태입니다.\n관리자에게 문의하세요."), MB_OK);
	}
	else{
		mysql_query(&mysql, "set names euckr"); //한글 인식
		//AfxMessageBox(_T("DB 연결 성공"), MB_OK);
		//mysql_close(&mysql);

		CString query;
		query.Format("select * from boards where subject like '%%" + m_search_edit + "%%' or message like '%%" + m_search_edit + "%%' order by num desc");

		if (mysql_query(&mysql, query))
		{
			// 쿼리 요청
			AfxMessageBox(_T("query가 옳바르지 않습니다."));
			return;
		}
		if ((m_res = mysql_store_result(&mysql)) == NULL)
		{
			// 결과를 m_Res에 저장
			AfxMessageBox(_T("mysql 서버 오류 입니다."));
			return;
		}

		int i = 0;
		row = mysql_fetch_row(m_res);

		do{
			if (row == NULL){
				AfxMessageBox(_T("검색 결과가 없습니다."));
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

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rect;

	if (GetDlgItem(IDC_LIST_search)->GetSafeHwnd() != NULL) //<-윈도우에 맞춰 사이즈 조정할 컨트롤
	{
		GetDlgItem(IDC_LIST_search)->SetWindowPos(NULL, 15, 80, cx - 30, cy - 100, SWP_NOZORDER);
	}
	if (GetDlgItem(IDC_search_edit)->GetSafeHwnd() != NULL) //<-윈도우 크기에 캊춰 위치만 조정할 컨트롤
	{
		//m_chat_edit.GetClientRect(rect);
		GetDlgItem(IDC_search_edit)->SetWindowPos(NULL, 90, 30, cx - 270, 30, SWP_NOZORDER);
	}
	if (GetDlgItem(IDC_search_btn)->GetSafeHwnd() != NULL) //<-윈도우 크기에 캊춰 위치만 조정할 컨트롤
	{
		//m_chat_edit.GetClientRect(rect);
		GetDlgItem(IDC_search_btn)->SetWindowPos(NULL, cx - 130, 25, 110, 47, SWP_NOZORDER);
	}
}
