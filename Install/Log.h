#pragma once

namespace MyLog
{

	#define LOG(FMT , ...) CLog::WriteLog(FMT, __VA_ARGS__)
	class CLog
	{
	public:
		~CLog(void);

		static bool Init();
		static bool UnInit();
		static void  WriteLog(LPTSTR Fmt, ...);
	
	private:
		CLog(void);
		static HANDLE m_hLogFile;

		static CRITICAL_SECTION  m_CriticalSection;
	};

}

