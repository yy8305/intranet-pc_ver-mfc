#pragma once
#include "afxcmn.h"


// CSearchDlg 대화 상자입니다.

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSearchDlg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void ProcessReturn();
// 대화 상자 데이터입니다.
	enum { IDD = IDD_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_list_search;
	afx_msg void OnBnClickedsearchbtn();
	CString m_search_edit;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
