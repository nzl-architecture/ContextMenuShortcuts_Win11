// DLLMain.cpp : Defines the entry point for the context menu DLL handler.

// See Also
// https://github.com/microsoft/AppModelSamples/Samples/SparsePackages/PhotoStoreContextMenu/dllmain.cpp
//#include "pch.h"
#define WIN32_LEAN_AND_MEAN
// Windows SDK
#include <windows.h>
#include <shlwapi.h>
#include <shobjidl_core.h>
#include <wrl/client.h>
#include <wrl/implements.h>
#include <wrl/module.h>
#include "wil\resource.h"
#include <sstream>
#include <string>
#include <vector>


#define ContextMenuUUID  "@@UUID@@"
#define ContextMenuTitle L"@@TITLE@@"
#define ContextMenuCMD   L"@@CMD@@"
#define ContextMenuIcon  L"@@ICON@@"


#define ContextMenuUUID2  "@@UUID2@@"
#define ContextMenuTitle2 L"@@TITLE2@@"
#define ContextMenuCMD2   L"@@CMD2@@"
#define ContextMenuIcon2  L"@@ICON2@@"

using namespace Microsoft::WRL;
extern "C" IMAGE_DOS_HEADER __ImageBase;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

class ExplorerCommandBase : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IExplorerCommand, IObjectWithSite> {
public:

    virtual const wchar_t* Title() = 0;
    virtual const wchar_t* Icon() = 0;
    virtual const wchar_t* Cmd() = 0;

    virtual const EXPCMDFLAGS Flags() { return ECF_DEFAULT; }
    virtual const EXPCMDSTATE State(_In_opt_ IShellItemArray* selection) { return ECS_ENABLED; }

    // IExplorerCommand
    IFACEMETHODIMP GetTitle(_In_opt_ IShellItemArray* items, _Outptr_result_nullonfailure_ PWSTR* name)
    {
        *name = nullptr;
        auto title = wil::make_cotaskmem_string_nothrow(Title());
        RETURN_IF_NULL_ALLOC(title);
        *name = title.release();
        return S_OK;
    }

    IFACEMETHODIMP GetIcon(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* icon)
    {
        const wchar_t* iconName = Icon();

        if (PathIsRelativeW(iconName))
        {
            wchar_t dllPath[MAX_PATH];
            if (!GetModuleFileNameW((HMODULE)&__ImageBase, dllPath, MAX_PATH)){
                oTaskMemFree(itemName);
                return E_FAIL;}

            PathRemoveFileSpecW(dllPath);

            wchar_t iconPath[MAX_PATH];
            swprintf_s(iconPath, MAX_PATH, L"%s\\%s", dllPath, iconName);

            *icon = _wcsdup(iconPath);
        }
        else
        {
            *icon = _wcsdup(iconName);
        }

        return S_OK;
    }

    IFACEMETHODIMP GetToolTip(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* infoTip)
    {
        *infoTip = nullptr;
        return E_NOTIMPL;
    }
    IFACEMETHODIMP GetCanonicalName(_Out_ GUID* guidCommandName)
    {
        *guidCommandName = GUID_NULL;
        return S_OK;
    }
    IFACEMETHODIMP GetState(_In_opt_ IShellItemArray* selection, _In_ BOOL okToBeSlow, _Out_ EXPCMDSTATE* cmdState)
    {
        *cmdState = State(selection);
        return S_OK;
    }

    IFACEMETHODIMP Invoke(_In_opt_ IShellItemArray* selection, _In_opt_ IBindCtx*) noexcept
    try {
        HWND parent = nullptr;
        if (m_site) {
            ComPtr<IOleWindow> oleWindow;
            RETURN_IF_FAILED(m_site.As(&oleWindow));
            RETURN_IF_FAILED(oleWindow->GetWindow(&parent));
        }

        if (selection) {
            DWORD count;
            RETURN_IF_FAILED(selection->GetCount(&count));

            for (DWORD i = 0; i < count; ++i) {
                ComPtr<IShellItem> psi;
                RETURN_IF_FAILED(selection->GetItemAt(i, &psi));

                PWSTR itemName = nullptr;
                RETURN_IF_FAILED(psi->GetDisplayName(SIGDN_FILESYSPATH, &itemName));

                // get location for dll
                wchar_t dllPath[MAX_PATH];
                if (!GetModuleFileNameW((HMODULE)&__ImageBase, dllPath, MAX_PATH)) {
                    CoTaskMemFree(itemName);
                    return E_FAIL;
                }
                PathRemoveFileSpecW(dllPath);

                wchar_t cmdPath[MAX_PATH];
                swprintf_s(cmdPath, MAX_PATH, L"%s\\%s", dllPath, Cmd());

                std::wstring cmdline = cmdPath;
                size_t pos = cmdline.find(L"%1");
                if (pos != std::wstring::npos) {
                    cmdline.replace(pos, 2, itemName);
                }

                STARTUPINFO si = {};
                si.cb = sizeof(si);
                PROCESS_INFORMATION pi = {};

                CreateProcessW(
                    nullptr, (LPWSTR)cmdline.c_str(),
                    nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);

                if (pi.hProcess) CloseHandle(pi.hProcess);
                if (pi.hThread) CloseHandle(pi.hThread);

                CoTaskMemFree(itemName);
            }
        }

        return S_OK;
    }
    CATCH_RETURN();

    IFACEMETHODIMP GetFlags(_Out_ EXPCMDFLAGS* flags)
    {
        *flags = Flags();
        return S_OK;
    }
    IFACEMETHODIMP EnumSubCommands(_COM_Outptr_ IEnumExplorerCommand** enumCommands)
    {
        *enumCommands = nullptr;
        return E_NOTIMPL;
    }

    // IObjectWithSite
    IFACEMETHODIMP SetSite(_In_ IUnknown* site) noexcept
    {
        m_site = site;
        return S_OK;
    }
    IFACEMETHODIMP GetSite(_In_ REFIID riid, _COM_Outptr_ void** site) noexcept { return m_site.CopyTo(riid, site); }

protected:
    ComPtr<IUnknown> m_site;
};

// ---------------- ContextMenu1 ----------------
class __declspec(uuid(ContextMenuUUID)) ExplorerCommandHandler final : public ExplorerCommandBase {
public:
    const wchar_t* Title() override { return ContextMenuTitle; }
    const wchar_t* Icon()  override { return ContextMenuIcon; }
    const wchar_t* Cmd()   override { return ContextMenuCMD; }
};

// ---------------- ContextMenu2 ----------------
class __declspec(uuid(ContextMenuUUID2)) ExplorerCommandHandler2 final : public ExplorerCommandBase {
public:
    const wchar_t* Title() override { return ContextMenuTitle2; }
    const wchar_t* Icon()  override { return ContextMenuIcon2; }
    const wchar_t* Cmd()   override { return ContextMenuCMD2; }
};

CoCreatableClass(ExplorerCommandHandler)
CoCreatableClassWrlCreatorMapInclude(ExplorerCommandHandler)

CoCreatableClass(ExplorerCommandHandler2)
CoCreatableClassWrlCreatorMapInclude(ExplorerCommandHandler2)

STDAPI DllGetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IActivationFactory** factory)
{
    return Module<ModuleType::InProc>::GetModule().GetActivationFactory(activatableClassId, factory);
}

STDAPI DllCanUnloadNow()
{
    return Module<InProc>::GetModule().GetObjectCount() == 0 ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _COM_Outptr_ void** instance)
{
    return Module<InProc>::GetModule().GetClassObject(rclsid, riid, instance);
}
