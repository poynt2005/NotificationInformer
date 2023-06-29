Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

$CsCode = @'

using System;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Threading;

namespace Utils {
    public delegate void CloseExecute();

    public class Notification {
        [DllImport("NotificationAPI.dll", CharSet = CharSet.Ansi)]
        private static extern UInt64 CreateNotification();

        [DllImport("NotificationAPI.dll", CharSet = CharSet.Ansi)]
        private static extern int CloseNotification(UInt64 handle);

        [DllImport("NotificationAPI.dll", CharSet = CharSet.Ansi)]
        private static extern bool InitializeNotiCommConnection(UInt64 handle);

        [DllImport("NotificationAPI.dll", CharSet = CharSet.Ansi)]
        private static extern bool SetNotiTitle(UInt64 handle, IntPtr kTitle);

        [DllImport("NotificationAPI.dll", CharSet = CharSet.Ansi)]
        private static extern bool SetNotiContent(UInt64 handle, IntPtr kContent);

        [DllImport("NotificationAPI.dll", CharSet = CharSet.Ansi)]
        private static extern bool SetNotiIcon(UInt64 handle, IntPtr kIcoPath, UInt32 kIcoWeight, UInt32 kIcoHeight);

        [DllImport("NotificationAPI.dll", CharSet = CharSet.Ansi)]
        private static extern bool SetNotiIconToolTip(UInt64 handle, IntPtr kTooltip);

        [DllImport("NotificationAPI.dll", CharSet = CharSet.Ansi)]
        private static extern bool SetNotiShowInfo(UInt64 handle, UInt32 durationMs, UInt32 intervalMs, UInt32 notifyCount);

        [DllImport("NotificationAPI.dll", CharSet = CharSet.Ansi)]
        private static extern bool ShowNoti(UInt64 handle);

        private UInt64 handle = 0;

        public Notification(){
            handle = CreateNotification();
        }

        public void FreeNotification(){
            CloseNotification(handle);
        }

        public bool SetTitle(String title) {
            var ptr = Marshal.StringToHGlobalAnsi(title);
            bool isSet = SetNotiTitle(handle, ptr);
            Marshal.FreeHGlobal(ptr);
            return isSet;
        }

        public bool SetContent(String content) {
            var ptr = Marshal.StringToHGlobalAnsi(content);
            bool isSet = SetNotiContent(handle, ptr);
            Marshal.FreeHGlobal(ptr);
            return isSet;
        }

        public bool SetIcon(String iconPath, UInt32 width, UInt32 height) {
            var ptr = Marshal.StringToHGlobalAnsi(iconPath);
            bool isSet = SetNotiIcon(handle, ptr, width, height);
            Marshal.FreeHGlobal(ptr);
            return isSet;
        }

        public bool SetIconToolTip(String toolTip) {
            var ptr = Marshal.StringToHGlobalAnsi(toolTip);
            bool isSet = SetNotiIconToolTip(handle, ptr);
            Marshal.FreeHGlobal(ptr);
            return isSet;
        }

        public bool SetShowInfo(UInt32 durationMs, UInt32 intervalMs, UInt32 notifyCount) {
            return SetNotiShowInfo(handle, durationMs, intervalMs, notifyCount);
        }

        public bool Initialize(){
            return InitializeNotiCommConnection(handle);
        }

        public bool Show(){
            return ShowNoti(handle);
        }

    }


    public class Utils {
        [DllImport("Kernel32.dll")]
        private static extern IntPtr GetConsoleWindow();

        [DllImport("user32.dll")]
        private static extern bool ShowWindow(IntPtr hWnd, Int32 nCmdShow);

        private static NotifyIcon iconInstance = null;
        private static CloseExecute closeExecuteDelegate = null;

        public static void CreateJobIcon(String icoPath){
            if(iconInstance != null) {
                return;
            }

            var contextMenu = new ContextMenu();

            contextMenu.MenuItems.Add("Close", new EventHandler(delegate (Object o, EventArgs a) {
                if(closeExecuteDelegate != null) {
                    closeExecuteDelegate();
                }
            }));

            iconInstance = new NotifyIcon();
            iconInstance.Icon = new Icon(icoPath);
            iconInstance.Visible = true;
            iconInstance.ContextMenu = contextMenu;
        }

        public static void CloseJobIcon() {
            iconInstance.Visible = false;
        }

        public static void SetCloseExecuteDelegate(CloseExecute inputDelegate) {
            closeExecuteDelegate = inputDelegate;
        }

        public static void HideConsole() {
            var hWnd = GetConsoleWindow();
            ShowWindow(hWnd, 0);
        }
    }
}

'@

Add-Type -TypeDefinition $CsCode -Language CSharp -ReferencedAssemblies System.Windows.Forms, System.Drawing

[Utils.Utils]::HideConsole()

$jsonStr = [System.IO.File]::ReadAllText("NotiInfo.json")
$notiInfo = ConvertFrom-Json -InputObject $jsonStr

$notier = [Utils.Notification]::new()

if (!$notier.SetTitle($notiInfo.title)) {
    
    Write-Host "Set Title Failed"
    $notier.FreeNotification()
    exit
}

if (!$notier.SetContent($notiInfo.content)) {
    Write-Host "Set Content Failed"
    $notier.FreeNotification()
    exit
}

if (!$notier.SetIcon($notiInfo.notifyIcon.file, $notiInfo.notifyIcon.width, $notiInfo.notifyIcon.height)) {
    Write-Host "Set icon Failed"
    $notier.FreeNotification()
    exit
}

if (!$notier.Initialize()) {
    Write-Host "Cannot initialize COM connection"
    $notier.FreeNotification()
    exit
}

$global:isJobRunning = $true
[Utils.Utils]::CreateJobIcon($notiInfo.jobIcon)
[Utils.Utils]::SetCloseExecuteDelegate(
    [Utils.CloseExecute] {
        $global:isJobRunning = $false
        [Utils.Utils]::CloseJobIcon()
    }
)


while ($global:isJobRunning) {
    $notier.Show()
    Start-Sleep -seconds $notiInfo.interval
}



