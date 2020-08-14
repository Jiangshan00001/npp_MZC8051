#include <windows.h>
#include <shellapi.h>
#include "text_encode.h"
#include "myexecute.h"



int myexecute(std::string cmd_utf8, std::string param_utf8, int wait_for_finish)
{
    std::wstring cmd_uni=Utf8ToUnicode(cmd_utf8);
    std::wstring param_uni = Utf8ToUnicode(param_utf8);

    SHELLEXECUTEINFO sei;
    memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = TEXT("open");
    sei.lpFile = cmd_uni.c_str();
    sei.lpParameters = param_uni.c_str();
    sei.nShow = SW_SHOWDEFAULT;
    ShellExecuteEx(&sei);

    if(wait_for_finish)
    {
        WaitForSingleObject(sei.hProcess, INFINITE);
    }

    CloseHandle(sei.hProcess);
	return 0;
}
