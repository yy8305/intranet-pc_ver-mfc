#pragma once
#include "afxwin.h"
#include "afxext.h"


// CLoginDlg ��ȭ �����Դϴ�.

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLoginDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ProcessReturn();
	afx_msg void OnBnClickedButton1();
	CString m_ID;
	CString m_PWD;
	CBitmapButton m_login_btn;
	virtual BOOL OnInitDialog();
	CFont g_editfont;
	CStatic m_logo;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
