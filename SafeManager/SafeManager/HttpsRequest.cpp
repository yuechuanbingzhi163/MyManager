#include "StdAfx.h"
#include "HttpsRequest.h"
#include "function.h"

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int i = 0;

	string str = (char *)ptr;

	return size * nmemb;
}


CHttpsRequest::CHttpsRequest(void)
{
}


CHttpsRequest::~CHttpsRequest(void)
{
}

CURLcode CHttpsRequest::Curl_Global_Init( )
{
	return curl_global_init(CURL_GLOBAL_DEFAULT);
}

void CHttpsRequest::Curl_global_cleanup()
{
	curl_global_cleanup();
}

CURLcode CHttpsRequest::HttpsRequest( string strRequest )
{
	CURL *curl;
	CURLcode retCode = CURLE_OK;
	CURLcode res;

	curl = curl_easy_init();
	if (curl)
	{
		res = curl_easy_setopt(curl, CURLOPT_URL, strRequest.c_str());
		res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);	
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
		{
			wstring strLog = ANSI2Unicode(strRequest);
			LOG(_T("%s«Î«Û ß∞‹!"), strLog.c_str());
			retCode = CURLE_HTTP_RETURNED_ERROR;
		}
		
		curl_easy_cleanup(curl);
	}

	return retCode;
}
