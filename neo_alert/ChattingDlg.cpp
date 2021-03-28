// ChattingDlg.cpp : ±¸Çö ÆÄÀÏÀÔ´Ï´Ù.
//

#include "stdafx.h"
#include "neo_alert.h"
#include "ChattingDlg.h"
#include "afxdialogex.h"


// CChattingDlg ´ëÈ­ »óÀÚÀÔ´Ï´Ù.

IMPLEMENT_DYNAMIC(CChattingDlg, CDialogEx)

CChattingDlg::CChattingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChattingDlg::IDD, pParent)
{
	WSADATA temp;
	//¼ÒÄÏ »ç¿ëÀº ÇØ´ç ÄÚµå ÀÌÈÄ ºÎÅÍ °¡´ÉÇÔ****
	WSAStartup(0x0202, &temp);
}

CChattingDlg::~CChattingDlg()
{
	if (mh_socket != INVALID_SOCKET) {
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
	}
	WSACleanup();
	//¼ÒÄÏÀ» Áö¿î´Ù->¾²°í ÀÖ´Â °Ç ´Ù ¾²°í Áö¿ò
}

bool CChattingDlg::Sock_Create()
{
	if (IsValid() == true)
		return true;

	mh_socket = socket(AF_INET, SOCK_STREAM, 0);//¼ÒÄÏ »ý¼º
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
	*p_send_data = 27;//Çì´õ ±¸¼º
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
	// ÀÌ ´ëÈ­ »óÀÚÀÇ ¾ÆÀÌÄÜÀ» ¼³Á¤ÇÕ´Ï´Ù.  ÀÀ¿ë ÇÁ·Î±×·¥ÀÇ ÁÖ Ã¢ÀÌ ´ëÈ­ »óÀÚ°¡ ¾Æ´Ò °æ¿ì¿¡´Â
	//  ÇÁ·¹ÀÓ¿öÅ©°¡ ÀÌ ÀÛ¾÷À» ÀÚµ¿À¸·Î ¼öÇàÇÕ´Ï´Ù.
	if (Sock_Create() == FALSE)
	{
		MessageBox("¼ÒÄÏ »ý¼º ½ÇÆÐ", "¾Ë¸²");
		return FALSE;
	}
	if (Connect("127.0.0.1", 18000) == false)
	{
		Close();
		MessageBox("¼­¹ö ¿¬°á ½ÇÆÐ", "¾Ë¸²");
		return FALSE;
	}

	MessageBox("¼­¹ö ¿¬°á ¼º°ø", "¾Ë¸²");

	AddEventString("¼­¹ö¿¡ ¿¬°áÇß½À´Ï´Ù.");
	WSAAsyncSelect(mh_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);

	mysql_init(&mysql);

	if (!mysql_real_connect(&mysql, DB_ADDRESS, DB_ID, DB_PASS, DB_NAME, 3306, 0, 0))
	{
		AfxMessageBox(_T("ÇöÀç DB°¡ ²¨Á® ÀÖ´Â »óÅÂÀÔ´Ï´Ù.\n°ü¸®ÀÚ¿¡°Ô ¹®ÀÇÇÏ¼¼¿ä."), MB_OK);
	}
	else{
		mysql_query(&mysql, "set names euckr"); //ÇÑ±Û ÀÎ½Ä
		//AfxMessageBox(_T("DB ¿¬°á ¼º°ø"), MB_OK);
		//mysql_close(&mysql);

		CString query;
		query.Format("select * from mfc_chatting order by idx desc");

		if (mysql_query(&mysql, query))
		{
			// Äõ¸® ¿äÃ»
			AfxMessageBox(_T("query°¡ ¿Ç¹Ù¸£Áö ¾Ê½À´Ï´Ù."));
		}
		if ((m_res = mysql_store_result(&mysql)) == NULL)
		{
			// °á°ú¸¦ m_Res¿¡ ÀúÀå
			AfxMessageBox(_T("mysql ¼­¹ö ¿À·ù ÀÔ´Ï´Ù."));
		}

		int i = 0;
		row = mysql_fetch_row(m_res);

		do{
			if (row == NULL){
				AfxMessageBox(_T("°Ë»ö °á°ú°¡ ¾ø½À´Ï´Ù."));
				break;
			}
			AddEventString(_T(row[1]));
			i++;
		} while ((row = mysql_fetch_row(m_res)) != NULL);
	}

	return TRUE;  // Æ÷Ä¿½º¸¦ ÄÁÆ®·Ñ¿¡ ¼³Á¤ÇÏÁö ¾ÊÀ¸¸é TRUE¸¦ ¹ÝÈ¯ÇÕ´Ï´Ù.
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

			//ÆÐÅ¶ ±æÀÌÀÎ 4¹ÙÀÌÆ® skip.
			AddEventString("dd");

			total -= packetSize;
		}
	}
	else if (event == FD_CLOSE)
	{
		Close();
		AddEventString("¦i°Ü³µ½À´Ï´Ù.");
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
		AddEventString("¼­¹ö¿¡ Á¢¼ÓÀ» ½ÇÆÐÇß½À´Ï´Ù.~");
	}
	else {
		m_connect_flag = 2;
		WSAAsyncSelect(mh_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
		AddEventString("¼­¹ö¿¡ Á¢¼ÓÇß½À´Ï´Ù.");
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
						if (retry > 5)break;//Àç½Ãµµ 5¹ø ³ÑÀ¸¸é break
					}
				}
			}
			if (message_id == 1) {//¼­¹ö¿Í ´Ù¸¥ ºÎºÐ-1
				AddEventString(p_body_data);
			}//¿©±â±îÁö-1
			if (p_body_data != NULL)delete[] p_body_data;

			WSAAsyncSelect(wParam, m_hWnd, 25002, FD_READ | FD_CLOSE);
		}
	}
	else {//FD_CLOSE
		//¼­¹ö¿Í ´Ù¸¥ ºÎºÐ-2
		closesocket(mh_socket);//Å¬¶óÀÌ¾ðÆ® ¼ÒÄÏ Å¬·ÎÁî
		mh_socket = INVALID_SOCKET;
		m_connect_flag = 0;//¿¬°á ÇØÁ¦ flag º¯°æ
		AddEventString("¼­¹ö°¡ ¿¬°áÀ» ÇØÁ¦ Çß½À´Ï´Ù.");
		//¿©±â±îÁö-2
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
	//Æ¯Á¤ ´ëÈ­»óÀÚ ÄÁÆ®·Ñ·Î º¸³½´Ù. -> ¹ÝÀü


	mysql_init(&mysql);

	if (!mysql_real_connect(&mysql, DB_ADDRESS, DB_ID, DB_PASS, DB_NAME, 3306, 0, 0))
	{
		AfxMessageBox(_T("ÇöÀç DB°¡ ²¨Á® ÀÖ´Â »óÅÂÀÔ´Ï´Ù.\n°ü¸®ÀÚ¿¡°Ô ¹®ÀÇÇÏ¼¼¿ä."), MB_OK);
	}
	else{
		mysql_query(&mysql, "set names euckr"); //ÇÑ±Û ÀÎ½Ä
		//AfxMessageBox(_T("DB ¿¬°á ¼º°ø"), MB_OK);
		//mysql_close(&mysql);

		CString query;
		query.Format("insert into mfc_chatting (cont,start) values ('" + str + "','" + m_global_id + "');");

		if (mysql_query(&mysql, query))
		{
			// Äõ¸® ¿äÃ»
			AfxMessageBox(_T("query°¡ ¿Ç¹Ù¸£Áö ¾Ê½À´Ï´Ù."));
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


// CChattingDlg ¸Þ½ÃÁö Ã³¸®±âÀÔ´Ï´Ù.
void CChattingDlg::OnLbnSelchangeChatList()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
}

void CChattingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	CRect rect;

	if (m_chat_list.GetSafeHwnd() != NULL) //<-À©µµ¿ì¿¡ ¸ÂÃç »çÀÌÁî Á¶Á¤ÇÒ ÄÁÆ®·Ñ
    {
		m_chat_list.SetWindowPos(NULL, 15, 45, cx - 30, cy - 180, SWP_NOZORDER);
    }
	if (m_chat_edit.GetSafeHwnd() != NULL) //<-À©µµ¿ì Å©±â¿¡ ¯•Ãç À§Ä¡¸¸ Á¶Á¤ÇÒ ÄÁÆ®·Ñ
	{
		//m_chat_edit.GetClientRect(rect);
		m_chat_edit.SetWindowPos(NULL, 15, cy - 100, cx -230, 70, SWP_NOZORDER);
	}
	if (GetDlgItem(IDC_SEND_BTN)->GetSafeHwnd() != NULL) //<-À©µµ¿ì Å©±â¿¡ ¯•Ãç À§Ä¡¸¸ Á¶Á¤ÇÒ ÄÁÆ®·Ñ
	{
		//m_chat_edit.GetClientRect(rect);
		GetDlgItem(IDC_SEND_BTN)->SetWindowPos(NULL, cx-200, cy - 100, 170, 70, SWP_NOZORDER);
	}
	


	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
}
