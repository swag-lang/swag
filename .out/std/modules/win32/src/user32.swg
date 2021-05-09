#global export
#global foreignlib "user32"
using swag

alias HWND = 		const *void
alias HMENU = 		const *void
alias HICON =		const *void
alias HCURSOR =		const *void
alias HBRUSH =		const *void
alias HDC =		    const *void
alias WPARAM =      u64
alias LPARAM =      u64

const VK_LBUTTON        = 0x01
const VK_RBUTTON        = 0x02
const VK_CANCEL         = 0x03
const VK_MBUTTON        = 0x04
const VK_XBUTTON1       = 0x05
const VK_XBUTTON2       = 0x06
const VK_BACK           = 0x08
const VK_TAB            = 0x09
const VK_CLEAR          = 0x0C
const VK_RETURN         = 0x0D
const VK_SHIFT          = 0x10
const VK_CONTROL        = 0x11
const VK_MENU           = 0x12
const VK_PAUSE          = 0x13
const VK_CAPITAL        = 0x14
const VK_KANA           = 0x15
const VK_HANGEUL        = 0x15
const VK_HANGUL         = 0x15
const VK_JUNJA          = 0x17
const VK_FINAL          = 0x18
const VK_HANJA          = 0x19
const VK_KANJI          = 0x19
const VK_ESCAPE         = 0x1B
const VK_CONVERT        = 0x1C
const VK_NONCONVERT     = 0x1D
const VK_ACCEPT         = 0x1E
const VK_MODECHANGE     = 0x1F
const VK_SPACE          = 0x20
const VK_PRIOR          = 0x21
const VK_NEXT           = 0x22
const VK_END            = 0x23
const VK_HOME           = 0x24
const VK_LEFT           = 0x25
const VK_UP             = 0x26
const VK_RIGHT          = 0x27
const VK_DOWN           = 0x28
const VK_SELECT         = 0x29
const VK_PRINT          = 0x2A
const VK_EXECUTE        = 0x2B
const VK_SNAPSHOT       = 0x2C
const VK_INSERT         = 0x2D
const VK_DELETE         = 0x2E
const VK_HELP           = 0x2F
const VK_LWIN           = 0x5B
const VK_RWIN           = 0x5C
const VK_APPS           = 0x5D
const VK_SLEEP          = 0x5F
const VK_NUMPAD0        = 0x60
const VK_NUMPAD1        = 0x61
const VK_NUMPAD2        = 0x62
const VK_NUMPAD3        = 0x63
const VK_NUMPAD4        = 0x64
const VK_NUMPAD5        = 0x65
const VK_NUMPAD6        = 0x66
const VK_NUMPAD7        = 0x67
const VK_NUMPAD8        = 0x68
const VK_NUMPAD9        = 0x69
const VK_MULTIPLY       = 0x6A
const VK_ADD            = 0x6B
const VK_SEPARATOR      = 0x6C
const VK_SUBTRACT       = 0x6D
const VK_DECIMAL        = 0x6E
const VK_DIVIDE         = 0x6F
const VK_F1             = 0x70
const VK_F2             = 0x71
const VK_F3             = 0x72
const VK_F4             = 0x73
const VK_F5             = 0x74
const VK_F6             = 0x75
const VK_F7             = 0x76
const VK_F8             = 0x77
const VK_F9             = 0x78
const VK_F10            = 0x79
const VK_F11            = 0x7A
const VK_F12            = 0x7B
const VK_F13            = 0x7C
const VK_F14            = 0x7D
const VK_F15            = 0x7E
const VK_F16            = 0x7F
const VK_F17            = 0x80
const VK_F18            = 0x81
const VK_F19            = 0x82
const VK_F20            = 0x83
const VK_F21            = 0x84
const VK_F22            = 0x85
const VK_F23            = 0x86
const VK_F24            = 0x87
const VK_NAVIGATION_VIEW     = 0x88
const VK_NAVIGATION_MENU     = 0x89
const VK_NAVIGATION_UP       = 0x8A
const VK_NAVIGATION_DOWN     = 0x8B
const VK_NAVIGATION_LEFT     = 0x8C
const VK_NAVIGATION_RIGHT    = 0x8D
const VK_NAVIGATION_ACCEPT   = 0x8E
const VK_NAVIGATION_CANCEL   = 0x8F
const VK_NUMLOCK        = 0x90
const VK_SCROLL         = 0x91
const VK_OEM_NEC_EQUAL  = 0x92
const VK_OEM_FJ_JISHO   = 0x92
const VK_OEM_FJ_MASSHOU = 0x93
const VK_OEM_FJ_TOUROKU = 0x94
const VK_OEM_FJ_LOYA    = 0x95
const VK_OEM_FJ_ROYA    = 0x96
const VK_LSHIFT         = 0xA0
const VK_RSHIFT         = 0xA1
const VK_LCONTROL       = 0xA2
const VK_RCONTROL       = 0xA3
const VK_LMENU          = 0xA4
const VK_RMENU          = 0xA5
const VK_BROWSER_BACK        = 0xA6
const VK_BROWSER_FORWARD     = 0xA7
const VK_BROWSER_REFRESH     = 0xA8
const VK_BROWSER_STOP        = 0xA9
const VK_BROWSER_SEARCH      = 0xAA
const VK_BROWSER_FAVORITES   = 0xAB
const VK_BROWSER_HOME        = 0xAC
const VK_VOLUME_MUTE         = 0xAD
const VK_VOLUME_DOWN         = 0xAE
const VK_VOLUME_UP           = 0xAF
const VK_MEDIA_NEXT_TRACK    = 0xB0
const VK_MEDIA_PREV_TRACK    = 0xB1
const VK_MEDIA_STOP          = 0xB2
const VK_MEDIA_PLAY_PAUSE    = 0xB3
const VK_LAUNCH_MAIL         = 0xB4
const VK_LAUNCH_MEDIA_SELECT = 0xB5
const VK_LAUNCH_APP1         = 0xB6
const VK_LAUNCH_APP2         = 0xB7
const VK_OEM_1				 = 0xBA
const VK_OEM_PLUS			 = 0xBB
const VK_OEM_COMMA			 = 0xBC
const VK_OEM_MINUS			 = 0xBD
const VK_OEM_PERIOD			 = 0xBE
const VK_OEM_2				 = 0xBF
const VK_OEM_3				 = 0xC0
const VK_GAMEPAD_A                         = 0xC3
const VK_GAMEPAD_B                         = 0xC4
const VK_GAMEPAD_X                         = 0xC5
const VK_GAMEPAD_Y                         = 0xC6
const VK_GAMEPAD_RIGHT_SHOULDER            = 0xC7
const VK_GAMEPAD_LEFT_SHOULDER             = 0xC8
const VK_GAMEPAD_LEFT_TRIGGER              = 0xC9
const VK_GAMEPAD_RIGHT_TRIGGER             = 0xCA
const VK_GAMEPAD_DPAD_UP                   = 0xCB
const VK_GAMEPAD_DPAD_DOWN                 = 0xCC
const VK_GAMEPAD_DPAD_LEFT                 = 0xCD
const VK_GAMEPAD_DPAD_RIGHT                = 0xCE
const VK_GAMEPAD_MENU                      = 0xCF
const VK_GAMEPAD_VIEW                      = 0xD0
const VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON    = 0xD1
const VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON   = 0xD2
const VK_GAMEPAD_LEFT_THUMBSTICK_UP        = 0xD3
const VK_GAMEPAD_LEFT_THUMBSTICK_DOWN      = 0xD4
const VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT     = 0xD5
const VK_GAMEPAD_LEFT_THUMBSTICK_LEFT      = 0xD6
const VK_GAMEPAD_RIGHT_THUMBSTICK_UP       = 0xD7
const VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN     = 0xD8
const VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT    = 0xD9
const VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT     = 0xDA
const VK_OEM_4          = 0xDB
const VK_OEM_5          = 0xDC
const VK_OEM_6          = 0xDD
const VK_OEM_7          = 0xDE
const VK_OEM_8          = 0xDF
const VK_OEM_AX         = 0xE1
const VK_OEM_102        = 0xE2
const VK_ICO_HELP       = 0xE3
const VK_ICO_00         = 0xE4
const VK_PROCESSKEY     = 0xE5
const VK_ICO_CLEAR      = 0xE6
const VK_PACKET         = 0xE7
const VK_OEM_RESET      = 0xE9
const VK_OEM_JUMP       = 0xEA
const VK_OEM_PA1        = 0xEB
const VK_OEM_PA2        = 0xEC
const VK_OEM_PA3        = 0xED
const VK_OEM_WSCTRL     = 0xEE
const VK_OEM_CUSEL      = 0xEF
const VK_OEM_ATTN       = 0xF0
const VK_OEM_FINISH     = 0xF1
const VK_OEM_COPY       = 0xF2
const VK_OEM_AUTO       = 0xF3
const VK_OEM_ENLW       = 0xF4
const VK_OEM_BACKTAB    = 0xF5
const VK_ATTN           = 0xF6
const VK_CRSEL          = 0xF7
const VK_EXSEL          = 0xF8
const VK_EREOF          = 0xF9
const VK_PLAY           = 0xFA
const VK_ZOOM           = 0xFB
const VK_NONAME         = 0xFC
const VK_PA1            = 0xFD
const VK_OEM_CLEAR      = 0xFE

const MB_OK               		= 0x00000000
const MB_OKCANCEL               = 0x00000001
const MB_ABORTRETRYIGNORE       = 0x00000002
const MB_YESNOCANCEL            = 0x00000003
const MB_YESNO                  = 0x00000004
const MB_RETRYCANCEL            = 0x00000005
const MB_CANCELTRYCONTINUE      = 0x00000006
const MB_ICONHAND				= 0x00000010
const MB_ICONQUESTION           = 0x00000020
const MB_ICONEXCLAMATION        = 0x00000030
const MB_ICONASTERISK           = 0x00000040
const MB_USERICON               = 0x00000080
const MB_ICONWARNING            = MB_ICONEXCLAMATION
const MB_ICONERROR              = MB_ICONHAND
const MB_ICONINFORMATION        = MB_ICONASTERISK
const MB_ICONSTOP               = MB_ICONHAND
const MB_DEFBUTTON1             = 0x00000000
const MB_DEFBUTTON2             = 0x00000100
const MB_DEFBUTTON3             = 0x00000200
const MB_DEFBUTTON4             = 0x00000300
const MB_APPLMODAL              = 0x00000000
const MB_SYSTEMMODAL            = 0x00001000
const MB_TASKMODAL              = 0x00002000
const MB_HELP                   = 0x00004000
const MB_NOFOCUS                = 0x00008000
const MB_SETFOREGROUND          = 0x00010000
const MB_DEFAULT_DESKTOP_ONLY   = 0x00020000
const MB_TOPMOST                = 0x00040000
const MB_RIGHT                  = 0x00080000
const MB_RTLREADING             = 0x00100000

const IDOK				= 1
const IDCANCEL          = 2
const IDABORT           = 3
const IDRETRY           = 4
const IDIGNORE        	= 5
const IDYES           	= 6
const IDNO            	= 7
const IDCLOSE         	= 8
const IDHELP          	= 9
const IDTRYAGAIN      	= 10
const IDCONTINUE      	= 11
const IDTIMEOUT 		= 32000

struct WNDCLASSA
{
    style:            UINT
    lpfnWndProc:      const *void
    cbClsExtra:       s32
    cbWndExtra:       s32
    hInstance:        HINSTANCE
    hIcon:            HICON
    hCursor:          HCURSOR
    hbrBackground:    HBRUSH
    lpszMenuName:     LPCSTR
    lpszClassName:    LPCSTR
}

struct WNDCLASSW
{
    style:            UINT
    lpfnWndProc:      const *void
    cbClsExtra:       s32
    cbWndExtra:       s32
    hInstance:        HINSTANCE
    hIcon:            HICON
    hCursor:          HCURSOR
    hbrBackground:    HBRUSH
    lpszMenuName:     LPCWSTR
    lpszClassName:    LPCWSTR
}

const WS_OVERLAPPED       = 0x00000000
const WS_POPUP            = 0x80000000
const WS_CHILD            = 0x40000000
const WS_MINIMIZE         = 0x20000000
const WS_VISIBLE          = 0x10000000
const WS_DISABLED         = 0x08000000
const WS_CLIPSIBLINGS     = 0x04000000
const WS_CLIPCHILDREN     = 0x02000000
const WS_MAXIMIZE         = 0x01000000
const WS_CAPTION          = 0x00C00000
const WS_BORDER           = 0x00800000
const WS_DLGFRAME         = 0x00400000
const WS_VSCROLL          = 0x00200000
const WS_HSCROLL          = 0x00100000
const WS_SYSMENU          = 0x00080000
const WS_THICKFRAME       = 0x00040000
const WS_GROUP            = 0x00020000
const WS_TABSTOP          = 0x00010000
const WS_MINIMIZEBOX      = 0x00020000
const WS_MAXIMIZEBOX      = 0x00010000
const WS_OVERLAPPEDWINDOW = WS_OVERLAPPED |WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX

const CW_USEDEFAULT       = -1

const SW_HIDE             = 0
const SW_SHOWNORMAL       = 1
const SW_NORMAL           = 1
const SW_SHOWMINIMIZED    = 2
const SW_SHOWMAXIMIZED    = 3
const SW_MAXIMIZE         = 3
const SW_SHOWNOACTIVATE   = 4
const SW_SHOW             = 5
const SW_MINIMIZE         = 6
const SW_SHOWMINNOACTIVE  = 7
const SW_SHOWNA           = 8
const SW_RESTORE          = 9
const SW_SHOWDEFAULT      = 10
const SW_FORCEMINIMIZE    = 11
const SW_MAX              = 11

struct MSG
{
    hwnd:       HWND
    message:    UINT
    wParam:     WPARAM
    lParam:     LPARAM
    time:       DWORD
    pt:         POINT
    lPrivate:   DWORD
}

const WA_INACTIVE     = 0
const WA_ACTIVE       = 1
const WA_CLICKACTIVE  = 2

const WM_CREATE         = 0x0001
const WM_DESTROY        = 0x0002
const WM_MOVE           = 0x0003
const WM_SIZE           = 0x0005
const WM_ACTIVATE       = 0x0006
const WM_SETFOCUS       = 0x0007
const WM_KILLFOCUS      = 0x0008
const WM_ENABLE         = 0x000A
const WM_SETREDRAW      = 0x000B
const WM_SETTEXT        = 0x000C
const WM_GETTEXT        = 0x000D
const WM_GETTEXTLENGTH  = 0x000E
const WM_PAINT          = 0x000F
const WM_CLOSE          = 0x0010
const WM_QUIT           = 0x0012
const WM_ERASEBKGND     = 0x0014
const WM_SYSCOLORCHANGE = 0x0015
const WM_SHOWWINDOW     = 0x0018
const WM_WININICHANGE   = 0x001A
const WM_SETTINGCHANGE  = WM_WININICHANGE

const PM_NOREMOVE = 0x0000
const PM_REMOVE   = 0x0001
const PM_NOYIELD  = 0x0002

const CS_VREDRAW          = 0x0001
const CS_HREDRAW          = 0x0002
const CS_DBLCLKS          = 0x0008
const CS_OWNDC            = 0x0020
const CS_CLASSDC          = 0x0040
const CS_PARENTDC         = 0x0080
const CS_NOCLOSE          = 0x0200
const CS_SAVEBITS         = 0x0800
const CS_BYTEALIGNCLIENT  = 0x1000
const CS_BYTEALIGNWINDOW  = 0x2000
const CS_GLOBALCLASS      = 0x4000

struct RECT
{
    left:   LONG
    top:    LONG
    right:  LONG
    bottom: LONG
}

struct PAINTSTRUCT
{
    hdc:            HDC
    fErase:         BOOL
    rcPaint:        RECT
    fRestore:       BOOL
    fIncUpdate:     BOOL
    rgbReserved:    [32] BYTE
}

#[foreign("user32")]
{
    func GetAsyncKeyState(vKey: s32)->SHORT;
    func GetKeyboardState(lpKeystate: *u8)->BOOL;
    func GetCursorPos(lpPoint: *POINT);
    func SetCursorPos(x, y: s32);
    func ShowCursor(show: BOOL);

    func CreateWindowExA(dwExStyle: DWORD, lpClassName: LPCSTR, lpWindowName: LPCSTR, dwStyle: DWORD, X: s32, Y: s32, nWidth: s32, nHeight: s32, hWndParent: HWND, hMenu: HMENU, hInstance: HINSTANCE, lpParam: LPVOID)->HWND;
    func DefWindowProcA(hwnd: HWND, uMsg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT;
    func DefWindowProcW(hwnd: HWND, uMsg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT;
    func GetMessageA(lpMsg: *MSG, hWnd: HWND, wMsgFilterMin: UINT, wMsgFilterMax: UINT)->BOOL;
    func GetMessageW(lpMsg: *MSG, hWnd: HWND, wMsgFilterMin: UINT, wMsgFilterMax: UINT)->BOOL;
    func PeekMessageA(lpMsg: *MSG, hWnd: HWND, wMsgFilterMin: UINT, wMsgFilterMax: UINT, wRemoveMsg: UINT)->BOOL;
    func PeekMessageW(lpMsg: *MSG, hWnd: HWND, wMsgFilterMin: UINT, wMsgFilterMax: UINT, wRemoveMsg: UINT)->BOOL;
    func PostQuitMessage(nExitCode: s32);

    func GetDC(hWnd: HWND)->HDC;
}

#[foreign("user32"), discardable]
{
    func ShowWindow(hWnd: HWND, nCmdShow: s32)->BOOL;
    func MessageBoxA(hWnd: HWND, lpText: LPCSTR, lpCaption: LPCSTR, uType: u32)->s32;
    func MessageBoxW(hWnd: HWND, lpText: LPCWSTR, lpCaption: LPCWSTR, uType: u32)->s32;

    func BeginPaint(hWnd: HWND, lpPaint: *PAINTSTRUCT)->HDC;
    func EndPaint(hWnd: HWND, lpPaint: const *PAINTSTRUCT)->BOOL;
    func ReleaseDC(hWnd: HWND, hDC: HDC)->s32;

    func TranslateMessage(lpMsg: const *MSG)->BOOL;
    func DispatchMessageA(lpMsg: const *MSG)->LRESULT;
    func DispatchMessageW(lpMsg: const *MSG)->LRESULT;
}

namespace user32priv
{
    #[foreign("user32")]
    {
        func RegisterClassA(lpWndClass: const *WNDCLASSA)->ATOM;
        func RegisterClassW(lpWndClass: const *WNDCLASSW)->ATOM;

        func UpdateWindow(hWnd: HWND)->BOOL;
        func DestroyWindow(hWnd: HWND)->BOOL;
        func PostMessageA(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->BOOL;
        func PostMessageW(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->BOOL;
    }
}

#[inline]
{
    func PostMessageA(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM) throw
    {
        if !user32priv.PostMessageA(hWnd, msg, wParam, lParam)
            try setError(GetLastError())
    }

    func PostMessageW(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM) throw
    {
        if !user32priv.PostMessageW(hWnd, msg, wParam, lParam)
            try setError(GetLastError())
    }

    #[discardable]
    func RegisterClassA(lpWndClass: const *WNDCLASSA)->ATOM throw
    {
        res := user32priv.RegisterClassA(lpWndClass)
        if !res
            try setError(GetLastError())
        return res
    }

    #[discardable]
    func RegisterClassW(lpWndClass: const *WNDCLASSW)->ATOM throw
    {
        res := user32priv.RegisterClassW(lpWndClass)
        if !res
            try setError(GetLastError())
        return res
    }

    func UpdateWindow(hWnd: HWND) throw
    {
        if !user32priv.UpdateWindow(hWnd)
            try setError(GetLastError())
    }

    func DestroyWindow(hWnd: HWND) throw
    {
        if !user32priv.DestroyWindow(hWnd)
            try setError(GetLastError())
    }
}