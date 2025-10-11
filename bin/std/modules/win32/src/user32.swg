#global export
#foreignlib("user32")
using Swag

#[Swag.Strict]
{
    alias HWND     = const *void
    alias HMENU    = const *void
    alias HICON    = const *void
    alias HBITMAP  = const *void
    alias HCURSOR  = const *void
    alias HBRUSH   = const *void
    alias HDC      = const *void
    alias HMONITOR = const *void
}

alias WPARAM   = u64
alias LPARAM   = u64
alias COLORREF = DWORD

const MOD_ALT      = 0x0001
const MOD_CONTROL  = 0x0002
const MOD_SHIFT    = 0x0004
const MOD_WIN      = 0x0008
const MOD_NOREPEAT = 0x4000

const VK_LBUTTON                         = 0x01
const VK_RBUTTON                         = 0x02
const VK_CANCEL                          = 0x03
const VK_MBUTTON                         = 0x04
const VK_XBUTTON1                        = 0x05
const VK_XBUTTON2                        = 0x06
const VK_BACK                            = 0x08
const VK_TAB                             = 0x09
const VK_CLEAR                           = 0x0C
const VK_RETURN                          = 0x0D
const VK_SHIFT                           = 0x10
const VK_CONTROL                         = 0x11
const VK_MENU                            = 0x12
const VK_PAUSE                           = 0x13
const VK_CAPITAL                         = 0x14
const VK_KANA                            = 0x15
const VK_HANGEUL                         = 0x15
const VK_HANGUL                          = 0x15
const VK_JUNJA                           = 0x17
const VK_FINAL                           = 0x18
const VK_HANJA                           = 0x19
const VK_KANJI                           = 0x19
const VK_ESCAPE                          = 0x1B
const VK_CONVERT                         = 0x1C
const VK_NONCONVERT                      = 0x1D
const VK_ACCEPT                          = 0x1E
const VK_MODECHANGE                      = 0x1F
const VK_SPACE                           = 0x20
const VK_PRIOR                           = 0x21
const VK_NEXT                            = 0x22
const VK_END                             = 0x23
const VK_HOME                            = 0x24
const VK_LEFT                            = 0x25
const VK_UP                              = 0x26
const VK_RIGHT                           = 0x27
const VK_DOWN                            = 0x28
const VK_SELECT                          = 0x29
const VK_PRINT                           = 0x2A
const VK_EXECUTE                         = 0x2B
const VK_SNAPSHOT                        = 0x2C
const VK_INSERT                          = 0x2D
const VK_DELETE                          = 0x2E
const VK_HELP                            = 0x2F
const VK_LWIN                            = 0x5B
const VK_RWIN                            = 0x5C
const VK_APPS                            = 0x5D
const VK_SLEEP                           = 0x5F
const VK_NUMPAD0                         = 0x60
const VK_NUMPAD1                         = 0x61
const VK_NUMPAD2                         = 0x62
const VK_NUMPAD3                         = 0x63
const VK_NUMPAD4                         = 0x64
const VK_NUMPAD5                         = 0x65
const VK_NUMPAD6                         = 0x66
const VK_NUMPAD7                         = 0x67
const VK_NUMPAD8                         = 0x68
const VK_NUMPAD9                         = 0x69
const VK_MULTIPLY                        = 0x6A
const VK_ADD                             = 0x6B
const VK_SEPARATOR                       = 0x6C
const VK_SUBTRACT                        = 0x6D
const VK_DECIMAL                         = 0x6E
const VK_DIVIDE                          = 0x6F
const VK_F1                              = 0x70
const VK_F2                              = 0x71
const VK_F3                              = 0x72
const VK_F4                              = 0x73
const VK_F5                              = 0x74
const VK_F6                              = 0x75
const VK_F7                              = 0x76
const VK_F8                              = 0x77
const VK_F9                              = 0x78
const VK_F10                             = 0x79
const VK_F11                             = 0x7A
const VK_F12                             = 0x7B
const VK_F13                             = 0x7C
const VK_F14                             = 0x7D
const VK_F15                             = 0x7E
const VK_F16                             = 0x7F
const VK_F17                             = 0x80
const VK_F18                             = 0x81
const VK_F19                             = 0x82
const VK_F20                             = 0x83
const VK_F21                             = 0x84
const VK_F22                             = 0x85
const VK_F23                             = 0x86
const VK_F24                             = 0x87
const VK_NAVIGATION_VIEW                 = 0x88
const VK_NAVIGATION_MENU                 = 0x89
const VK_NAVIGATION_UP                   = 0x8A
const VK_NAVIGATION_DOWN                 = 0x8B
const VK_NAVIGATION_LEFT                 = 0x8C
const VK_NAVIGATION_RIGHT                = 0x8D
const VK_NAVIGATION_ACCEPT               = 0x8E
const VK_NAVIGATION_CANCEL               = 0x8F
const VK_NUMLOCK                         = 0x90
const VK_SCROLL                          = 0x91
const VK_OEM_NEC_EQUAL                   = 0x92
const VK_OEM_FJ_JISHO                    = 0x92
const VK_OEM_FJ_MASSHOU                  = 0x93
const VK_OEM_FJ_TOUROKU                  = 0x94
const VK_OEM_FJ_LOYA                     = 0x95
const VK_OEM_FJ_ROYA                     = 0x96
const VK_LSHIFT                          = 0xA0
const VK_RSHIFT                          = 0xA1
const VK_LCONTROL                        = 0xA2
const VK_RCONTROL                        = 0xA3
const VK_LMENU                           = 0xA4
const VK_RMENU                           = 0xA5
const VK_BROWSER_BACK                    = 0xA6
const VK_BROWSER_FORWARD                 = 0xA7
const VK_BROWSER_REFRESH                 = 0xA8
const VK_BROWSER_STOP                    = 0xA9
const VK_BROWSER_SEARCH                  = 0xAA
const VK_BROWSER_FAVORITES               = 0xAB
const VK_BROWSER_HOME                    = 0xAC
const VK_VOLUME_MUTE                     = 0xAD
const VK_VOLUME_DOWN                     = 0xAE
const VK_VOLUME_UP                       = 0xAF
const VK_MEDIA_NEXT_TRACK                = 0xB0
const VK_MEDIA_PREV_TRACK                = 0xB1
const VK_MEDIA_STOP                      = 0xB2
const VK_MEDIA_PLAY_PAUSE                = 0xB3
const VK_LAUNCH_MAIL                     = 0xB4
const VK_LAUNCH_MEDIA_SELECT             = 0xB5
const VK_LAUNCH_APP1                     = 0xB6
const VK_LAUNCH_APP2                     = 0xB7
const VK_OEM_1                           = 0xBA
const VK_OEM_PLUS                        = 0xBB
const VK_OEM_COMMA                       = 0xBC
const VK_OEM_MINUS                       = 0xBD
const VK_OEM_PERIOD                      = 0xBE
const VK_OEM_2                           = 0xBF
const VK_OEM_3                           = 0xC0
const VK_GAMEPAD_A                       = 0xC3
const VK_GAMEPAD_B                       = 0xC4
const VK_GAMEPAD_X                       = 0xC5
const VK_GAMEPAD_Y                       = 0xC6
const VK_GAMEPAD_RIGHT_SHOULDER          = 0xC7
const VK_GAMEPAD_LEFT_SHOULDER           = 0xC8
const VK_GAMEPAD_LEFT_TRIGGER            = 0xC9
const VK_GAMEPAD_RIGHT_TRIGGER           = 0xCA
const VK_GAMEPAD_DPAD_UP                 = 0xCB
const VK_GAMEPAD_DPAD_DOWN               = 0xCC
const VK_GAMEPAD_DPAD_LEFT               = 0xCD
const VK_GAMEPAD_DPAD_RIGHT              = 0xCE
const VK_GAMEPAD_MENU                    = 0xCF
const VK_GAMEPAD_VIEW                    = 0xD0
const VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON  = 0xD1
const VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON = 0xD2
const VK_GAMEPAD_LEFT_THUMBSTICK_UP      = 0xD3
const VK_GAMEPAD_LEFT_THUMBSTICK_DOWN    = 0xD4
const VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT   = 0xD5
const VK_GAMEPAD_LEFT_THUMBSTICK_LEFT    = 0xD6
const VK_GAMEPAD_RIGHT_THUMBSTICK_UP     = 0xD7
const VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN   = 0xD8
const VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT  = 0xD9
const VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT   = 0xDA
const VK_OEM_4                           = 0xDB
const VK_OEM_5                           = 0xDC
const VK_OEM_6                           = 0xDD
const VK_OEM_7                           = 0xDE
const VK_OEM_8                           = 0xDF
const VK_OEM_AX                          = 0xE1
const VK_OEM_102                         = 0xE2
const VK_ICO_HELP                        = 0xE3
const VK_ICO_00                          = 0xE4
const VK_PROCESSKEY                      = 0xE5
const VK_ICO_CLEAR                       = 0xE6
const VK_PACKET                          = 0xE7
const VK_OEM_RESET                       = 0xE9
const VK_OEM_JUMP                        = 0xEA
const VK_OEM_PA1                         = 0xEB
const VK_OEM_PA2                         = 0xEC
const VK_OEM_PA3                         = 0xED
const VK_OEM_WSCTRL                      = 0xEE
const VK_OEM_CUSEL                       = 0xEF
const VK_OEM_ATTN                        = 0xF0
const VK_OEM_FINISH                      = 0xF1
const VK_OEM_COPY                        = 0xF2
const VK_OEM_AUTO                        = 0xF3
const VK_OEM_ENLW                        = 0xF4
const VK_OEM_BACKTAB                     = 0xF5
const VK_ATTN                            = 0xF6
const VK_CRSEL                           = 0xF7
const VK_EXSEL                           = 0xF8
const VK_EREOF                           = 0xF9
const VK_PLAY                            = 0xFA
const VK_ZOOM                            = 0xFB
const VK_NONAME                          = 0xFC
const VK_PA1                             = 0xFD
const VK_OEM_CLEAR                       = 0xFE

const MB_OK                   = 0x00000000
const MB_OKCANCEL             = 0x00000001
const MB_ABORTRETRYIGNORE     = 0x00000002
const MB_YESNOCANCEL          = 0x00000003
const MB_YESNO                = 0x00000004
const MB_RETRYCANCEL          = 0x00000005
const MB_CANCELTRYCONTINUE    = 0x00000006
const MB_ICONHAND             = 0x00000010
const MB_ICONQUESTION         = 0x00000020
const MB_ICONEXCLAMATION      = 0x00000030
const MB_ICONASTERISK         = 0x00000040
const MB_USERICON             = 0x00000080
const MB_ICONWARNING          = MB_ICONEXCLAMATION
const MB_ICONERROR            = MB_ICONHAND
const MB_ICONINFORMATION      = MB_ICONASTERISK
const MB_ICONSTOP             = MB_ICONHAND
const MB_DEFBUTTON1           = 0x00000000
const MB_DEFBUTTON2           = 0x00000100
const MB_DEFBUTTON3           = 0x00000200
const MB_DEFBUTTON4           = 0x00000300
const MB_APPLMODAL            = 0x00000000
const MB_SYSTEMMODAL          = 0x00001000
const MB_TASKMODAL            = 0x00002000
const MB_HELP                 = 0x00004000
const MB_NOFOCUS              = 0x00008000
const MB_SETFOREGROUND        = 0x00010000
const MB_DEFAULT_DESKTOP_ONLY = 0x00020000
const MB_TOPMOST              = 0x00040000
const MB_RIGHT                = 0x00080000
const MB_RTLREADING           = 0x00100000

const IDOK       = 1
const IDCANCEL   = 2
const IDABORT    = 3
const IDRETRY    = 4
const IDIGNORE   = 5
const IDYES      = 6
const IDNO       = 7
const IDCLOSE    = 8
const IDHELP     = 9
const IDTRYAGAIN = 10
const IDCONTINUE = 11
const IDTIMEOUT  = 32000

struct WNDCLASSA
{
    style:             UINT
    lpfnWndProc:       const *void
    cbClsExtra:        s32
    cbWndExtra:        s32
    hInstance:         HINSTANCE
    hIcon:             HICON
    hCursor:           HCURSOR
    hbrBackground:     HBRUSH
    lpszMenuName:      LPCSTR
    lpszClassName:     LPCSTR
}

struct WNDCLASSW
{
    style:             UINT
    lpfnWndProc:       const *void
    cbClsExtra:        s32
    cbWndExtra:        s32
    hInstance:         HINSTANCE
    hIcon:             HICON
    hCursor:           HCURSOR
    hbrBackground:     HBRUSH
    lpszMenuName:      LPCWSTR
    lpszClassName:     LPCWSTR
}

struct CREATESTRUCTA
{
    lpCreateParams:     LPVOID
    hInstance:          HINSTANCE
    hMenu:              HMENU
    hwndParent:         HWND
    cy:                 s32
    cx:                 s32
    y:                  s32
    x:                  s32
    style:              LONG
    lpszName:           LPCSTR
    lpszClass:          LPCSTR
    dwExStyle:          DWORD
}

struct CREATESTRUCTW
{
    lpCreateParams:     LPVOID
    hInstance:          HINSTANCE
    hMenu:              HMENU
    hwndParent:         HWND
    cy:                 s32
    cx:                 s32
    y:                  s32
    x:                  s32
    style:              LONG
    lpszName:           LPCWSTR
    lpszClass:          LPCWSTR
    dwExStyle:          DWORD
}

struct WINDOWPOS
{
    hwnd:                HWND
    hwndInsertAfter:     HWND
    x:                   s32
    y:                   s32
    cx:                  s32
    cy:                  s32
    flags:               UINT
}

struct MINMAXINFO
{
    ptReserved:         POINT
    ptMaxSize:          POINT
    ptMaxPosition:      POINT
    ptMinTrackSize:     POINT
    ptMaxTrackSize:     POINT
}

const WS_OVERLAPPED   = 0x00000000
const WS_POPUP        = 0x80000000
const WS_CHILD        = 0x40000000
const WS_MINIMIZE     = 0x20000000
const WS_VISIBLE      = 0x10000000
const WS_DISABLED     = 0x08000000
const WS_CLIPSIBLINGS = 0x04000000
const WS_CLIPCHILDREN = 0x02000000
const WS_MAXIMIZE     = 0x01000000
const WS_CAPTION      = 0x00C00000
const WS_BORDER       = 0x00800000
const WS_DLGFRAME     = 0x00400000
const WS_VSCROLL      = 0x00200000
const WS_HSCROLL      = 0x00100000
const WS_SYSMENU      = 0x00080000
const WS_THICKFRAME   = 0x00040000
const WS_GROUP        = 0x00020000
const WS_TABSTOP      = 0x00010000
const WS_MINIMIZEBOX  = 0x00020000
const WS_MAXIMIZEBOX  = 0x00010000

const WS_EX_DLGMODALFRAME       = 0x00000001
const WS_EX_NOPARENTNOTIFY      = 0x00000004
const WS_EX_TOPMOST             = 0x00000008
const WS_EX_ACCEPTFILES         = 0x00000010
const WS_EX_TRANSPARENT         = 0x00000020
const WS_EX_MDICHILD            = 0x00000040
const WS_EX_TOOLWINDOW          = 0x00000080
const WS_EX_WINDOWEDGE          = 0x00000100
const WS_EX_CLIENTEDGE          = 0x00000200
const WS_EX_CONTEXTHELP         = 0x00000400
const WS_EX_RIGHT               = 0x00001000
const WS_EX_LEFT                = 0x00000000
const WS_EX_RTLREADING          = 0x00002000
const WS_EX_LTRREADING          = 0x00000000
const WS_EX_LEFTSCROLLBAR       = 0x00004000
const WS_EX_RIGHTSCROLLBAR      = 0x00000000
const WS_EX_CONTROLPARENT       = 0x00010000
const WS_EX_STATICEDGE          = 0x00020000
const WS_EX_APPWINDOW           = 0x00040000
const WS_EX_LAYERED             = 0x00080000
const WS_EX_NOINHERITLAYOUT     = 0x00100000
const WS_EX_NOREDIRECTIONBITMAP = 0x00200000
const WS_EX_LAYOUTRTL           = 0x00400000
const WS_EX_COMPOSITED          = 0x02000000
const WS_EX_NOACTIVATE          = 0x08000000

const WS_OVERLAPPEDWINDOW    = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
const WS_POPUPWINDOW         = WS_POPUP | WS_BORDER | WS_SYSMENU
const WS_EX_OVERLAPPEDWINDOW = WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE
const WS_EX_PALETTEWINDOW    = WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST

const CW_USEDEFAULT = -1

const SW_HIDE            = 0
const SW_SHOWNORMAL      = 1
const SW_NORMAL          = 1
const SW_SHOWMINIMIZED   = 2
const SW_SHOWMAXIMIZED   = 3
const SW_MAXIMIZE        = 3
const SW_SHOWNOACTIVATE  = 4
const SW_SHOW            = 5
const SW_MINIMIZE        = 6
const SW_SHOWMINNOACTIVE = 7
const SW_SHOWNA          = 8
const SW_RESTORE         = 9
const SW_SHOWDEFAULT     = 10
const SW_FORCEMINIMIZE   = 11
const SW_MAX             = 11

func MAKEINTRESOURCEA(val: s32) => cast(LPCSTR) cast(u64) val
func MAKEINTRESOURCEW(val: s32) => cast(LPCWSTR) cast(u64) val

const WHEEL_DELTA = 120
func GET_WHEEL_DELTA_WPARAM(wParam: u64) => HIWORD(wParam)

const IDC_ARROW       = 32512
const IDC_IBEAM       = 32513
const IDC_WAIT        = 32514
const IDC_CROSS       = 32515
const IDC_UPARROW     = 32516
const IDC_SIZE        = 32640
const IDC_ICON        = 32641
const IDC_SIZENWSE    = 32642
const IDC_SIZENESW    = 32643
const IDC_SIZEWE      = 32644
const IDC_SIZENS      = 32645
const IDC_SIZEALL     = 32646
const IDC_NO          = 32648
const IDC_HAND        = 32649
const IDC_APPSTARTING = 32650
const IDC_HELP        = 32651
const IDC_PIN         = 32671
const IDC_PERSON      = 32672

struct MSG
{
    hwnd:         HWND
    message:      UINT
    wParam:       WPARAM
    lParam:       LPARAM
    time:         DWORD
    pt:           POINT
    lPrivate:     DWORD
}

const WA_INACTIVE    = 0
const WA_ACTIVE      = 1
const WA_CLICKACTIVE = 2

const WM_CREATE                         = 0x0001
const WM_DESTROY                        = 0x0002
const WM_MOVE                           = 0x0003
const WM_SIZE                           = 0x0005
const WM_ACTIVATE                       = 0x0006
const WM_SETFOCUS                       = 0x0007
const WM_KILLFOCUS                      = 0x0008
const WM_ENABLE                         = 0x000A
const WM_SETREDRAW                      = 0x000B
const WM_SETTEXT                        = 0x000C
const WM_GETTEXT                        = 0x000D
const WM_GETTEXTLENGTH                  = 0x000E
const WM_PAINT                          = 0x000F
const WM_CLOSE                          = 0x0010
const WM_QUIT                           = 0x0012
const WM_ERASEBKGND                     = 0x0014
const WM_SYSCOLORCHANGE                 = 0x0015
const WM_SHOWWINDOW                     = 0x0018
const WM_WININICHANGE                   = 0x001A
const WM_SETTINGCHANGE                  = WM_WININICHANGE
const WM_DEVMODECHANGE                  = 0x001B
const WM_ACTIVATEAPP                    = 0x001C
const WM_FONTCHANGE                     = 0x001D
const WM_TIMECHANGE                     = 0x001E
const WM_CANCELMODE                     = 0x001F
const WM_SETCURSOR                      = 0x0020
const WM_MOUSEACTIVATE                  = 0x0021
const WM_CHILDACTIVATE                  = 0x0022
const WM_QUEUESYNC                      = 0x0023
const WM_GETMINMAXINFO                  = 0x0024
const WM_PAINTICON                      = 0x0026
const WM_ICONERASEBKGND                 = 0x0027
const WM_NEXTDLGCTL                     = 0x0028
const WM_SPOOLERSTATUS                  = 0x002A
const WM_DRAWITEM                       = 0x002B
const WM_MEASUREITEM                    = 0x002C
const WM_DELETEITEM                     = 0x002D
const WM_VKEYTOITEM                     = 0x002E
const WM_CHARTOITEM                     = 0x002F
const WM_SETFONT                        = 0x0030
const WM_GETFONT                        = 0x0031
const WM_SETHOTKEY                      = 0x0032
const WM_GETHOTKEY                      = 0x0033
const WM_QUERYDRAGICON                  = 0x0037
const WM_COMPAREITEM                    = 0x0039
const WM_GETOBJECT                      = 0x003D
const WM_COMPACTING                     = 0x0041
const WM_COMMNOTIFY                     = 0x0044
const WM_WINDOWPOSCHANGING              = 0x0046
const WM_WINDOWPOSCHANGED               = 0x0047
const WM_POWER                          = 0x0048
const WM_COPYDATA                       = 0x004A
const WM_CANCELJOURNAL                  = 0x004B
const WM_NOTIFY                         = 0x004E
const WM_INPUTLANGCHANGEREQUEST         = 0x0050
const WM_INPUTLANGCHANGE                = 0x0051
const WM_TCARD                          = 0x0052
const WM_HELP                           = 0x0053
const WM_USERCHANGED                    = 0x0054
const WM_NOTIFYFORMAT                   = 0x0055
const WM_CONTEXTMENU                    = 0x007B
const WM_STYLECHANGING                  = 0x007C
const WM_STYLECHANGED                   = 0x007D
const WM_DISPLAYCHANGE                  = 0x007E
const WM_GETICON                        = 0x007F
const WM_SETICON                        = 0x0080
const WM_NCCREATE                       = 0x0081
const WM_NCDESTROY                      = 0x0082
const WM_NCCALCSIZE                     = 0x0083
const WM_NCHITTEST                      = 0x0084
const WM_NCPAINT                        = 0x0085
const WM_NCACTIVATE                     = 0x0086
const WM_GETDLGCODE                     = 0x0087
const WM_SYNCPAINT                      = 0x0088
const WM_NCMOUSEMOVE                    = 0x00A0
const WM_NCLBUTTONDOWN                  = 0x00A1
const WM_NCLBUTTONUP                    = 0x00A2
const WM_NCLBUTTONDBLCLK                = 0x00A3
const WM_NCRBUTTONDOWN                  = 0x00A4
const WM_NCRBUTTONUP                    = 0x00A5
const WM_NCRBUTTONDBLCLK                = 0x00A6
const WM_NCMBUTTONDOWN                  = 0x00A7
const WM_NCMBUTTONUP                    = 0x00A8
const WM_NCMBUTTONDBLCLK                = 0x00A9
const WM_NCXBUTTONDOWN                  = 0x00AB
const WM_NCXBUTTONUP                    = 0x00AC
const WM_NCXBUTTONDBLCLK                = 0x00AD
const WM_INPUT_DEVICE_CHANGE            = 0x00FE
const WM_INPUT                          = 0x00FF
const WM_KEYFIRST                       = 0x0100
const WM_KEYDOWN                        = 0x0100
const WM_KEYUP                          = 0x0101
const WM_CHAR                           = 0x0102
const WM_DEADCHAR                       = 0x0103
const WM_SYSKEYDOWN                     = 0x0104
const WM_SYSKEYUP                       = 0x0105
const WM_SYSCHAR                        = 0x0106
const WM_SYSDEADCHAR                    = 0x0107
const WM_UNICHAR                        = 0x0109
const WM_KEYLAST                        = 0x0109
const WM_IME_STARTCOMPOSITION           = 0x010D
const WM_IME_ENDCOMPOSITION             = 0x010E
const WM_IME_COMPOSITION                = 0x010F
const WM_IME_KEYLAST                    = 0x010F
const WM_INITDIALOG                     = 0x0110
const WM_COMMAND                        = 0x0111
const WM_SYSCOMMAND                     = 0x0112
const WM_TIMER                          = 0x0113
const WM_HSCROLL                        = 0x0114
const WM_VSCROLL                        = 0x0115
const WM_INITMENU                       = 0x0116
const WM_INITMENUPOPUP                  = 0x0117
const WM_GESTURE                        = 0x0119
const WM_GESTURENOTIFY                  = 0x011A
const WM_MENUSELECT                     = 0x011F
const WM_MENUCHAR                       = 0x0120
const WM_ENTERIDLE                      = 0x0121
const WM_MENURBUTTONUP                  = 0x0122
const WM_MENUDRAG                       = 0x0123
const WM_MENUGETOBJECT                  = 0x0124
const WM_UNINITMENUPOPUP                = 0x0125
const WM_MENUCOMMAND                    = 0x0126
const WM_CHANGEUISTATE                  = 0x0127
const WM_UPDATEUISTATE                  = 0x0128
const WM_QUERYUISTATE                   = 0x0129
const WM_CTLCOLORMSGBOX                 = 0x0132
const WM_CTLCOLOREDIT                   = 0x0133
const WM_CTLCOLORLISTBOX                = 0x0134
const WM_CTLCOLORBTN                    = 0x0135
const WM_CTLCOLORDLG                    = 0x0136
const WM_CTLCOLORSCROLLBAR              = 0x0137
const WM_CTLCOLORSTATIC                 = 0x0138
const MN_GETHMENU                       = 0x01E1
const WM_MOUSEFIRST                     = 0x0200
const WM_MOUSEMOVE                      = 0x0200
const WM_LBUTTONDOWN                    = 0x0201
const WM_LBUTTONUP                      = 0x0202
const WM_LBUTTONDBLCLK                  = 0x0203
const WM_RBUTTONDOWN                    = 0x0204
const WM_RBUTTONUP                      = 0x0205
const WM_RBUTTONDBLCLK                  = 0x0206
const WM_MBUTTONDOWN                    = 0x0207
const WM_MBUTTONUP                      = 0x0208
const WM_MBUTTONDBLCLK                  = 0x0209
const WM_MOUSEWHEEL                     = 0x020A
const WM_XBUTTONDOWN                    = 0x020B
const WM_XBUTTONUP                      = 0x020C
const WM_XBUTTONDBLCLK                  = 0x020D
const WM_MOUSEHWHEEL                    = 0x020E
const WM_MOUSELAST                      = 0x020E
const WM_PARENTNOTIFY                   = 0x0210
const WM_ENTERMENULOOP                  = 0x0211
const WM_EXITMENULOOP                   = 0x0212
const WM_NEXTMENU                       = 0x0213
const WM_SIZING                         = 0x0214
const WM_CAPTURECHANGED                 = 0x0215
const WM_MOVING                         = 0x0216
const WM_POWERBROADCAST                 = 0x0218
const WM_DEVICECHANGE                   = 0x0219
const WM_MDICREATE                      = 0x0220
const WM_MDIDESTROY                     = 0x0221
const WM_MDIACTIVATE                    = 0x0222
const WM_MDIRESTORE                     = 0x0223
const WM_MDINEXT                        = 0x0224
const WM_MDIMAXIMIZE                    = 0x0225
const WM_MDITILE                        = 0x0226
const WM_MDICASCADE                     = 0x0227
const WM_MDIICONARRANGE                 = 0x0228
const WM_MDIGETACTIVE                   = 0x0229
const WM_MDISETMENU                     = 0x0230
const WM_ENTERSIZEMOVE                  = 0x0231
const WM_EXITSIZEMOVE                   = 0x0232
const WM_DROPFILES                      = 0x0233
const WM_MDIREFRESHMENU                 = 0x0234
const WM_POINTERDEVICECHANGE            = 0x238
const WM_POINTERDEVICEINRANGE           = 0x239
const WM_POINTERDEVICEOUTOFRANGE        = 0x23A
const WM_TOUCH                          = 0x0240
const WM_NCPOINTERUPDATE                = 0x0241
const WM_NCPOINTERDOWN                  = 0x0242
const WM_NCPOINTERUP                    = 0x0243
const WM_POINTERUPDATE                  = 0x0245
const WM_POINTERDOWN                    = 0x0246
const WM_POINTERUP                      = 0x0247
const WM_POINTERENTER                   = 0x0249
const WM_POINTERLEAVE                   = 0x024A
const WM_POINTERACTIVATE                = 0x024B
const WM_POINTERCAPTURECHANGED          = 0x024C
const WM_TOUCHHITTESTING                = 0x024D
const WM_POINTERWHEEL                   = 0x024E
const WM_POINTERHWHEEL                  = 0x024F
const DM_POINTERHITTEST                 = 0x0250
const WM_POINTERROUTEDTO                = 0x0251
const WM_POINTERROUTEDAWAY              = 0x0252
const WM_POINTERROUTEDRELEASED          = 0x0253
const WM_IME_SETCONTEXT                 = 0x0281
const WM_IME_NOTIFY                     = 0x0282
const WM_IME_CONTROL                    = 0x0283
const WM_IME_COMPOSITIONFULL            = 0x0284
const WM_IME_SELECT                     = 0x0285
const WM_IME_CHAR                       = 0x0286
const WM_IME_REQUEST                    = 0x0288
const WM_IME_KEYDOWN                    = 0x0290
const WM_IME_KEYUP                      = 0x0291
const WM_MOUSEHOVER                     = 0x02A1
const WM_MOUSELEAVE                     = 0x02A3
const WM_NCMOUSEHOVER                   = 0x02A0
const WM_NCMOUSELEAVE                   = 0x02A2
const WM_WTSSESSION_CHANGE              = 0x02B1
const WM_TABLET_FIRST                   = 0x02c0
const WM_TABLET_LAST                    = 0x02df
const WM_DPICHANGED                     = 0x02E0
const WM_DPICHANGED_BEFOREPARENT        = 0x02E2
const WM_DPICHANGED_AFTERPARENT         = 0x02E3
const WM_GETDPISCALEDSIZE               = 0x02E4
const WM_CUT                            = 0x0300
const WM_COPY                           = 0x0301
const WM_PASTE                          = 0x0302
const WM_CLEAR                          = 0x0303
const WM_UNDO                           = 0x0304
const WM_RENDERFORMAT                   = 0x0305
const WM_RENDERALLFORMATS               = 0x0306
const WM_DESTROYCLIPBOARD               = 0x0307
const WM_DRAWCLIPBOARD                  = 0x0308
const WM_PAINTCLIPBOARD                 = 0x0309
const WM_VSCROLLCLIPBOARD               = 0x030A
const WM_SIZECLIPBOARD                  = 0x030B
const WM_ASKCBFORMATNAME                = 0x030C
const WM_CHANGECBCHAIN                  = 0x030D
const WM_HSCROLLCLIPBOARD               = 0x030E
const WM_QUERYNEWPALETTE                = 0x030F
const WM_PALETTEISCHANGING              = 0x0310
const WM_PALETTECHANGED                 = 0x0311
const WM_HOTKEY                         = 0x0312
const WM_PRINT                          = 0x0317
const WM_PRINTCLIENT                    = 0x0318
const WM_APPCOMMAND                     = 0x0319
const WM_THEMECHANGED                   = 0x031A
const WM_CLIPBOARDUPDATE                = 0x031D
const WM_DWMCOMPOSITIONCHANGED          = 0x031E
const WM_DWMNCRENDERINGCHANGED          = 0x031F
const WM_DWMCOLORIZATIONCOLORCHANGED    = 0x0320
const WM_DWMWINDOWMAXIMIZEDCHANGE       = 0x0321
const WM_DWMSENDICONICTHUMBNAIL         = 0x0323
const WM_DWMSENDICONICLIVEPREVIEWBITMAP = 0x0326
const WM_GETTITLEBARINFOEX              = 0x033F
const WM_HANDHELDFIRST                  = 0x0358
const WM_HANDHELDLAST                   = 0x035F
const WM_AFXFIRST                       = 0x0360
const WM_AFXLAST                        = 0x037F
const WM_PENWINFIRST                    = 0x0380
const WM_PENWINLAST                     = 0x038F
const WM_APP                            = 0x8000
const WM_USER                           = 0x0400

const PM_NOREMOVE = 0x0000
const PM_REMOVE   = 0x0001
const PM_NOYIELD  = 0x0002

const CS_VREDRAW         = 0x0001
const CS_HREDRAW         = 0x0002
const CS_DBLCLKS         = 0x0008
const CS_OWNDC           = 0x0020
const CS_CLASSDC         = 0x0040
const CS_PARENTDC        = 0x0080
const CS_NOCLOSE         = 0x0200
const CS_SAVEBITS        = 0x0800
const CS_BYTEALIGNCLIENT = 0x1000
const CS_BYTEALIGNWINDOW = 0x2000
const CS_GLOBALCLASS     = 0x4000
const CS_IME             = 0x00010000
const CS_DROPSHADOW      = 0x00020000

const GWLP_WNDPROC    = -4
const GWLP_HINSTANCE  = -6
const GWLP_HWNDPARENT = -8
const GWLP_USERDATA   = -21
const GWLP_ID         = -12

const GWL_EXSTYLE = -20

const HWND_TOP       = cast(HWND) 0'u64
const HWND_BOTTOM    = cast(HWND) 1'u64
const HWND_TOPMOST   = cast(HWND) cast #bit (u64) -1's64
const HWND_NOTOPMOST = cast(HWND) cast #bit (u64) -2's64

const SWP_NOSIZE         = 0x0001
const SWP_NOMOVE         = 0x0002
const SWP_NOZORDER       = 0x0004
const SWP_NOREDRAW       = 0x0008
const SWP_NOACTIVATE     = 0x0010
const SWP_FRAMECHANGED   = 0x0020
const SWP_SHOWWINDOW     = 0x0040
const SWP_HIDEWINDOW     = 0x0080
const SWP_NOCOPYBITS     = 0x0100
const SWP_NOOWNERZORDER  = 0x0200
const SWP_NOSENDCHANGING = 0x0400
const SWP_DRAWFRAME      = SWP_FRAMECHANGED
const SWP_NOREPOSITION   = SWP_NOOWNERZORDER
const SWP_DEFERERASE     = 0x2000
const SWP_ASYNCWINDOWPOS = 0x4000

const SC_SIZE         = 0xF000
const SC_MOVE         = 0xF010
const SC_MINIMIZE     = 0xF020
const SC_MAXIMIZE     = 0xF030
const SC_NEXTWINDOW   = 0xF040
const SC_PREVWINDOW   = 0xF050
const SC_CLOSE        = 0xF060
const SC_VSCROLL      = 0xF070
const SC_HSCROLL      = 0xF080
const SC_MOUSEMENU    = 0xF090
const SC_KEYMENU      = 0xF100
const SC_ARRANGE      = 0xF110
const SC_RESTORE      = 0xF120
const SC_TASKLIST     = 0xF130
const SC_SCREENSAVE   = 0xF140
const SC_HOTKEY       = 0xF150
const SC_DEFAULT      = 0xF160
const SC_MONITORPOWER = 0xF170
const SC_CONTEXTHELP  = 0xF180
const SC_SEPARATOR    = 0xF00F

const MAPVK_VK_TO_VSC    = 0'u32
const MAPVK_VSC_TO_VK    = 1'u32
const MAPVK_VK_TO_CHAR   = 2'u32
const MAPVK_VSC_TO_VK_EX = 3'u32
const MAPVK_VK_TO_VSC_EX = 4'u32

const CF_TEXT         = 1'u32
const CF_BITMAP       = 2'u32
const CF_METAFILEPICT = 3'u32
const CF_SYLK         = 4'u32
const CF_DIF          = 5'u32
const CF_TIFF         = 6'u32
const CF_OEMTEXT      = 7'u32
const CF_DIB          = 8'u32
const CF_PALETTE      = 9'u32
const CF_PENDATA      = 10'u32
const CF_RIFF         = 11'u32
const CF_WAVE         = 12'u32
const CF_UNICODETEXT  = 13'u32
const CF_ENHMETAFILE  = 14'u32
const CF_HDROP        = 15'u32
const CF_LOCALE       = 16'u32
const CF_DIBV5        = 17'u32

struct RECT
{
    left:       LONG
    top:        LONG
    right:      LONG
    bottom:     LONG
}

struct PAINTSTRUCT
{
    hdc:             HDC
    fErase:          BOOL
    rcPaint:         RECT
    fRestore:        BOOL
    fIncUpdate:      BOOL
    rgbReserved:     [32] BYTE
}

const MONITOR_DEFAULTTONULL    = 0x00000000
const MONITOR_DEFAULTTOPRIMARY = 0x00000001
const MONITOR_DEFAULTTONEAREST = 0x00000002

struct MONITORINFO
{
    cbSize:        DWORD
    rcMonitor:     RECT
    rcWork:        RECT
    dwFlags:       DWORD
}

const SM_CXSCREEN                    = 0
const SM_CYSCREEN                    = 1
const SM_CXVSCROLL                   = 2
const SM_CYHSCROLL                   = 3
const SM_CYCAPTION                   = 4
const SM_CXBORDER                    = 5
const SM_CYBORDER                    = 6
const SM_CXDLGFRAME                  = 7
const SM_CYDLGFRAME                  = 8
const SM_CYVTHUMB                    = 9
const SM_CXHTHUMB                    = 10
const SM_CXICON                      = 11
const SM_CYICON                      = 12
const SM_CXCURSOR                    = 13
const SM_CYCURSOR                    = 14
const SM_CYMENU                      = 15
const SM_CXFULLSCREEN                = 16
const SM_CYFULLSCREEN                = 17
const SM_CYKANJIWINDOW               = 18
const SM_MOUSEPRESENT                = 19
const SM_CYVSCROLL                   = 20
const SM_CXHSCROLL                   = 21
const SM_DEBUG                       = 22
const SM_SWAPBUTTON                  = 23
const SM_RESERVED1                   = 24
const SM_RESERVED2                   = 25
const SM_RESERVED3                   = 26
const SM_RESERVED4                   = 27
const SM_CXMIN                       = 28
const SM_CYMIN                       = 29
const SM_CXSIZE                      = 30
const SM_CYSIZE                      = 31
const SM_CXFRAME                     = 32
const SM_CYFRAME                     = 33
const SM_CXMINTRACK                  = 34
const SM_CYMINTRACK                  = 35
const SM_CXDOUBLECLK                 = 36
const SM_CYDOUBLECLK                 = 37
const SM_CXICONSPACING               = 38
const SM_CYICONSPACING               = 39
const SM_MENUDROPALIGNMENT           = 40
const SM_PENWINDOWS                  = 41
const SM_DBCSENABLED                 = 42
const SM_CMOUSEBUTTONS               = 43
const SM_CXFIXEDFRAME                = SM_CXDLGFRAME
const SM_CYFIXEDFRAME                = SM_CYDLGFRAME
const SM_CXSIZEFRAME                 = SM_CXFRAME
const SM_CYSIZEFRAME                 = SM_CYFRAME
const SM_SECURE                      = 44
const SM_CXEDGE                      = 45
const SM_CYEDGE                      = 46
const SM_CXMINSPACING                = 47
const SM_CYMINSPACING                = 48
const SM_CXSMICON                    = 49
const SM_CYSMICON                    = 50
const SM_CYSMCAPTION                 = 51
const SM_CXSMSIZE                    = 52
const SM_CYSMSIZE                    = 53
const SM_CXMENUSIZE                  = 54
const SM_CYMENUSIZE                  = 55
const SM_ARRANGE                     = 56
const SM_CXMINIMIZED                 = 57
const SM_CYMINIMIZED                 = 58
const SM_CXMAXTRACK                  = 59
const SM_CYMAXTRACK                  = 60
const SM_CXMAXIMIZED                 = 61
const SM_CYMAXIMIZED                 = 62
const SM_NETWORK                     = 63
const SM_CLEANBOOT                   = 67
const SM_CXDRAG                      = 68
const SM_CYDRAG                      = 69
const SM_SHOWSOUNDS                  = 70
const SM_CXMENUCHECK                 = 71
const SM_CYMENUCHECK                 = 72
const SM_SLOWMACHINE                 = 73
const SM_MIDEASTENABLED              = 74
const SM_MOUSEWHEELPRESENT           = 75
const SM_XVIRTUALSCREEN              = 76
const SM_YVIRTUALSCREEN              = 77
const SM_CXVIRTUALSCREEN             = 78
const SM_CYVIRTUALSCREEN             = 79
const SM_CMONITORS                   = 80
const SM_SAMEDISPLAYFORMAT           = 81
const SM_IMMENABLED                  = 82
const SM_CXFOCUSBORDER               = 83
const SM_CYFOCUSBORDER               = 84
const SM_TABLETPC                    = 86
const SM_MEDIACENTER                 = 87
const SM_STARTER                     = 88
const SM_SERVERR2                    = 89
const SM_MOUSEHORIZONTALWHEELPRESENT = 91
const SM_CXPADDEDBORDER              = 92
const SM_DIGITIZER                   = 94
const SM_MAXIMUMTOUCHES              = 95
const SM_CMETRICS                    = 97
const SM_REMOTESESSION               = 0x1000
const SM_SHUTTINGDOWN                = 0x2000
const SM_REMOTECONTROL               = 0x2001
const SM_CARETBLINKINGENABLED        = 0x2002
const SM_CONVERTIBLESLATEMODE        = 0x2003
const SM_SYSTEMDOCKED                = 0x2004

const LWA_COLORKEY = 0x00000001
const LWA_ALPHA    = 0x00000002

const GW_HWNDFIRST    = 0'u32
const GW_HWNDLAST     = 1'u32
const GW_HWNDNEXT     = 2'u32
const GW_HWNDPREV     = 3'u32
const GW_OWNER        = 4'u32
const GW_CHILD        = 5'u32
const GW_ENABLEDPOPUP = 6'u32
const GW_MAX          = 6'u32

const CWP_ALL             = 0x0000
const CWP_SKIPINVISIBLE   = 0x0001
const CWP_SKIPDISABLED    = 0x0002
const CWP_SKIPTRANSPARENT = 0x0004

struct ICONINFO
{
    fIcon:        BOOL
    xHotspot:     DWORD
    yHotspot:     DWORD
    hbmMask:      HBITMAP
    hbmColor:     HBITMAP
}

alias DPI_AWARENESS_CONTEXT = *void
const DPI_AWARENESS_CONTEXT_UNAWARE              = cast(DPI_AWARENESS_CONTEXT) cast #bit (u64) -1's64
const DPI_AWARENESS_CONTEXT_SYSTEM_AWARE         = cast(DPI_AWARENESS_CONTEXT) cast #bit (u64) -2's64
const DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE    = cast(DPI_AWARENESS_CONTEXT) cast #bit (u64) -3's64
const DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 = cast(DPI_AWARENESS_CONTEXT) cast #bit (u64) -4's64
const DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED    = cast(DPI_AWARENESS_CONTEXT) cast #bit (u64) -5's64

func GET_X_LPARAM(lp: LPARAM) => cast(s32) LOWORD(lp)
func GET_Y_LPARAM(lp: LPARAM) => cast(s32) HIWORD(lp)

#[Foreign("user32")]
{
    func GetAsyncKeyState(vKey: s32)->SHORT;
    func GetKeyboardState(lpKeystate: *u8)->BOOL;
    func SetCursorPos(x, y: s32);
    func ShowCursor(show: BOOL);
    func LoadCursorA(hInstance: HINSTANCE, lpCursorName: LPCSTR)->HCURSOR;
    func LoadCursorW(hInstance: HINSTANCE, lpCursorName: LPCWSTR)->HCURSOR;

    func CreateWindowExA(dwExStyle: DWORD, lpClassName: LPCSTR, lpWindowName: LPCSTR, dwStyle: DWORD, X: s32, Y: s32, nWidth: s32, nHeight: s32, hWndParent: HWND, hMenu: HMENU, hInstance: HINSTANCE, lpParam: LPVOID)->HWND;
    func CreateWindowExW(dwExStyle: DWORD, lpClassName: LPCWSTR, lpWindowName: LPCWSTR, dwStyle: DWORD, X: s32, Y: s32, nWidth: s32, nHeight: s32, hWndParent: HWND, hMenu: HMENU, hInstance: HINSTANCE, lpParam: LPVOID)->HWND;
    func DefWindowProcA(hwnd: HWND, uMsg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT;
    func DefWindowProcW(hwnd: HWND, uMsg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT;
    func GetMessageA(lpMsg: *MSG, hWnd: HWND, wMsgFilterMin: UINT, wMsgFilterMax: UINT)->BOOL;
    func GetMessageW(lpMsg: *MSG, hWnd: HWND, wMsgFilterMin: UINT, wMsgFilterMax: UINT)->BOOL;
    func PeekMessageA(lpMsg: *MSG, hWnd: HWND, wMsgFilterMin: UINT, wMsgFilterMax: UINT, wRemoveMsg: UINT)->BOOL;
    func PeekMessageW(lpMsg: *MSG, hWnd: HWND, wMsgFilterMin: UINT, wMsgFilterMax: UINT, wRemoveMsg: UINT)->BOOL;
    func PostQuitMessage(nExitCode: s32);
    func SendMessageA(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT;
    func SendMessageW(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->LRESULT;

    func GetWindowLongPtrA(hWnd: HWND, nIndex: s32)->*LONG;
    func GetWindowLongPtrW(hWnd: HWND, nIndex: s32)->*LONG;
    func WindowFromPoint(pt: POINT)->HWND;
    func ChildWindowFromPoint(hWnd: HWND, pt: POINT)->HWND;
    func ChildWindowFromPointEx(hwnd: HWND, pt: POINT, flags: UINT)->HWND;
    func FindWindowA(lpClassName: LPCSTR, lpWindowName: LPCSTR)->HWND;
    func FindWindowW(lpClassName: LPCWSTR, lpWindowName: LPCWSTR)->HWND;
    func GetDesktopWindow()->HWND;
    func GetTopWindow(hWnd: HWND)->HWND;
    func GetActiveWindow()->HWND;
    func GetForegroundWindow()->HWND;
    func GetWindow(hWnd: HWND, uCmd: UINT)->HWND;
    func GetNextWindow(hWnd: HWND, wCmd: UINT);
    func IsWindowVisible(hWnd: HWND)->BOOL;

    func GetDC(hWnd: HWND)->HDC;

    func MapVirtualKeyA(uCode: UINT, uMapType: UINT)->UINT;
    func MapVirtualKeyW(uCode: UINT, uMapType: UINT)->UINT;
    func ToUnicode(wVirtKey, wScanCode: UINT, lpKeyState: const *BYTE, pwszBuff: LPWSTR, cchBuff: s32, wFlags: UINT)->s32;

    func OpenClipboard(hWndNewOwner: HWND)->BOOL;
    func CloseClipboard()->BOOL;
    func EmptyClipboard()->BOOL;
    func IsClipboardFormatAvailable(format: UINT)->BOOL;
    func GetClipboardData(uFormat: UINT)->HANDLE;
    func SetClipboardData(uFormat: UINT, hMem: HANDLE)->HANDLE;

    func MonitorFromPoint(pt: POINT, dwFlags: DWORD)->HMONITOR;
    func MonitorFromWindow(hwnd: HWND, dwFlags: DWORD)->HMONITOR;
    func SetThreadDpiAwarenessContext(dpiContext: DPI_AWARENESS_CONTEXT)->DPI_AWARENESS_CONTEXT;

    func GetSystemMetrics(nIndex: s32)->s32;
}

#[Foreign("user32"), Discardable]
{
    func GetCursorPos(lpPoint: *POINT)->BOOL;

    func FillRect(hDC: HDC, lprc: const *RECT, hbr: HBRUSH)->s32;
    func GetClientRect(hWnd: HWND, lpRect: *RECT)->BOOL;
    func GetWindowRect(hWnd: HWND, lpRect: *RECT)->BOOL;
    func GetWindowTextA(hWnd: HWND, lpString: LPSTR, nMaxCount: s32)->s32;
    func GetWindowTextW(hWnd: HWND, lpString: LPWSTR, nMaxCount: s32)->s32;
    func GetClassNameA(hWnd: HWND, lpClassName: LPSTR, nMaxCount: s32)->s32;
    func GetClassNameW(hWnd: HWND, lpClassName: LPWSTR, nMaxCount: s32)->s32;

    func ShowWindow(hWnd: HWND, nCmdShow: s32)->BOOL;
    func EnableWindow(hWnd: HWND, bEnable: BOOL)->BOOL;
    func MessageBoxA(hWnd: HWND, lpText: LPCSTR, lpCaption: LPCSTR, uType: u32)->s32;
    func MessageBoxW(hWnd: HWND, lpText: LPCWSTR, lpCaption: LPCWSTR, uType: u32)->s32;
    func SetCapture(hWnd: HWND)->HWND;
    func ReleaseCapture()->BOOL;
    func SetCursor(hCursor: HCURSOR)->HCURSOR;
    func SetFocus(hWnd: HWND)->HWND;
    func SetActiveWindow(hWnd: HWND)->HWND;

    func BeginPaint(hWnd: HWND, lpPaint: *PAINTSTRUCT)->HDC;
    func EndPaint(hWnd: HWND, lpPaint: const *PAINTSTRUCT)->BOOL;
    func ReleaseDC(hWnd: HWND, hDC: HDC)->s32;
    func InvalidateRect(hWnd: HWND, lpRect: #null const *RECT, bErase: BOOL)->BOOL;

    func ScreenToClient(hWnd: HWND, lpPoint: *POINT)->BOOL;
    func ClientToScreen(hWnd: HWND, lpPoint: *POINT)->BOOL;
    func BringWindowToTop(hWnd: HWND)->BOOL;

    func TranslateMessage(lpMsg: const *MSG)->BOOL;
    func DispatchMessageA(lpMsg: const *MSG)->LRESULT;
    func DispatchMessageW(lpMsg: const *MSG)->LRESULT;

    func SetWindowLongPtrA(hWnd: HWND, nIndex: s32, dwNewLong: *LONG)->*LONG;
    func SetWindowLongPtrW(hWnd: HWND, nIndex: s32, dwNewLong: *LONG)->*LONG;

    func EnumDisplayMonitors(hdc: HDC, lprcClip: #null *RECT, lpfnEnum: const *void, dwData: LPARAM)->BOOL;
    func EnumWindows(lpEnumFunc: const *void, lParam: LPARAM)->BOOL;
}

private namespace User32Priv
{
    #[Foreign("user32")]
    {
        func RegisterClassA(lpWndClass: const *WNDCLASSA)->ATOM;
        func RegisterClassW(lpWndClass: const *WNDCLASSW)->ATOM;

        func UpdateWindow(hWnd: HWND)->BOOL;
        func DestroyWindow(hWnd: HWND)->BOOL;
        func PostMessageA(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->BOOL;
        func PostMessageW(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM)->BOOL;
        func MoveWindow(hWnd: HWND, x, y, nWidth, nHeight: s32, bRepaint: BOOL)->BOOL;
        func SetWindowPos(hWnd, hWndInsertAfter: HWND, x, y, cx, cy: s32, uFlags: UINT)->BOOL;

        func SetLayeredWindowAttributes(hwnd: HWND, crKey: COLORREF, bAlpha: BYTE, dwFlags: DWORD)->BOOL;

        func GetMonitorInfoA(hMonitor: HMONITOR, lpmi: *MONITORINFO)->BOOL;
        func GetMonitorInfoW(hMonitor: HMONITOR, lpmi: *MONITORINFO)->BOOL;

        func RegisterClipboardFormatA(lpszFormat: LPCSTR)->UINT;
        func RegisterClipboardFormatW(lpszFormat: LPCWSTR)->UINT;

        func DestroyIcon(hIcon: HICON)->BOOL;
        func GetIconInfo(hIcon: HICON, piconinfo: *ICONINFO)->BOOL;
        func CreateIconIndirect(piconinfo: *ICONINFO)->HICON;

        func RegisterHotKey(hWnd: HWND, id: s32, fsModifiers: UINT, vk: UINT)->BOOL;
        func UnregisterHotKey(hWnd: HWND, id: s32)->BOOL;
    }
}

#[Inline]
{
    func RegisterHotKey(hWnd: HWND, id: s32, fsModifiers: UINT, vk: UINT) throw
    {
        if !User32Priv.RegisterHotKey(hWnd, id, fsModifiers, vk) do
            throwWinError(GetLastError())
    }

    func UnregisterHotKey(hWnd: HWND, id: s32) throw
    {
        if !User32Priv.UnregisterHotKey(hWnd, id) do
            throwWinError(GetLastError())
    }

    func PostMessageA(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM) throw
    {
        if !User32Priv.PostMessageA(hWnd, msg, wParam, lParam) do
            throwWinError(GetLastError())
    }

    func PostMessageW(hWnd: HWND, msg: UINT, wParam: WPARAM, lParam: LPARAM) throw
    {
        if !User32Priv.PostMessageW(hWnd, msg, wParam, lParam) do
            throwWinError(GetLastError())
    }

    #[Discardable]
    func RegisterClassA(lpWndClass: const *WNDCLASSA)->ATOM throw
    {
        let res = User32Priv.RegisterClassA(lpWndClass)
        if !res do
            throwWinError(GetLastError())
        return res
    }

    #[Discardable]
    func RegisterClassW(lpWndClass: const *WNDCLASSW)->ATOM throw
    {
        let res = User32Priv.RegisterClassW(lpWndClass)
        if !res do
            throwWinError(GetLastError())
        return res
    }

    func UpdateWindow(hWnd: HWND) throw
    {
        if !User32Priv.UpdateWindow(hWnd) do
            throwWinError(GetLastError())
    }

    func DestroyWindow(hWnd: HWND) throw
    {
        if !User32Priv.DestroyWindow(hWnd) do
            throwWinError(GetLastError())
    }

    func MoveWindow(hWnd: HWND, x, y, nWidth, nHeight: s32, bRepaint: BOOL) throw
    {
        if !User32Priv.MoveWindow(hWnd, x, y, nWidth, nHeight, bRepaint) do
            throwWinError(GetLastError())
    }

    func SetWindowPos(hWnd, hWndInsertAfter: HWND, x, y, cx, cy: s32, uFlags: UINT) throw
    {
        if !User32Priv.SetWindowPos(hWnd, hWndInsertAfter, x, y, cx, cy, uFlags) do
            throwWinError(GetLastError())
    }

    func SetLayeredWindowAttributes(hwnd: HWND, crKey: COLORREF, bAlpha: BYTE, dwFlags: DWORD) throw
    {
        if !User32Priv.SetLayeredWindowAttributes(hwnd, crKey, bAlpha, dwFlags) do
            throwWinError(GetLastError())
    }

    func GetMonitorInfoA(hMonitor: HMONITOR, lpmi: *MONITORINFO) throw
    {
        lpmi.cbSize = #sizeof(MONITORINFO)
        if !User32Priv.GetMonitorInfoA(hMonitor, lpmi) do
            throwWinError(GetLastError())
    }

    func GetMonitorInfoW(hMonitor: HMONITOR, lpmi: *MONITORINFO) throw
    {
        lpmi.cbSize = #sizeof(MONITORINFO)
        if !User32Priv.GetMonitorInfoW(hMonitor, lpmi) do
            throwWinError(GetLastError())
    }

    func RegisterClipboardFormatA(name: LPCSTR)->UINT throw
    {
        let res = User32Priv.RegisterClipboardFormatA(name)
        if !res do
            throwWinError(GetLastError())
        return res
    }

    func RegisterClipboardFormatW(name: LPCWSTR)->UINT throw
    {
        let res = User32Priv.RegisterClipboardFormatW(name)
        if !res do
            throwWinError(GetLastError())
        return res
    }

    func DestroyIcon(hIcon: HICON) throw
    {
        if !User32Priv.DestroyIcon(hIcon) do
            throwWinError(GetLastError())
    }

    func GetIconInfo(hIcon: HICON, piconinfo: *ICONINFO) throw
    {
        if !User32Priv.GetIconInfo(hIcon, piconinfo) do
            throwWinError(GetLastError())
    }

    func CreateIconIndirect(piconinfo: *ICONINFO)->HICON throw
    {
        let h = User32Priv.CreateIconIndirect(piconinfo)
        if !h do
            throwWinError(GetLastError())
        return h
    }
}
