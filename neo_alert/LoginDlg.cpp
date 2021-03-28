// LoginDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "neo_alert.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "SubDlg.h"


// CLoginDlg ��ȭ �����Դϴ�.

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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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

// CLoginDlg �޽��� ó�����Դϴ�.


void CLoginDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	/* Connect to SQL */

	// mysql�� �ʱ�ȭ�� ����Ѵ�.
	mysql_init(&mysql);

	if (!mysql_real_connect(&mysql, DB_ADDRESS, DB_ID, DB_PASS, DB_NAME, 3306, 0, 0))
	{
		AfxMessageBox(_T("���� DB�� ���� �ִ� �����Դϴ�.\n�����ڿ��� �����ϼ���."), MB_OK);
	}
	else{
		mysql_query(&mysql, "set names euckr"); //�ѱ� �ν�
		//AfxMessageBox(_T("DB ���� ����"), MB_OK);
		//mysql_close(&mysql);

		CString query;
		query.Format("select * from member where id = '" + m_ID + "' and pass = password('" + m_PWD + "')");

		if (mysql_query(&mysql, query))
		{
			// ���� ��û
			AfxMessageBox(_T("query�� �ǹٸ��� �ʽ��ϴ�."));
			return;
		}
		if ((m_res = mysql_store_result(&mysql)) == NULL)
		{
			// ����� m_Res�� ����
			AfxMessageBox(_T("mysql ���� ���� �Դϴ�."));
			return;
		}

		if ((row = mysql_fetch_row(m_res)) != NULL){
			AfxMessageBox(_T("�α��� �Ǿ����ϴ�."));
			m_nickname = row[3];
			m_global_id = m_ID;
			m_global_pw = m_PWD;
			OnOK();

			CSubDlg S1;
			S1.DoModal();
		}
		else{
			AfxMessageBox(_T("�α��ο� ���� �Ǿ����ϴ�.\n���̵�/��й�ȣ�� �ٽ� Ȯ�� ���ּ���."));
		}
	}

	UpdateData(FALSE);
}


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//���
	SetBackgroundColor(RGB(255, 255, 255));

	//�ΰ� ���
	HBITMAP hbit;
	hbit = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_LOGO));
	m_logo.SetBitmap(hbit);

	//edit control ���� ũ��
	g_editfont.CreatePointFont(250,TEXT("����"));
	GetDlgItem(IDC_EDIT1)->SetFont(&g_editfont);
	GetDlgItem(IDC_EDIT2)->SetFont(&g_editfont);
	//��ư �̹���
	m_login_btn.LoadBitmaps(IDB_LOGIN_BTN, IDB_LOGIN_BTN_CLICK, IDB_LOGIN_BTN_ON, NULL);
	m_login_btn.SizeToContent();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


HBRUSH CLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.

	if (pWnd->GetDlgCtrlID() == IDC_EDIT1 || pWnd->GetDlgCtrlID() == IDC_EDIT2){
		hbr = (HBRUSH)::GetSysColorBrush(COLOR_WINDOW);  // ��� �귯��
		pDC->SetBkColor(RGB(245, 245, 245)); // ����
		pDC->SetTextColor(RGB(0, 0, 0));  // �ؽ�Ʈ ��
	}

	// TODO:  �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�մϴ�.
	return hbr;
}
