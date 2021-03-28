#pragma once
#include "afxext.h"


// CSubDlg 대화 상자입니다.

class CSubDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSubDlg)

public:
	CSubDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSubDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SUB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void Onintranet();
	virtual BOOL OnInitDialog();
	CBitmapButton m_chat_btn;
	CBitmapButton m_search_btn;
	CBitmapButton m_alim_btn;
};
