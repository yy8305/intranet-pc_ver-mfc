#pragma once
#include "afxwin.h"


// CChattingDlg 대화 상자입니다.

class CChattingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChattingDlg)

public:
	CChattingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CChattingDlg();

	bool Sock_Create();
	void Close();
	bool IsValid() { return mh_socket != INVALID_SOCKET; }
	bool Connect(const char *ip, unsigned short port);
	void AddEventString(const char *ap_string);
	int Send(const char *buf, int size);
	int Recv(char *buf, int size);
	afx_msg LRESULT OnClient(WPARAM wParam, LPARAM lParam);
	void SendFrameData(SOCKET ah_socket, char a_message_id, unsigned short int a_body_size, char* ap_send_data);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHATTING };

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	afx_msg LRESULT On25001(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On25002(WPARAM wParam, LPARAM lParam);

	BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
private:
	CListBox m_chat_list;
	SOCKET mh_socket = INVALID_SOCKET;
	char m_connect_flag = 0;
public:
	afx_msg void OnBnClickedSendBtn();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnLbnSelchangeChatList();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CEdit m_chat_edit;
};
