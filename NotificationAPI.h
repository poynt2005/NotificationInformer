#ifndef __NOTIFICATION_API_H__

#ifdef BUILDNOTICAPI
#define NOTICAPI __declspec(dllexport)
#else
#define NOTICAPI __declspec(dllimport)
#endif

typedef unsigned long long NotiHandle;

#ifdef __cplusplus
extern "C"
{
#endif

    NOTICAPI NotiHandle CreateNotification();
    NOTICAPI int CloseNotification(const NotiHandle handle);

    NOTICAPI int InitializeNotiCommConnection(const NotiHandle handle);
    NOTICAPI int SetNotiTitle(const NotiHandle handle, const char *kTitle);
    NOTICAPI int SetNotiContent(const NotiHandle handle, const char *kContent);
    NOTICAPI int SetNotiIcon(const NotiHandle handle, const char *kIcoPath, const unsigned int kIcoWeight, const unsigned int kIcoHeight);
    NOTICAPI int SetNotiIconToolTip(const NotiHandle handle, const char *kTooltip);
    NOTICAPI int SetNotiShowInfo(const NotiHandle handle, const unsigned int durationMs, const unsigned int intervalMs, const unsigned int notifyCount);
    NOTICAPI int ShowNoti(const NotiHandle handle);

#ifdef __cplusplus
}
#endif

#endif