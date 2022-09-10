//
// Link with: atls.lib ole32.lib oleaut32.lib uuid.lib

module;
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#pragma warning(suppress : 4471)
#include <atlbase.h>

#include <cstdio>
export module DTE;

namespace DTE
{
#pragma warning(push)
#pragma warning(disable : 4471)
    struct __declspec(uuid("04a72314-32e9-48e2-9b87-a63603454f3e")) _DTE : IDispatch
    {
        virtual HRESULT __stdcall get_Name(BSTR *)                                         = 0;
        virtual HRESULT __stdcall get_FileName(BSTR *)                                     = 0;
        virtual HRESULT __stdcall get_Version(BSTR *)                                      = 0;
        virtual HRESULT __stdcall get_CommandBars(IDispatch **)                            = 0;
        virtual HRESULT __stdcall get_Windows(struct Windows **)                           = 0;
        virtual HRESULT __stdcall get_Events(struct Events **)                             = 0;
        virtual HRESULT __stdcall get_AddIns(struct AddIns **)                             = 0;
        virtual HRESULT __stdcall get_MainWindow(struct Window **)                         = 0;
        virtual HRESULT __stdcall get_ActiveWindow(struct Window **)                       = 0;
        virtual HRESULT __stdcall Quit()                                                   = 0;
        virtual HRESULT __stdcall get_DisplayMode(enum vsDisplay *)                        = 0;
        virtual HRESULT __stdcall put_DisplayMode(enum vsDisplay)                          = 0;
        virtual HRESULT __stdcall get_Solution(struct _Solution **)                        = 0;
        virtual HRESULT __stdcall get_Commands(struct Commands **)                         = 0;
        virtual HRESULT __stdcall GetObject2(BSTR Name, IDispatch **)                      = 0;
        virtual HRESULT __stdcall get_Properties(BSTR, BSTR, struct Properties **)         = 0;
        virtual HRESULT __stdcall get_SelectedItems(struct SelectedItems **)               = 0;
        virtual HRESULT __stdcall get_CommandLineArguments(BSTR *)                         = 0;
        virtual HRESULT __stdcall OpenFile(BSTR ViewKind, BSTR, struct Window **)          = 0;
        virtual HRESULT __stdcall get_IsOpenFile(BSTR, BSTR, VARIANT_BOOL *)               = 0;
        virtual HRESULT __stdcall get_DTE(struct _DTE **)                                  = 0;
        virtual HRESULT __stdcall get_LocaleID(long *)                                     = 0;
        virtual HRESULT __stdcall get_WindowConfigurations(struct WindowConfigurations **) = 0;
        virtual HRESULT __stdcall get_Documents(struct Documents **)                       = 0;
        virtual HRESULT __stdcall get_ActiveDocument(struct Document **)                   = 0;
        virtual HRESULT __stdcall ExecuteCommand(BSTR, BSTR)                               = 0;
        virtual HRESULT __stdcall get_Globals(struct Globals **)                           = 0;
        virtual HRESULT __stdcall get_StatusBar(struct StatusBar **)                       = 0;
        virtual HRESULT __stdcall get_FullName(BSTR *)                                     = 0;
        virtual HRESULT __stdcall get_UserControl(VARIANT_BOOL *)                          = 0;
        virtual HRESULT __stdcall put_UserControl(VARIANT_BOOL)                            = 0;
        virtual HRESULT __stdcall get_ObjectExtenders(struct ObjectExtenders **)           = 0;
        virtual HRESULT __stdcall get_Find(struct Find **)                                 = 0;
        virtual HRESULT __stdcall get_Mode(enum vsIDEMode *)                               = 0;
        virtual HRESULT __stdcall LaunchWizard(BSTR, SAFEARRAY **, enum wizardResult *)    = 0;
        virtual HRESULT __stdcall get_ItemOperations(struct ItemOperations **)             = 0;
        virtual HRESULT __stdcall get_UndoContext(struct UndoContext **)                   = 0;
        virtual HRESULT __stdcall get_Macros(struct Macros **)                             = 0;
        virtual HRESULT __stdcall get_ActiveSolutionProjects(VARIANT *)                    = 0;
        virtual HRESULT __stdcall get_MacrosIDE(struct _DTE **)                            = 0;
        virtual HRESULT __stdcall get_RegistryRoot(BSTR *)                                 = 0;
        virtual HRESULT __stdcall get_Application(struct _DTE **pVal)                      = 0;
        virtual HRESULT __stdcall get_ContextAttributes(struct ContextAttributes **)       = 0;
        virtual HRESULT __stdcall get_SourceControl(struct SourceControl **)               = 0;
        virtual HRESULT __stdcall get_SuppressUI(VARIANT_BOOL *)                           = 0;
        virtual HRESULT __stdcall put_SuppressUI(VARIANT_BOOL)                             = 0;
        virtual HRESULT __stdcall get_Debugger(struct Debugger **)                         = 0;
        virtual HRESULT __stdcall SatelliteDllPath(BSTR, BSTR, BSTR *)                     = 0;
        virtual HRESULT __stdcall get_Edition(BSTR *)                                      = 0;
    };
#pragma warning(pop)


    auto GetDTE() -> CComPtr<_DTE>
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

        CComPtr<_DTE> DTE;
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
