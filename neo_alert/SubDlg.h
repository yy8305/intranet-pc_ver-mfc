#pragma once
#include "afxext.h"


// CSubDlg ��ȭ �����Դϴ�.

class CSubDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSubDlg)

public:
	CSubDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSubDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SUB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
