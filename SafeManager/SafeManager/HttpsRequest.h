#pragma once
#include "..\..\include\curl\curl.h"

class CHttpsRequest
{
public:
	CHttpsRequest(void);
	~CHttpsRequest(void);

public:
	static CURLcode Curl_Global_Init();
	static void Curl_global_cleanup();

	//≤‚ ‘”√https://106.37.241.74:8446/smserver/rest/service/test/11/a1dd
	CURLcode HttpsRequest(string strRequest);
};

