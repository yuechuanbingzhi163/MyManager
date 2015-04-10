// dllmain.h : 模块类的声明。

class CShellExModule : public ATL::CAtlDllModuleT< CShellExModule >
{
public :
	DECLARE_LIBID(LIBID_ShellExLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SHELLEX, "{DEDC1E10-3153-4434-B4DC-D53D44617B63}")
};

extern class CShellExModule _AtlModule;
