#include "Notification.h"

#include <Windows.h>
#include <string>
#include <memory>
#include <Shobjidl.h>
#include <optional>
#include <filesystem>
#include <stdint.h>

//

static inline std::optional<std::wstring> ToWstring(const std::string &inputStr)
{
    auto size = MultiByteToWideChar(CP_UTF8, 0, inputStr.data(), inputStr.length(), nullptr, 0);

    if (size == 0)
    {
        return std::nullopt;
    }

    auto wchars = std::make_unique<wchar_t[]>(size + 1);
    ZeroMemory(wchars.get(), size + 1);
    auto result = MultiByteToWideChar(CP_UTF8, 0, inputStr.data(), inputStr.length(), wchars.get(), size);

    if (result == 0)
    {
        return std::nullopt;
    }

    return std::wstring(wchars.get());
}

Notification::Notification() {}
Notification::~Notification()
{
    if (userNotificationInstance != nullptr)
    {
        auto userNotification = reinterpret_cast<IUserNotification *>(userNotificationInstance);
        userNotification->Release();
        userNotification = nullptr;
#ifndef __PowerShell
        CoUninitialize();
#endif
    }

    if (iconHandle != nullptr)
    {
        DestroyIcon(reinterpret_cast<HICON>(iconHandle));
        iconHandle = nullptr;
    }

    if (iconResource != nullptr)
    {
        HeapFree(GetProcessHeap(), 0, iconResource);
        iconResource = nullptr;
    }
}

bool Notification::InitializeCom()
{
#ifndef __PowerShell
    auto hResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (hResult != S_OK)
    {
        return false;
    }
#else
    HRESULT hResult = S_FALSE;
#endif

    IUserNotification *userNotification = nullptr;
    hResult = CoCreateInstance(
        CLSID_UserNotification,
        nullptr,
        CLSCTX_ALL,
        IID_IUserNotification,
        reinterpret_cast<void **>(&userNotification)
        //
    );

    if (hResult != S_OK || userNotification == nullptr)
    {
#ifndef __PowerShell
        CoUninitialize();
#endif
        return false;
    }

    userNotificationInstance = reinterpret_cast<void *>(userNotification);

    return true;
}
bool Notification::SetTitle(const std::string &kTitle)
{
    auto converted = ToWstring(kTitle);

    if (!converted.has_value())
    {
        return false;
    }

    m_kTitle = converted.value();

    return true;
}
bool Notification::SetContent(const std::string &kContent)
{
    auto converted = ToWstring(kContent);

    if (!converted.has_value())
    {
        return false;
    }

    m_kContent = converted.value();

    return true;
}
bool Notification::SetIcon(const std::string &kIcoPath, const unsigned int kIcoWeight, const unsigned int kIcoHeight)
{
    auto strAbsPath = std::filesystem::absolute(kIcoPath);

    if (!std::filesystem::exists(strAbsPath))
    {
        return false;
    }

    if (strAbsPath.extension() != ".ico")
    {
        return false;
    }

    if (iconResource != nullptr)
    {
        HeapFree(GetProcessHeap(), 0, iconResource);
        iconResource = nullptr;
    }

    HANDLE hFile = CreateFile(
        strAbsPath.string().data(),
        GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
        //
    );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    LARGE_INTEGER fileSizeStru;
    if (!GetFileSizeEx(hFile, &fileSizeStru))
    {
        CloseHandle(hFile);
        return false;
    }

    DWORD fileSize = static_cast<DWORD>(fileSizeStru.QuadPart);

    auto imgBuffer = (uint8_t *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fileSize);
    if (!ReadFile(hFile, imgBuffer, fileSize, &fileSize, nullptr))
    {
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);

    if (iconHandle != nullptr)
    {
        DestroyIcon(reinterpret_cast<HICON>(iconHandle));
        iconHandle = nullptr;
    }

    auto offsetOfIcon = LookupIconIdFromDirectoryEx(
        reinterpret_cast<PBYTE>(imgBuffer),
        TRUE,
        kIcoWeight,
        kIcoHeight,
        LR_DEFAULTCOLOR
        //
    );

    auto hIcon = CreateIconFromResourceEx(
        reinterpret_cast<PBYTE>(imgBuffer + offsetOfIcon),
        fileSize - offsetOfIcon,
        TRUE,
        0x00030000,
        kIcoWeight,
        kIcoHeight,
        LR_DEFAULTCOLOR
        //
    );

    if (hIcon == nullptr)
    {
        HeapFree(GetProcessHeap(), 0, imgBuffer);
        return false;
    }

    iconHandle = reinterpret_cast<void *>(hIcon);
    iconResource = reinterpret_cast<void *>(imgBuffer);

    return true;
}
bool Notification::SetIconToolTip(const std::string &kTooltip)
{
    auto converted = ToWstring(kTooltip);

    if (!converted.has_value())
    {
        return false;
    }

    m_kToolTip = converted.value();

    return true;
}
void Notification::SetShowInfo(const unsigned int durationMs, const unsigned int intervalMs, const unsigned int notifyCount)
{
    m_kDurationMs = durationMs;
    m_kIntervalMs = intervalMs;
    m_kNotifyCount = notifyCount;
}
bool Notification::Show()
{
    if (userNotificationInstance == nullptr)
    {
        return false;
    }

    auto userNotification = reinterpret_cast<IUserNotification *>(userNotificationInstance);

    auto hResult = userNotification->SetBalloonInfo(
        m_kTitle.data(),
        m_kContent.data(),
        NIIF_USER
        //
    );

    if (hResult != S_OK)
    {
        return false;
    }

    hResult = userNotification->SetBalloonRetry(
        m_kDurationMs,
        m_kIntervalMs,
        m_kNotifyCount
        //
    );

    if (hResult != S_OK)
    {
        return false;
    }

    if (!m_kToolTip.empty() && m_kToolTip.length())
    {
        hResult = userNotification->SetIconInfo(reinterpret_cast<HICON>(iconHandle), m_kToolTip.data());
    }
    else
    {
        hResult = userNotification->SetIconInfo(reinterpret_cast<HICON>(iconHandle), nullptr);
    }

    if (hResult != S_OK)
    {
        return false;
    }

    hResult = userNotification->Show(nullptr, m_kDurationMs);

    if (hResult != S_OK)
    {
        return false;
    }

    return true;
}
