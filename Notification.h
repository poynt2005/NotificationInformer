#ifndef __NOTIFICAION_H__

#ifdef BUILDNOTIAPI
#define NOTIAPI __declspec(dllexport)
#else
#define NOTIAPI __declspec(dllimport)
#endif

#include <string>

class NOTIAPI Notification
{
public:
    Notification();
    ~Notification();

    bool InitializeCom();
    bool SetTitle(const std::string &kTitle);
    bool SetContent(const std::string &kContent);
    void SetShowInfo(const unsigned int durationMs, const unsigned int intervalMs, const unsigned int notifyCount);
    bool SetIcon(const std::string &kIcoPath, const unsigned int kIcoWeight, const unsigned int kIcoHeight);
    bool SetIconToolTip(const std::string &kTooltip);

    bool Show();

private:
    std::wstring m_kTitle = L"";
    std::wstring m_kContent = L"";
    std::wstring m_kToolTip = L"";

    void *userNotificationInstance = nullptr;
    void *iconHandle = nullptr;
    void *iconResource = nullptr;

    unsigned int m_kDurationMs = 1000;
    unsigned int m_kIntervalMs = 1000;
    unsigned int m_kNotifyCount = 0;
};

#endif