// ChattingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "neo_alert.h"
#include "ChattingDlg.h"
#include "afxdialogex.h"


// CChattingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CChattingDlg, CDialogEx)

CChattingDlg::CChattingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChattingDlg::IDD, pParent)
{
	WSADATA temp;
	//소켓 사용은 해당 코드 이후 부터 가능함****
	WSAStartup(0x0202, &temp);
}

CChattingDlg::~CChattingDlg()
{
	if (mh_socket != INVALID_SOCKET) {
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
	}
	WSACleanup();
	//소켓을 지운다->쓰고 있는 건 다 쓰고 지움
}

bool CChattingDlg::Sock_Create()
{
	if (IsValid() == true)
		return true;

	mh_socket = socket(AF_INET, SOCK_STREAM, 0);//소켓 생성
	return mh_socket != INVALID_SOCKET;
}

void CChattingDlg::Close()
{
	if (IsValid() == true)
	{
		::closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
	}
}

bool CChattingDlg::Connect(const char *ip, unsigned short port)
{
	if (IsValid() == false)
		return false;

	sockaddr_in addr{ 0 };

	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	int ret = ::connect(mh_socket, (sockaddr*)&addr, sizeof(addr));

	if (ret == SOCKET_ERROR)
		Close();


	return ret != SOCKET_ERROR;
}

void CChattingDlg::SendFrameData(SOCKET ah_socket, char a_message_id,
	unsigned short int a_body_size, char* ap_send_data)
{
	char *p_send_data = new char[4 + a_body_size];
	*p_send_data = 27;//헤더 구성
	*(p_send_data + 1) = a_message_id;
	*(unsigned short *)(p_send_data + 2) = a_body_size;

	memcpy(p_send_data + 4, ap_send_data, a_body_size);
	send(ah_socket, p_send_data, a_body_size + 4, 0);
	delete[] p_send_data;
}


int CChattingDlg::Send(const char *buf, int size)
{
	if (IsValid() == false)
		return SOCKET_ERROR;

	return ::send(mh_socket, buf, size, 0);
}

int CChattingDlg::Recv(char *buf, int size)
{
	if (IsValid() == false)
		return SOCKET_ERROR;

	return ::recv(mh_socket, buf, size, 0);
}

void CChattingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHAT_LIST, m_chat_list);
	DDX_Control(pDX, IDC_CHAT_EDIT, m_chat_edit);
}

void CChattingDlg::AddEventString(const char *ap_string) {
	while (m_chat_list.GetCount() > 500) {
		m_chat_list.DeleteString(0);
	}
	int index = m_chat_list.InsertString(-1, ap_string);
	m_chat_list.SetCurSel(index);


	CString str;
	CSize sz;
	int dx = 0;
	CDC *pDC = m_chat_list.GetDC();

	for (int i = 0; i < m_chat_list.GetCount(); i++)
	{
		m_chat_list.GetText(i, str);
		sz = pDC->GetTextExtent(str);

		if (sz.cx > dx)
			dx = sz.cx;
	}
	m_chat_list.ReleaseDC(pDC);

	if (m_chat_list.GetHorizontalExtent() < dx)
	{
		m_chat_list.SetHorizontalExtent(dx);
		ASSERT(m_chat_list.GetHorizontalExtent() == dx);
	}
}

BOOL CChattingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	if (Sock_Create() == FALSE)
	{
		MessageBox("소켓 생성 실패", "알림");
		return FALSE;
	}
	if (Connect("127.0.0.1", 18000) == false)
	{
		Close();
		MessageBox("서버 연결 실패", "알림");
		return FALSE;
	}

	MessageBox("서버 연결 성공", "알림");

	AddEventString("서버에 연결했습니다.");
	WSAAsyncSelect(mh_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);

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
		query.Format("select * from mfc_chatting order by idx desc");

		if (mysql_query(&mysql, query))
		{
			// 쿼리 요청
			AfxMessageBox(_T("query가 옳바르지 않습니다."));
		}
		if ((m_res = mysql_store_result(&mysql)) == NULL)
		{
			// 결과를 m_Res에 저장
			AfxMessageBox(_T("mysql 서버 오류 입니다."));
		}

		int i = 0;
		row = mysql_fetch_row(m_res);

		do{
			if (row == NULL){
				AfxMessageBox(_T("검색 결과가 없습니다."));
				break;
			}
			AddEventString(_T(row[1]));
			i++;
		} while ((row = mysql_fetch_row(m_res)) != NULL);
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

LRESULT CChattingDlg::OnClient(WPARAM wParam, LPARAM lParam)
{
	int event = WSAGETSELECTEVENT(lParam);

	if (event == FD_READ)
	{
		wchar_t buf[256];

		int total = 0;
		while (true)
		{
			int ret = Recv(((char *)buf) + total, sizeof(buf) - total);

			if (ret == 0 || ret == SOCKET_ERROR)
				break;

			total += ret;

			if (total < 4)
				continue;

			int packetSize = *((int*)buf);

			if (total < packetSize)
				continue;

			//패킷 길이인 4바이트 skip.
			AddEventString("dd");

			total -= packetSize;
		}
	}
	else if (event == FD_CLOSE)
	{
		Close();
		AddEventString("쫒겨났습니다.");
	}

	return 0;
}

afx_msg LRESULT CChattingDlg::On25001(WPARAM wParam, LPARAM lParam)
{
	AddEventString("test");
	if (WSAGETSELECTERROR(lParam)) {
		m_connect_flag = 0;
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
		AddEventString("서버에 접속을 실패했습니다.~");
	}
	else {
		m_connect_flag = 2;
		WSAAsyncSelect(mh_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
		AddEventString("서버에 접속했습니다.");
	}
	return 0;
}

afx_msg LRESULT CChattingDlg::On25002(WPARAM wParam, LPARAM lParam)
{
	CString str;
	if (WSAGETSELECTEVENT(lParam) == FD_READ) {
		WSAAsyncSelect(wParam, m_hWnd, 25002, FD_CLOSE);
		char key;
		recv(wParam, &key, 1, 0);
		if (key == 27) {
			char message_id;
			recv(wParam, &message_id, 1, 0);
			unsigned short body_size;
			recv(wParam, (char *)&body_size, 2, 0);
			char* p_body_data = NULL;
			if (body_size > 0) {
				p_body_data = new char[body_size];
				int total = 0, x, retry = 0;
				while (total < body_size) {
					x = recv(wParam, p_body_data + total, body_size - total, 0);
					if (x == SOCKET_ERROR)break;
					total = total + x;
					if (total < body_size) {
						Sleep(50);
						retry++;
						if (retry > 5)break;//재시도 5번 넘으면 break
					}
				}
			}
			if (message_id == 1) {//서버와 다른 부분-1
				AddEventString(p_body_data);
			}//여기까지-1
			if (p_body_data != NULL)delete[] p_body_data;

			WSAAsyncSelect(wParam, m_hWnd, 25002, FD_READ | FD_CLOSE);
		}
	}
	else {//FD_CLOSE
		//서버와 다른 부분-2
		closesocket(mh_socket);//클라이언트 소켓 클로즈
		mh_socket = INVALID_SOCKET;
		m_connect_flag = 0;//연결 해제 flag 변경
		AddEventString("서버가 연결을 해제 했습니다.");
		//여기까지-2
	}
	return 0;
}

void CChattingDlg::OnBnClickedSendBtn()
{
	CString str;
	GetDlgItemText(IDC_CHAT_EDIT, str);
	str = m_nickname + " : " + str;
	SendFrameData(mh_socket, 1, str.GetLength() + 1, (char *)(const char *)str);

	SetDlgItemText(IDC_CHAT_LIST, str);
	GotoDlgCtrl(GetDlgItem(IDC_CHAT_EDIT));
	SetDlgItemText(IDC_CHAT_EDIT, "");
	//특정 대화상자 컨트롤로 보낸다. -> 반전


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
		query.Format("insert into mfc_chatting (cont,start) values ('" + str + "','" + m_global_id + "');");

		if (mysql_query(&mysql, query))
		{
			// 쿼리 요청
			AfxMessageBox(_T("query가 옳바르지 않습니다."));
			return;
		}
	}
}

BEGIN_MESSAGE_MAP(CChattingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SEND_BTN, &CChattingDlg::OnBnClickedSendBtn)
	ON_LBN_SELCHANGE(IDC_CHAT_LIST, &CChattingDlg::OnLbnSelchangeChatList)
	ON_MESSAGE(25001, &CChattingDlg::On25001)
	ON_MESSAGE(25002, &CChattingDlg::On25002)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CChattingDlg 메시지 처리기입니다.
void CChattingDlg::OnLbnSelchangeChatList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CChattingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	CRect rect;

	if (m_chat_list.GetSafeHwnd() != NULL) //<-윈도우에 맞춰 사이즈 조정할 컨트롤
    {
		m_chat_list.SetWindowPos(NULL, 15, 45, cx - 30, cy - 180, SWP_NOZORDER);
    }
	if (m_chat_edit.GetSafeHwnd() != NULL) //<-윈도우 크기에 캊춰 위치만 조정할 컨트롤
	{
		//m_chat_edit.GetClientRect(rect);
		m_chat_edit.SetWindowPos(NULL, 15, cy - 100, cx -230, 70, SWP_NOZORDER);
	}
	if (GetDlgItem(IDC_SEND_BTN)->GetSafeHwnd() != NULL) //<-윈도우 크기에 캊춰 위치만 조정할 컨트롤
	{
		//m_chat_edit.GetClientRect(rect);
		GetDlgItem(IDC_SEND_BTN)->SetWindowPos(NULL, cx-200, cy - 100, 170, 70, SWP_NOZORDER);
	}
	


	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
