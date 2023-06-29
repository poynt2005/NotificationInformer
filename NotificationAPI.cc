#include "NotificationAPI.h"
#include "Notification.h"
#include <unordered_map>
#include <memory>

std::unordered_map<NotiHandle, std::unique_ptr<Notification>> notiInstanceMapping;

NotiHandle CreateNotification()
{
    auto noti = std::make_unique<Notification>();
    auto handle = reinterpret_cast<NotiHandle>(noti.get());

    notiInstanceMapping.insert(std::pair<NotiHandle, std::unique_ptr<Notification>>(handle, std::move(noti)));

    return handle;
}
int CloseNotification(const NotiHandle handle)
{
    return notiInstanceMapping.erase(handle);
}

int InitializeNotiCommConnection(const NotiHandle handle)
{
    if (notiInstanceMapping.count(handle))
    {
        return static_cast<int>(notiInstanceMapping[handle]->InitializeCom());
    }
    return 0;
}
int SetNotiTitle(const NotiHandle handle, const char *kTitle)
{
    if (notiInstanceMapping.count(handle))
    {
        return static_cast<int>(notiInstanceMapping[handle]->SetTitle(kTitle));
    }
    return 0;
}
int SetNotiContent(const NotiHandle handle, const char *kContent)
{
    if (notiInstanceMapping.count(handle))
    {
        return static_cast<int>(notiInstanceMapping[handle]->SetContent(kContent));
    }
    return 0;
}
int SetNotiIcon(const NotiHandle handle, const char *kIcoPath, const unsigned int kIcoWeight, const unsigned int kIcoHeight)
{
    if (notiInstanceMapping.count(handle))
    {
        return static_cast<int>(notiInstanceMapping[handle]->SetIcon(kIcoPath, kIcoWeight, kIcoHeight));
    }
    return 0;
}
int SetNotiIconToolTip(const NotiHandle handle, const char *kTooltip)
{
    if (notiInstanceMapping.count(handle))
    {
        return static_cast<int>(notiInstanceMapping[handle]->SetIconToolTip(kTooltip));
    }
    return 0;
}
int SetNotiShowInfo(const NotiHandle handle, const unsigned int durationMs, const unsigned int intervalMs, const unsigned int notifyCount)
{
    if (notiInstanceMapping.count(handle))
    {
        notiInstanceMapping[handle]->SetShowInfo(durationMs, intervalMs, notifyCount);
        return 1;
    }
    return 0;
}
int ShowNoti(const NotiHandle handle)
{
    if (notiInstanceMapping.count(handle))
    {
        return static_cast<int>(notiInstanceMapping[handle]->Show());
    }
    return 0;
}
