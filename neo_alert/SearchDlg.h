#pragma once
#include "afxcmn.h"


// CSearchDlg ��ȭ �����Դϴ�.

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSearchDlg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void ProcessReturn();
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_list_search;
	afx_msg void OnBnClickedsearchbtn();
	CString m_search_edit;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
