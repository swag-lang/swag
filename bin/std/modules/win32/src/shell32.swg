#global export
#foreignlib("shell32")
using Swag

alias SHSTDAPI = *void
alias HRESULT  = *void

const S_OK = cast(HRESULT) null

const CSIDL_DESKTOP                 = 0x0000             // <desktop>
const CSIDL_INTERNET                = 0x0001             // Internet Explorer (icon on desktop)
const CSIDL_PROGRAMS                = 0x0002             // Start Menu/Programs
const CSIDL_CONTROLS                = 0x0003             // My Computer/Control Panel
const CSIDL_PRINTERS                = 0x0004             // My Computer/Printers
const CSIDL_PERSONAL                = 0x0005             // My Documents
const CSIDL_FAVORITES               = 0x0006             // <user name>/Favorites
const CSIDL_STARTUP                 = 0x0007             // Start Menu/Programs/Startup
const CSIDL_RECENT                  = 0x0008             // <user name>/Recent
const CSIDL_SENDTO                  = 0x0009             // <user name>/SendTo
const CSIDL_BITBUCKET               = 0x000a             // <desktop>/Recycle Bin
const CSIDL_STARTMENU               = 0x000b             // <user name>/Start Menu
const CSIDL_MYDOCUMENTS             = CSIDL_PERSONAL     //  Personal was just a silly name for My Documents
const CSIDL_MYMUSIC                 = 0x000d             // "My Music" folder
const CSIDL_MYVIDEO                 = 0x000e             // "My Videos" folder
const CSIDL_DESKTOPDIRECTORY        = 0x0010             // <user name>/Desktop
const CSIDL_DRIVES                  = 0x0011             // My Computer
const CSIDL_NETWORK                 = 0x0012             // Network Neighborhood (My Network Places)
const CSIDL_NETHOOD                 = 0x0013             // <user name>/nethood
const CSIDL_FONTS                   = 0x0014             // windows/fonts
const CSIDL_TEMPLATES               = 0x0015
const CSIDL_COMMON_STARTMENU        = 0x0016             // All Users/Start Menu
const CSIDL_COMMON_PROGRAMS         = 0X0017             // All Users/Start Menu/Programs
const CSIDL_COMMON_STARTUP          = 0x0018             // All Users/Startup
const CSIDL_COMMON_DESKTOPDIRECTORY = 0x0019             // All Users/Desktop
const CSIDL_APPDATA                 = 0x001a             // <user name>/Application Data
const CSIDL_PRINTHOOD               = 0x001b             // <user name>/PrintHood
const CSIDL_LOCAL_APPDATA           = 0x001c             // <user name>/Local Settings/Applicaiton Data (non roaming)
const CSIDL_ALTSTARTUP              = 0x001d             // non localized startup
const CSIDL_COMMON_ALTSTARTUP       = 0x001e             // non localized common startup
const CSIDL_COMMON_FAVORITES        = 0x001f
const CSIDL_INTERNET_CACHE          = 0x0020
const CSIDL_COOKIES                 = 0x0021
const CSIDL_HISTORY                 = 0x0022
const CSIDL_COMMON_APPDATA          = 0x0023             // All Users/Application Data
const CSIDL_WINDOWS                 = 0x0024             // GetWindowsDirectory()
const CSIDL_SYSTEM                  = 0x0025             // GetSystemDirectory()
const CSIDL_PROGRAM_FILES           = 0x0026             // C:/Program Files
const CSIDL_MYPICTURES              = 0x0027             // C:/Program Files/My Pictures
const CSIDL_PROFILE                 = 0x0028             // USERPROFILE
const CSIDL_SYSTEMX86               = 0x0029             // x86 system directory on RISC
const CSIDL_PROGRAM_FILESX86        = 0x002a             // x86 C:/Program Files on RISC
const CSIDL_PROGRAM_FILES_COMMON    = 0x002b             // C:/Program Files/Common
const CSIDL_PROGRAM_FILES_COMMONX86 = 0x002c             // x86 Program Files/Common on RISC
const CSIDL_COMMON_TEMPLATES        = 0x002d             // All Users/Templates
const CSIDL_COMMON_DOCUMENTS        = 0x002e             // All Users/Documents
const CSIDL_COMMON_ADMINTOOLS       = 0x002f             // All Users/Start Menu/Programs/Administrative Tools
const CSIDL_ADMINTOOLS              = 0x0030             // <user name>/Start Menu/Programs/Administrative Tools
const CSIDL_CONNECTIONS             = 0x0031             // Network and Dial-up Connections
const CSIDL_COMMON_MUSIC            = 0x0035             // All Users/My Music
const CSIDL_COMMON_PICTURES         = 0x0036             // All Users/My Pictures
const CSIDL_COMMON_VIDEO            = 0x0037             // All Users/My Video
const CSIDL_RESOURCES               = 0x0038             // Resource Direcotry
const CSIDL_RESOURCES_LOCALIZED     = 0x0039             // Localized Resource Direcotry
const CSIDL_COMMON_OEM_LINKS        = 0x003a             // Links to All Users OEM specific apps
const CSIDL_CDBURN_AREA             = 0x003b             // USERPROFILE/Local Settings/Application Data/Microsoft/CD Burning
const CSIDL_COMPUTERSNEARME         = 0x003d             // Computers Near Me (computered from Workgroup membership)
const CSIDL_FLAG_CREATE             = 0x8000             // combine with CSIDL_ value to force folder creation in SHGetFolderPath()
const CSIDL_FLAG_DONT_VERIFY        = 0x4000             // combine with CSIDL_ value to return an unverified folder path
const CSIDL_FLAG_DONT_UNEXPAND      = 0x2000             // combine with CSIDL_ value to avoid unexpanding environment variables
const CSIDL_FLAG_NO_ALIAS           = 0x1000             // combine with CSIDL_ value to insure non-alias versions of the pidl
const CSIDL_FLAG_PER_USER_INIT      = 0x0800             // combine with CSIDL_ value to indicate per-user init (eg. upgrade)
const CSIDL_FLAG_MASK               = 0xFF00             // mask for all possible flag values

const SHGFI_ICON              = 0x000000100     // get icon
const SHGFI_DISPLAYNAME       = 0x000000200     // get display name
const SHGFI_TYPENAME          = 0x000000400     // get type name
const SHGFI_ATTRIBUTES        = 0x000000800     // get attributes
const SHGFI_ICONLOCATION      = 0x000001000     // get icon location
const SHGFI_EXETYPE           = 0x000002000     // return exe type
const SHGFI_SYSICONINDEX      = 0x000004000     // get system icon index
const SHGFI_LINKOVERLAY       = 0x000008000     // put a link overlay on icon
const SHGFI_SELECTED          = 0x000010000     // show icon in selected state
const SHGFI_ATTR_SPECIFIED    = 0x000020000     // get only specified attributes
const SHGFI_LARGEICON         = 0x000000000     // get large icon
const SHGFI_SMALLICON         = 0x000000001     // get small icon
const SHGFI_OPENICON          = 0x000000002     // get open icon
const SHGFI_SHELLICONSIZE     = 0x000000004     // get shell size icon
const SHGFI_PIDL              = 0x000000008     // pszPath is a pidl
const SHGFI_USEFILEATTRIBUTES = 0x000000010     // use passed dwFileAttribute
const SHGFI_ADDOVERLAYS       = 0x000000020     // apply the appropriate overlays
const SHGFI_OVERLAYINDEX      = 0x000000040     // Get the index of the overlay

const SHGSI_ICONLOCATION  = 0                       // you always get the icon location
const SHGSI_ICON          = SHGFI_ICON
const SHGSI_SYSICONINDEX  = SHGFI_SYSICONINDEX
const SHGSI_LINKOVERLAY   = SHGFI_LINKOVERLAY
const SHGSI_SELECTED      = SHGFI_SELECTED
const SHGSI_LARGEICON     = SHGFI_LARGEICON
const SHGSI_SMALLICON     = SHGFI_SMALLICON
const SHGSI_SHELLICONSIZE = SHGFI_SHELLICONSIZE

enum SHSTOCKICONID
{
    SIID_DOCNOASSOC        = 0       // document (blank page) no associated program
    SIID_DOCASSOC          = 1       // document with an associated program
    SIID_APPLICATION       = 2       // generic application with no custom icon
    SIID_FOLDER            = 3       // folder (closed)
    SIID_FOLDEROPEN        = 4       // folder (open)
    SIID_DRIVE525          = 5       // 5.25" floppy disk drive
    SIID_DRIVE35           = 6       // 3.5" floppy disk drive
    SIID_DRIVEREMOVE       = 7       // removable drive
    SIID_DRIVEFIXED        = 8       // fixed (hard disk) drive
    SIID_DRIVENET          = 9       // network drive
    SIID_DRIVENETDISABLED  = 10      // disconnected network drive
    SIID_DRIVECD           = 11      // CD drive
    SIID_DRIVERAM          = 12      // RAM disk drive
    SIID_WORLD             = 13      // entire network
    SIID_SERVER            = 15      // a computer on the network
    SIID_PRINTER           = 16      // printer
    SIID_MYNETWORK         = 17      // My network places
    SIID_FIND              = 22      // Find
    SIID_HELP              = 23      // Help
    SIID_SHARE             = 28      // overlay for shared items
    SIID_LINK              = 29      // overlay for shortcuts to items
    SIID_SLOWFILE          = 30      // overlay for slow items
    SIID_RECYCLER          = 31      // empty recycle bin
    SIID_RECYCLERFULL      = 32      // full recycle bin
    SIID_MEDIACDAUDIO      = 40      // Audio CD Media
    SIID_LOCK              = 47      // Security lock
    SIID_AUTOLIST          = 49      // AutoList
    SIID_PRINTERNET        = 50      // Network printer
    SIID_SERVERSHARE       = 51      // Server share
    SIID_PRINTERFAX        = 52      // Fax printer
    SIID_PRINTERFAXNET     = 53      // Networked Fax Printer
    SIID_PRINTERFILE       = 54      // Print to File
    SIID_STACK             = 55      // Stack
    SIID_MEDIASVCD         = 56      // SVCD Media
    SIID_STUFFEDFOLDER     = 57      // Folder containing other items
    SIID_DRIVEUNKNOWN      = 58      // Unknown drive
    SIID_DRIVEDVD          = 59      // DVD Drive
    SIID_MEDIADVD          = 60      // DVD Media
    SIID_MEDIADVDRAM       = 61      // DVD-RAM Media
    SIID_MEDIADVDRW        = 62      // DVD-RW Media
    SIID_MEDIADVDR         = 63      // DVD-R Media
    SIID_MEDIADVDROM       = 64      // DVD-ROM Media
    SIID_MEDIACDAUDIOPLUS  = 65      // CD+ (Enhanced CD) Media
    SIID_MEDIACDRW         = 66      // CD-RW Media
    SIID_MEDIACDR          = 67      // CD-R Media
    SIID_MEDIACDBURN       = 68      // Burning CD
    SIID_MEDIABLANKCD      = 69      // Blank CD Media
    SIID_MEDIACDROM        = 70      // CD-ROM Media
    SIID_AUDIOFILES        = 71      // Audio files
    SIID_IMAGEFILES        = 72      // Image files
    SIID_VIDEOFILES        = 73      // Video files
    SIID_MIXEDFILES        = 74      // Mixed files
    SIID_FOLDERBACK        = 75      // Folder back
    SIID_FOLDERFRONT       = 76      // Folder front
    SIID_SHIELD            = 77      // Security shield. Use for UAC prompts only.
    SIID_WARNING           = 78      // Warning
    SIID_INFO              = 79      // Informational
    SIID_ERROR             = 80      // Error
    SIID_KEY               = 81      // Key / Secure
    SIID_SOFTWARE          = 82      // Software
    SIID_RENAME            = 83      // Rename
    SIID_DELETE            = 84      // Delete
    SIID_MEDIAAUDIODVD     = 85      // Audio DVD Media
    SIID_MEDIAMOVIEDVD     = 86      // Movie DVD Media
    SIID_MEDIAENHANCEDCD   = 87      // Enhanced CD Media
    SIID_MEDIAENHANCEDDVD  = 88      // Enhanced DVD Media
    SIID_MEDIAHDDVD        = 89      // HD-DVD Media
    SIID_MEDIABLURAY       = 90      // BluRay Media
    SIID_MEDIAVCD          = 91      // VCD Media
    SIID_MEDIADVDPLUSR     = 92      // DVD+R Media
    SIID_MEDIADVDPLUSRW    = 93      // DVD+RW Media
    SIID_DESKTOPPC         = 94      // desktop computer
    SIID_MOBILEPC          = 95      // mobile computer (laptop/notebook)
    SIID_USERS             = 96      // users
    SIID_MEDIASMARTMEDIA   = 97      // Smart Media
    SIID_MEDIACOMPACTFLASH = 98      // Compact Flash
    SIID_DEVICECELLPHONE   = 99      // Cell phone
    SIID_DEVICECAMERA      = 100     // Camera
    SIID_DEVICEVIDEOCAMERA = 101     // Video camera
    SIID_DEVICEAUDIOPLAYER = 102     // Audio player
    SIID_NETWORKCONNECT    = 103     // Connect to network
    SIID_INTERNET          = 104     // Internet
    SIID_ZIPFILE           = 105     // ZIP file
    SIID_SETTINGS          = 106     // Settings
    // 107-131 are internal Vista RTM icons
    // 132-159 for SP1 icons
    SIID_DRIVEHDDVD     = 132     // HDDVD Drive (all types)
    SIID_DRIVEBD        = 133     // BluRay Drive (all types)
    SIID_MEDIAHDDVDROM  = 134     // HDDVD-ROM Media
    SIID_MEDIAHDDVDR    = 135     // HDDVD-R Media
    SIID_MEDIAHDDVDRAM  = 136     // HDDVD-RAM Media
    SIID_MEDIABDROM     = 137     // BluRay ROM Media
    SIID_MEDIABDR       = 138     // BluRay R Media
    SIID_MEDIABDRE      = 139     // BluRay RE Media (Rewriable and RAM)
    SIID_CLUSTEREDDRIVE = 140     // Clustered disk
    // 160+ are for Windows 7 icons
    SIID_MAX_ICONS = 181
}

struct SHFILEINFOA
{
    hIcon:             HICON
    iIcon:             s32
    dwAttributes:      DWORD
    szDisplayName:     [MAX_PATH] CHAR
    szTypeName:        [80] CHAR
}

struct SHFILEINFOW
{
    hIcon:             HICON
    iIcon:             s32
    dwAttributes:      DWORD
    szDisplayName:     [MAX_PATH] WCHAR
    szTypeName:        [80] WCHAR
}

struct SHSTOCKICONINFO
{
    cbSize:             DWORD
    hIcon:              HICON
    iSysImageIndex:     s32
    iIcon:              s32
    szPath:             [MAX_PATH] WCHAR
}

struct GUID
{
    Data1:     u32
    Data2:     u16
    Data3:     u16
    Data4:     [8] u8
}

const NIM_ADD        = 0x00000000
const NIM_MODIFY     = 0x00000001
const NIM_DELETE     = 0x00000002
const NIM_SETFOCUS   = 0x00000003
const NIM_SETVERSION = 0x00000004

const NIF_MESSAGE  = 0x00000001
const NIF_ICON     = 0x00000002
const NIF_TIP      = 0x00000004
const NIF_STATE    = 0x00000008
const NIF_INFO     = 0x00000010
const NIF_GUID     = 0x00000020
const NIF_REALTIME = 0x00000040
const NIF_SHOWTIP  = 0x00000080

struct NOTIFYICONDATAA
{
    cbSize:               DWORD
    hWnd:                 HWND
    uID:                  UINT
    uFlags:               UINT
    uCallbackMessage:     UINT
    hIcon:                HICON
    szTip:                [128] CHAR
    dwState:              DWORD
    dwStateMask:          DWORD
    szInfo:               [256] CHAR
    using DUMMYUNIONNAME: union
    {
        uTimeout:     UINT
        uVersion:     UINT
    }
    szInfoTitle:      [64] CHAR
    dwInfoFlags:      DWORD
    guidItem:         GUID
    hBalloonIcon:     HICON
}

struct NOTIFYICONDATAW
{
    cbSize:               DWORD
    hWnd:                 HWND
    uID:                  UINT
    uFlags:               UINT
    uCallbackMessage:     UINT
    hIcon:                HICON
    szTip:                [128] WCHAR
    dwState:              DWORD
    dwStateMask:          DWORD
    szInfo:               [256] WCHAR
    using DUMMYUNIONNAME: union
    {
        uTimeout:     UINT
        uVersion:     UINT
    }
    szInfoTitle:      [64] WCHAR
    dwInfoFlags:      DWORD
    guidItem:         GUID
    hBalloonIcon:     HICON
}

private namespace Shell32Priv
{
    #[Foreign("shell32")]
    {
        func SHGetSpecialFolderPathA(hwnd: HWND, pszPath: LPSTR, csidl: s32, fCreate: BOOL)->BOOL;
        func SHGetSpecialFolderPathW(hwnd: HWND, pszPath: LPWSTR, csidl: s32, fCreate: BOOL)->BOOL;
        func ExtractIconExA(lpszFile: LPCSTR, nIconIndex: s32, phiconLarge: #null *HICON, phiconSmall: #null *HICON, nIcons: UINT)->UINT;
        func ExtractIconExW(lpszFile: LPCWSTR, nIconIndex: s32, phiconLarge: #null *HICON, phiconSmall: #null *HICON, nIcons: UINT)->UINT;
        func SHGetStockIconInfo(siid: SHSTOCKICONID, uFlags: UINT, psii: SHSTOCKICONINFO)->SHSTDAPI;
        func Shell_NotifyIconA(dwMessage: DWORD, lpData: *NOTIFYICONDATAA)->BOOL;
        func Shell_NotifyIconW(dwMessage: DWORD, lpData: *NOTIFYICONDATAW)->BOOL;
        func ShellExecuteA(hwnd: HWND, lpOperation: LPCSTR, lpFile: LPCSTR, lpParameters: LPCSTR, lpDirectory: LPCSTR, nShowCmd: INT)->HINSTANCE;
    }
}

#[Foreign("shell32")]
{
    func SHGetFileInfoA(pszPath: LPCSTR, dwFileAttributes: DWORD, psfi: *SHFILEINFOA, cbFileInfo: UINT, uFlags: UINT)->DWORD_PTR;
    func SHGetFileInfoW(pszPath: LPCWSTR, dwFileAttributes: DWORD, psfi: *SHFILEINFOW, cbFileInfo: UINT, uFlags: UINT)->DWORD_PTR;
}

#[Inline]
{
    func SHGetSpecialFolderPathA(hwnd: HWND, pszPath: LPSTR, csidl: s32, fCreate: BOOL) throw
    {
        if !Shell32Priv.SHGetSpecialFolderPathA(hwnd, pszPath, csidl, fCreate) do
            throwWinError(GetLastError())
    }

    func SHGetSpecialFolderPathW(hwnd: HWND, pszPath: LPWSTR, csidl: s32, fCreate: BOOL) throw
    {
        if !Shell32Priv.SHGetSpecialFolderPathW(hwnd, pszPath, csidl, fCreate) do
            throwWinError(GetLastError())
    }

    #[Swag.Discardable]
    func ExtractIconExA(lpszFile: LPCSTR, nIconIndex: s32, phiconLarge: #null *HICON, phiconSmall: #null *HICON, nIcons: UINT)->UINT throw
    {
        let res = Shell32Priv.ExtractIconExA(lpszFile, nIconIndex, phiconLarge, phiconSmall, nIcons)
        if res == Swag.U32.Max do
            throwWinError(GetLastError())
        return res
    }

    #[Swag.Discardable]
    func ExtractIconExW(lpszFile: LPCWSTR, nIconIndex: s32, phiconLarge: #null *HICON, phiconSmall: #null *HICON, nIcons: UINT)->UINT throw
    {
        let res = Shell32Priv.ExtractIconExW(lpszFile, nIconIndex, phiconLarge, phiconSmall, nIcons)
        if res == Swag.U32.Max do
            throwWinError(GetLastError())
        return res
    }

    func SHGetStockIconInfo(siid: SHSTOCKICONID, uFlags: UINT, psii: SHSTOCKICONINFO) throw
    {
        if Shell32Priv.SHGetStockIconInfo(siid, uFlags, psii) != S_OK do
            throwWinError(GetLastError())
    }

    func Shell_NotifyIconA(dwMessage: DWORD, lpData: *NOTIFYICONDATAA) throw
    {
        if !Shell32Priv.Shell_NotifyIconA(dwMessage, lpData) do
            throwWinError(GetLastError())
    }

    func Shell_NotifyIconW(dwMessage: DWORD, lpData: *NOTIFYICONDATAW) throw
    {
        if !Shell32Priv.Shell_NotifyIconW(dwMessage, lpData) do
            throwWinError(GetLastError())
    }

    func ShellExecuteA(hwnd: HWND, lpOperation: LPCSTR, lpFile: LPCSTR, lpParameters: LPCSTR, lpDirectory: LPCSTR, nShowCmd: INT)->HINSTANCE throw
    {
        let result = Shell32Priv.ShellExecuteA(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd)
        if cast(u64) result <= 32 do  // ?!
            throwWinError(GetLastError())
        return result
    }
}
