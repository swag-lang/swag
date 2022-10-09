#global export
#foreignlib "shell32"
using Swag

const CSIDL_DESKTOP                     = 0x0000        // <desktop>
const CSIDL_INTERNET                    = 0x0001        // Internet Explorer (icon on desktop)
const CSIDL_PROGRAMS                    = 0x0002        // Start Menu\Programs
const CSIDL_CONTROLS                    = 0x0003        // My Computer\Control Panel
const CSIDL_PRINTERS                    = 0x0004        // My Computer\Printers
const CSIDL_PERSONAL                    = 0x0005        // My Documents
const CSIDL_FAVORITES                   = 0x0006        // <user name>\Favorites
const CSIDL_STARTUP                     = 0x0007        // Start Menu\Programs\Startup
const CSIDL_RECENT                      = 0x0008        // <user name>\Recent
const CSIDL_SENDTO                      = 0x0009        // <user name>\SendTo
const CSIDL_BITBUCKET                   = 0x000a        // <desktop>\Recycle Bin
const CSIDL_STARTMENU                   = 0x000b        // <user name>\Start Menu
const CSIDL_MYDOCUMENTS                 = CSIDL_PERSONAL //  Personal was just a silly name for My Documents
const CSIDL_MYMUSIC                     = 0x000d        // "My Music" folder
const CSIDL_MYVIDEO                     = 0x000e        // "My Videos" folder
const CSIDL_DESKTOPDIRECTORY            = 0x0010        // <user name>\Desktop
const CSIDL_DRIVES                      = 0x0011        // My Computer
const CSIDL_NETWORK                     = 0x0012        // Network Neighborhood (My Network Places)
const CSIDL_NETHOOD                     = 0x0013        // <user name>\nethood
const CSIDL_FONTS                       = 0x0014        // windows\fonts
const CSIDL_TEMPLATES                   = 0x0015
const CSIDL_COMMON_STARTMENU            = 0x0016        // All Users\Start Menu
const CSIDL_COMMON_PROGRAMS             = 0X0017        // All Users\Start Menu\Programs
const CSIDL_COMMON_STARTUP              = 0x0018        // All Users\Startup
const CSIDL_COMMON_DESKTOPDIRECTORY     = 0x0019        // All Users\Desktop
const CSIDL_APPDATA                     = 0x001a        // <user name>\Application Data
const CSIDL_PRINTHOOD                   = 0x001b        // <user name>\PrintHood
const CSIDL_LOCAL_APPDATA               = 0x001c        // <user name>\Local Settings\Applicaiton Data (non roaming)
const CSIDL_ALTSTARTUP                  = 0x001d        // non localized startup
const CSIDL_COMMON_ALTSTARTUP           = 0x001e        // non localized common startup
const CSIDL_COMMON_FAVORITES            = 0x001f
const CSIDL_INTERNET_CACHE              = 0x0020
const CSIDL_COOKIES                     = 0x0021
const CSIDL_HISTORY                     = 0x0022
const CSIDL_COMMON_APPDATA              = 0x0023        // All Users\Application Data
const CSIDL_WINDOWS                     = 0x0024        // GetWindowsDirectory()
const CSIDL_SYSTEM                      = 0x0025        // GetSystemDirectory()
const CSIDL_PROGRAM_FILES               = 0x0026        // C:\Program Files
const CSIDL_MYPICTURES                  = 0x0027        // C:\Program Files\My Pictures
const CSIDL_PROFILE                     = 0x0028        // USERPROFILE
const CSIDL_SYSTEMX86                   = 0x0029        // x86 system directory on RISC
const CSIDL_PROGRAM_FILESX86            = 0x002a        // x86 C:\Program Files on RISC
const CSIDL_PROGRAM_FILES_COMMON        = 0x002b        // C:\Program Files\Common
const CSIDL_PROGRAM_FILES_COMMONX86     = 0x002c        // x86 Program Files\Common on RISC
const CSIDL_COMMON_TEMPLATES            = 0x002d        // All Users\Templates
const CSIDL_COMMON_DOCUMENTS            = 0x002e        // All Users\Documents
const CSIDL_COMMON_ADMINTOOLS           = 0x002f        // All Users\Start Menu\Programs\Administrative Tools
const CSIDL_ADMINTOOLS                  = 0x0030        // <user name>\Start Menu\Programs\Administrative Tools
const CSIDL_CONNECTIONS                 = 0x0031        // Network and Dial-up Connections
const CSIDL_COMMON_MUSIC                = 0x0035        // All Users\My Music
const CSIDL_COMMON_PICTURES             = 0x0036        // All Users\My Pictures
const CSIDL_COMMON_VIDEO                = 0x0037        // All Users\My Video
const CSIDL_RESOURCES                   = 0x0038        // Resource Direcotry
const CSIDL_RESOURCES_LOCALIZED         = 0x0039        // Localized Resource Direcotry
const CSIDL_COMMON_OEM_LINKS            = 0x003a        // Links to All Users OEM specific apps
const CSIDL_CDBURN_AREA                 = 0x003b        // USERPROFILE\Local Settings\Application Data\Microsoft\CD Burning
const CSIDL_COMPUTERSNEARME             = 0x003d        // Computers Near Me (computered from Workgroup membership)
const CSIDL_FLAG_CREATE                 = 0x8000        // combine with CSIDL_ value to force folder creation in SHGetFolderPath()
const CSIDL_FLAG_DONT_VERIFY            = 0x4000        // combine with CSIDL_ value to return an unverified folder path
const CSIDL_FLAG_DONT_UNEXPAND          = 0x2000        // combine with CSIDL_ value to avoid unexpanding environment variables
const CSIDL_FLAG_NO_ALIAS               = 0x1000        // combine with CSIDL_ value to insure non-alias versions of the pidl
const CSIDL_FLAG_PER_USER_INIT          = 0x0800        // combine with CSIDL_ value to indicate per-user init (eg. upgrade)
const CSIDL_FLAG_MASK                   = 0xFF00        // mask for all possible flag values

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

struct SHFILEINFOA
{
    hIcon:            HICON
    iIcon:            s32
    dwAttributes:     DWORD
    szDisplayName:    [MAX_PATH] CHAR
    szTypeName:       [80] CHAR
}

struct SHFILEINFOW
{
    hIcon:            HICON
    iIcon:            s32
    dwAttributes:     DWORD
    szDisplayName:    [MAX_PATH] WCHAR
    szTypeName:       [80] WCHAR
}

namespace Shell32Priv
{
    #[Foreign("shell32")]
    {
        func SHGetSpecialFolderPathA(hwnd: HWND, pszPath: LPSTR, csidl: s32, fCreate: BOOL)->BOOL;
        func SHGetSpecialFolderPathW(hwnd: HWND, pszPath: LPWSTR, csidl: s32, fCreate: BOOL)->BOOL;
        func ExtractIconExA(lpszFile: LPCSTR, nIconIndex: s32, phiconLarge: *HICON, phiconSmall: *HICON, nIcons: UINT)->UINT;
        func ExtractIconExW(lpszFile: LPCWSTR, nIconIndex: s32, phiconLarge: *HICON, phiconSmall: *HICON, nIcons: UINT)->UINT;
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
        if !Shell32Priv.SHGetSpecialFolderPathA(hwnd, pszPath, csidl, fCreate)
            try setError(GetLastError())
    }

    func SHGetSpecialFolderPathW(hwnd: HWND, pszPath: LPWSTR, csidl: s32, fCreate: BOOL) throw
    {
        if !Shell32Priv.SHGetSpecialFolderPathW(hwnd, pszPath, csidl, fCreate)
            try setError(GetLastError())
    }

    #[Swag.Discardable]
    func ExtractIconExA(lpszFile: LPCSTR, nIconIndex: s32, phiconLarge: *HICON, phiconSmall: *HICON, nIcons: UINT)->UINT throw
    {
        res := Shell32Priv.ExtractIconExA(lpszFile, nIconIndex, phiconLarge, phiconSmall, nIcons)
        if res == Swag.U32.Max
            try setError(GetLastError())
        return res
    }

    #[Swag.Discardable]
    func ExtractIconExW(lpszFile: LPCWSTR, nIconIndex: s32, phiconLarge: *HICON, phiconSmall: *HICON, nIcons: UINT)->UINT throw
    {
        res := Shell32Priv.ExtractIconExW(lpszFile, nIconIndex, phiconLarge, phiconSmall, nIcons)
        if res == Swag.U32.Max
            try setError(GetLastError())
        return res
    }
}