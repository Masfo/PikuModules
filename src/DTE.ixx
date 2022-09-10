//
// Link with: atls.lib ole32.lib oleaut32.lib uuid.lib

module;
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning(push)
#pragma warning(disable : 4471)
#include <atlbase.h>
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8") lcid("0") raw_interfaces_only rename("GetObject", "GetObject2"), rename("SearchPath", "SearchPath2")
#pragma warning(pop)

#include <cstdio>
export module DTE;


namespace DTE
{
    auto GetDTE() -> CComPtr<EnvDTE::_DTE>
    {

        HRESULT result;
        CLSID   clsid;

        // 2022: VisualStudio.DTE.17.0
        // 2019: VisualStudio.DTE.16.0

        result = ::CLSIDFromProgID(L"VisualStudio.DTE", &clsid);
        if (FAILED(result))
            return nullptr;

        CComPtr<IUnknown> punk;
        result = ::GetActiveObject(clsid, nullptr, (IUnknown **)&punk);
        if (FAILED(result))
            return nullptr;

        CComPtr<EnvDTE::_DTE> DTE;
        if (!punk)
            return nullptr;

        punk->QueryInterface(&DTE);

        if (!DTE)
            return nullptr;

        return DTE;
    }

    export bool ExecuteCommand(wchar_t const *action, const wchar_t *action_parameter)
    {
        auto DTE = GetDTE();

        CComBSTR command(action);
        CComBSTR param(action_parameter);
        auto     hr = DTE->ExecuteCommand(command, param);
        if (FAILED(hr))
            return false;

        return true;
    }


    export bool GotoLine(const wchar_t *filename, unsigned int line)
    {
        if (ExecuteCommand(L"File.OpenFile", filename) == false)
            return false;

        wchar_t linetext[64]{};
        int     size = swprintf_s(linetext, 64, L"%d", line);
        if (size <= 0 || ExecuteCommand(L"Edit.Goto", linetext) == false)
            return false;

        return true;
    }

    export bool GotoLine(const char *filename, unsigned int line)
    {
        wchar_t quoted_filename[MAX_PATH]{};
        if (swprintf_s(quoted_filename, MAX_PATH, L"\"%S\"", filename) <= 0)
            return false;

        return GotoLine(quoted_filename, line);
    }
}   // namespace DTE
