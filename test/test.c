#include "../NotificationAPI.h"

#include <stdio.h>

int main()
{
    NotiHandle handle = CreateNotification();

    SetNotiTitle(handle, "Danger!");
    SetNotiContent(handle, "Test");
    SetNotiIcon(handle, "./favicon.ico", 32, 32);
    InitializeNotiCommConnection(handle);
    ShowNoti(handle);

    CloseNotification(handle);

    printf("End\n");
    return 0;
}