
// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// neo_alert.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"


#pragma comment(lib, "WS2_32.lib")

MYSQL_ROW row;
MYSQL_RES *m_res;
MYSQL mysql;

CString m_nickname;
CString m_global_id;
CString m_global_pw;