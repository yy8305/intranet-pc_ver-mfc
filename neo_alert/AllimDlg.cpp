// AllimDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "neo_alert.h"
#include "AllimDlg.h"
#include "afxdialogex.h"


// CAllimDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CAllimDlg, CDialogEx)

CAllimDlg::CAllimDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAllimDlg::IDD, pParent)
{
	WSADATA temp;
	//���� ����� �ش� �ڵ� ���� ���� ������****
	WSAStartup(0x0202, &temp);
}

CAllimDlg::~CAllimDlg()
{
	if (mh_socket != INVALID_SOCKET) {
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
	}
	WSACleanup();
}

bool CAllimDlg::Sock_Create()
{
	if (IsValid() == true)
		return true;

	mh_socket = socket(AF_INET, SOCK_STREAM, 0);//���� ����
	return mh_socket != INVALID_SOCKET;
}

void CAllimDlg::Close()
{
	if (IsValid() == true)
	{
		::closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
	}
}

bool CAllimDlg::Connect(const char *ip, unsigned short port)
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

void CAllimDlg::SendFrameData(SOCKET ah_socket, char a_message_id,
	unsigned short int a_body_size, char* ap_send_data)
{
	char *p_send_data = new char[4 + a_body_size];
	*p_send_data = 27;//��� ����
	*(p_send_data + 1) = a_message_id;
	*(unsigned short *)(p_send_data + 2) = a_body_size;

	memcpy(p_send_data + 4, ap_send_data, a_body_size);
	send(ah_socket, p_send_data, a_body_size + 4, 0);
	delete[] p_send_data;
}


int CAllimDlg::Send(const char *buf, int size)
{
	if (IsValid() == false)
		return SOCKET_ERROR;

	return ::send(mh_socket, buf, size, 0);
}

int CAllimDlg::Recv(char *buf, int size)
{
	if (IsValid() == false)
		return SOCKET_ERROR;

	return ::recv(mh_socket, buf, size, 0);
}

void CAllimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ALIM_LIST, m_alim_list);
}

void CAllimDlg::AddEventString(const char *ap_string) {
	while (m_alim_list.GetCount() > 500) {
		m_alim_list.DeleteString(0);
	}
	int index = m_alim_list.InsertString(-1, ap_string);
	m_alim_list.SetCurSel(index);


	CString str;
	CSize sz;
	int dx = 0;
	CDC *pDC = m_alim_list.GetDC();

	for (int i = 0; i < m_alim_list.GetCount(); i++)
	{
		m_alim_list.GetText(i, str);
		sz = pDC->GetTextExtent(str);

		if (sz.cx > dx)
			dx = sz.cx;
	}
	m_alim_list.ReleaseDC(pDC);

	if (m_alim_list.GetHorizontalExtent() < dx)
	{
		m_alim_list.SetHorizontalExtent(dx);
		ASSERT(m_alim_list.GetHorizontalExtent() == dx);
	}
}

BOOL CAllimDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

LRESULT CAllimDlg::OnClient(WPARAM wParam, LPARAM lParam)
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

			//��Ŷ ������ 4����Ʈ skip.
			AddEventString("dd");

			total -= packetSize;
		}
	}
	else if (event == FD_CLOSE)
	{
		Close();
		AddEventString("�i�ܳ����ϴ�.");
	}

	return 0;
}

afx_msg LRESULT CAllimDlg::On25001(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam)) {
		m_connect_flag = 0;
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
		AddEventString("������ ������ �����߽��ϴ�.~");
	}
	else {
		m_connect_flag = 2;
		WSAAsyncSelect(mh_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
		AddEventString("������ �����߽��ϴ�.");
	}
	return 0;
}

afx_msg LRESULT CAllimDlg::On25002(WPARAM wParam, LPARAM lParam)
{
	CString str;
	if (WSAGETSELECTEVENT(lParam) == FD_READ) {
		WSAAsyncSelect(wParam, m_hWnd, 25002, FD_CLOSE);
		char key;
		recv(wParam, &key, 1, 0);
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
					if (retry > 5)break;//��õ� 5�� ������ break
				}
			}
			//if (message_id == 1) {//������ �ٸ� �κ�-1
			AddEventString(p_body_data);
			MessageBox("�˸��� �����߽��ϴ�. Ȯ�� ���ּ���.", "�˸�", MB_OK | MB_TOPMOST);
			//}//�������-1
			if (p_body_data != NULL)delete[] p_body_data;

			WSAAsyncSelect(wParam, m_hWnd, 25002, FD_READ | FD_CLOSE);
		}
	}
	else {//FD_CLOSE
		//������ �ٸ� �κ�-2
		closesocket(mh_socket);//Ŭ���̾�Ʈ ���� Ŭ����
		mh_socket = INVALID_SOCKET;
		m_connect_flag = 0;//���� ���� flag ����
		AddEventString("������ ������ ���� �߽��ϴ�.");
		//�������-2
	}
	return 0;
}

BEGIN_MESSAGE_MAP(CAllimDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAllimDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAllimDlg::OnBnClickedButton2)
	ON_MESSAGE(25001, &CAllimDlg::On25001)
	ON_MESSAGE(25002, &CAllimDlg::On25002)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CAllimDlg �޽��� ó�����Դϴ�.

void CAllimDlg::OnBnClickedButton1()
{
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	if (Sock_Create() == FALSE)
	{
		MessageBox("���� ���� ����", "�˸�");
		return;
	}
	if (Connect("127.0.0.1", 20000) == false)
	{
		Close();
		MessageBox("���� ���� ����", "�˸�");
		return;
	}

	MessageBox("���� ���� ����", "�˸�");

	AddEventString("������ �����߽��ϴ�.");
	WSAAsyncSelect(mh_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);

	return;
}


void CAllimDlg::OnBnClickedButton2()
{
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	if (mh_socket != INVALID_SOCKET) {
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
	}
	WSACleanup();
	//������ �����->���� �ִ� �� �� ���� ����
	OnOK();
}

void CAllimDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CRect rect;

	if (GetDlgItem(IDC_ALIM_LIST)->GetSafeHwnd() != NULL) //<-�����쿡 ���� ������ ������ ��Ʈ��
	{
		GetDlgItem(IDC_ALIM_LIST)->SetWindowPos(NULL, 15, 15, cx - 30, cy - 250, SWP_NOZORDER);
	}
	if (GetDlgItem(IDC_BUTTON1)->GetSafeHwnd() != NULL) //<-������ ũ�⿡ ���� ��ġ�� ������ ��Ʈ��
	{
		//m_chat_edit.GetClientRect(rect);
		GetDlgItem(IDC_BUTTON1)->SetWindowPos(NULL, cx-750, cy-200, 230, 150, SWP_NOZORDER);
	}
	if (GetDlgItem(IDC_BUTTON2)->GetSafeHwnd() != NULL) //<-������ ũ�⿡ ���� ��ġ�� ������ ��Ʈ��
	{
		//m_chat_edit.GetClientRect(rect);
		GetDlgItem(IDC_BUTTON2)->SetWindowPos(NULL, cx-500, cy-200, 230, 150, SWP_NOZORDER);
	}
}
