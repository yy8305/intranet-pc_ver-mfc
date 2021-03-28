// SubDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "neo_alert.h"
#include "SubDlg.h"
#include "afxdialogex.h"
#include "ChattingDlg.h"
#include "SearchDlg.h"
#include "AllimDlg.h"


// CSubDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSubDlg, CDialogEx)

CSubDlg::CSubDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSubDlg::IDD, pParent)
{

}

CSubDlg::~CSubDlg()
{
}

void CSubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_chat_btn);
	DDX_Control(pDX, IDC_BUTTON2, m_search_btn);
	DDX_Control(pDX, IDC_BUTTON3, m_alim_btn);
}


BEGIN_MESSAGE_MAP(CSubDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CSubDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CSubDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CSubDlg::OnBnClickedButton3)
	ON_COMMAND(ID_32771, &CSubDlg::Onintranet)
END_MESSAGE_MAP()


// CSubDlg 메시지 처리기입니다.


void CSubDlg::OnBnClickedButton1()
{
	CChattingDlg *pDlg = new CChattingDlg;
	pDlg->Create(IDD_CHATTING, CWnd::GetDesktopWindow());
	pDlg->ShowWindow(SW_SHOW);
}


void CSubDlg::OnBnClickedButton2()
{
	CSearchDlg *pDlg = new CSearchDlg;
	pDlg->Create(IDD_SEARCH, CWnd::GetDesktopWindow());
	pDlg->ShowWindow(SW_SHOW);
}


void CSubDlg::OnBnClickedButton3()
{
	CAllimDlg *pDlg = new CAllimDlg;
	pDlg->Create(IDD_ALLIM, CWnd::GetDesktopWindow());
	pDlg->ShowWindow(SW_SHOW);
}


void CSubDlg::Onintranet()
{
	ShellExecute(NULL, _T("open"), _T("http://<intranet url>"), _T(""), _T(""), SW_SHOW);
}


BOOL CSubDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//배경
	SetBackgroundColor(RGB(255, 255, 255));

	//버튼 이미지
	m_chat_btn.LoadBitmaps(IDB_CHATTING_BTN, IDB_CHATTING_BTN_CLICK, IDB_CHATTING_BTN_ON, NULL);
	m_chat_btn.SizeToContent();

	m_search_btn.LoadBitmaps(IDB_SEARCH_BTN, IDB_SEARCH_BTN_CLICK, IDB_SEARCH_BTN_ON, NULL);
	m_search_btn.SizeToContent();

	m_alim_btn.LoadBitmaps(IDB_ALIM_BTN, IDB_ALIM_CLICK, IDB_ALIM_ON, NULL);
	m_alim_btn.SizeToContent();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
