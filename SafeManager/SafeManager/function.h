string_t GetCurrentModulePath();

RECT CalTextRect(LPCTSTR lpText, HFONT hFont);

BOOL CreateShortcut(LPCTSTR lpFileName, LPCTSTR lpLnkName);

BOOL ReleaseResource(IN ULONG ResID, IN LPCTSTR lpResType, IN LPTSTR lpPath, IN LPTSTR lpFileName);

HBITMAP CreateQRCode(PCTSTR lpSource, int cx, int cy);

HBITMAP GetIconByFileType(LPCTSTR lpFileType, bool IsDirectory = false);

UINT_PTR CALLBACK OpenFileDlgProc (HWND hWnd, UINT uMsg, WPARAM wParam,  LPARAM lParam);

string_t CreateGUID();

void CheckFileExistAndRename(LPTSTR lpFileName);

BOOL	ModifyFileSizeFormat(LPTSTR	lpText, ULONG	ulLen,	UINT	Size	);

void SplitString(string_t str, string_t pattern, vector<string_t> &vecStrs);

wstring ANSI2Unicode(const string &str);
string Unicode2Ansi(const wstring &str);

wstring UTF8ToUnicode(const char* lpcszString);
string UnicodeToUTF8(const wchar_t* lpwcszWString);

string String_tToString(string_t &str);
wstring String_tToUnicode(string_t &str);

string_t ANSIToString_t(string &str);
string_t UnicodeToString_t(wstring &str);

string_t Utf8ToString_t(string str);
string   String_tToUtf8(string_t str);

INT64 SystemTimeToINT64(const SYSTEMTIME &systime);
string_t INT64TimeToString(INT64 nTime);


string_t GetMaxFreeSpaceDisk();

BOOL IsUsbDev(LPTSTR lpFileName);
BOOL IsExtDev(LPTSTR lpFileName);