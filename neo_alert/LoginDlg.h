#pragma once
#include "afxwin.h"
#include "afxext.h"


// CLoginDlg 대화 상자입니다.

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoginDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
