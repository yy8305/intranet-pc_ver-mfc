// LoginDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "neo_alert.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "SubDlg.h"


// CLoginDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
	, m_ID(_T("test"))
	, m_PWD(_T("test"))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ID);
	DDV_MaxChars(pDX, m_ID, 20);
	DDX_Text(pDX, IDC_EDIT2, m_PWD);
	DDV_MaxChars(pDX, m_PWD, 30);
	DDX_Control(pDX, IDC_BUTTON1, m_login_btn);
	DDX_Control(pDX, IDC_LOGO, m_logo);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CLoginDlg::OnBnClickedButton1)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL || pMsg->wParam == VK_RETURN))
	{
		if (pMsg->wParam == VK_RETURN)
		{
			ProcessReturn();
		}
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CLoginDlg::ProcessReturn()
{
	UpdateData(TRUE);

	CWnd *w;
	w = GetFocus();

	if (w->GetDlgCtrlID() == IDC_EDIT1)
	{
		SendMessage(WM_COMMAND, IDC_BUTTON1, 0);
	}
	else if (w->GetDlgCtrlID() == IDC_EDIT2)
	{
		SendMessage(WM_COMMAND, IDC_BUTTON1, 0);
	}
	UpdateData(FALSE);
}

// CLoginDlg 메시지 처리기입니다.


void CLoginDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	/* Connect to SQL */

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
		query.Format("select * from member where id = '" + m_ID + "' and pass = password('" + m_PWD + "')");

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

		if ((row = mysql_fetch_row(m_res)) != NULL){
			AfxMessageBox(_T("로그인 되었습니다."));
			m_nickname = row[3];
			m_global_id = m_ID;
			m_global_pw = m_PWD;
			OnOK();

			CSubDlg S1;
			S1.DoModal();
		}
		else{
			AfxMessageBox(_T("로그인에 실패 되었습니다.\n아이디/비밀번호를 다시 확인 해주세요."));
		}
	}

	UpdateData(FALSE);
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//배경
	SetBackgroundColor(RGB(255, 255, 255));

	//로고 출력
	HBITMAP hbit;
	hbit = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_LOGO));
	m_logo.SetBitmap(hbit);

	//edit control 글자 크기
	g_editfont.CreatePointFont(250,TEXT("굴림"));
	GetDlgItem(IDC_EDIT1)->SetFont(&g_editfont);
	GetDlgItem(IDC_EDIT2)->SetFont(&g_editfont);
	//버튼 이미지
	m_login_btn.LoadBitmaps(IDB_LOGIN_BTN, IDB_LOGIN_BTN_CLICK, IDB_LOGIN_BTN_ON, NULL);
	m_login_btn.SizeToContent();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


HBRUSH CLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	if (pWnd->GetDlgCtrlID() == IDC_EDIT1 || pWnd->GetDlgCtrlID() == IDC_EDIT2){
		hbr = (HBRUSH)::GetSysColorBrush(COLOR_WINDOW);  // 배경 브러쉬
		pDC->SetBkColor(RGB(245, 245, 245)); // 배경색
		pDC->SetTextColor(RGB(0, 0, 0));  // 텍스트 색
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}
