# NotificationInformer

## 說明

寫這個東西主要是要讓電腦自動提醒我一些事項  
運行 StartInform.ps1 後，會在右下角的 task bar 顯示 Job Icon 提示使用者已經打開 informer  
目前是使用 powershell 進行執行，不過 Job Icon 右鍵 close 的部分需要點擊兩次才可以關閉

## 編譯

必要軟體: Visual Studio, Mingw64  
必要包: WinSDK 14.29.30133(從 VS installer 安裝)

```
clone poynt2005/NotificationInformer
cd NotificationInformer
mingw32-make
```

## 使用

依照範例寫好 NotiInfo.json，搭配 Task Scheduler 執行 powershell 任務
