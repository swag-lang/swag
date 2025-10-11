#global namespace Input

// Represents a virtual system key
public alias VirtualKey = u32

#[Swag.EnumFlags]
public enum KeyModifiers
{
    Zero      = 0
    Shift
    Control
    Alt
    CtrlShift = Control | Shift
    CtrlAlt   = Control | Alt
}

// Defines the keys on a keyboard
#[Swag.EnumIndex]
public enum Key
{
    None
    A
    Add
    Apps
    Attn
    B
    Back
    BrowserBack
    BrowserFavorites
    BrowserForward
    BrowserHome
    BrowserRefresh
    BrowserSearch
    BrowserStop
    C
    Cancel
    Capital
    CapsLock
    Clear
    Control
    Crsel
    D
    D0
    D1
    D2
    D3
    D4
    D5
    D6
    D7
    D8
    D9
    Decimal
    Delete
    Divide
    Down
    E
    End
    Enter
    EraseEof
    Escape
    Execute
    Exsel
    F
    F1
    F10
    F11
    F12
    F13
    F14
    F15
    F16
    F17
    F18
    F19
    F2
    F20
    F21
    F22
    F23
    F24
    F3
    F4
    F5
    F6
    F7
    F8
    F9
    FinalMode
    G
    H
    HanguelMode
    HangulMode
    HanjaMode
    Help
    Home
    I
    IMEAccept
    IMEAceept
    IMEConvert
    IMEModeChange
    IMENonConvert
    Insert
    J
    JunjaMode
    K
    KanaMode
    KanjiMode
    L
    LButton
    LControl
    LMenu
    LShift
    LWin
    LaunchApplication1
    LaunchApplication2
    LaunchMail
    Left
    LineFeed
    M
    MButton
    MediaNextTrack
    MediaPlayPause
    MediaPreviousTrack
    MediaStop
    MenuCtrl
    Multiply
    N
    Next
    NoName
    NumLock
    NumPad0
    NumPad1
    NumPad2
    NumPad3
    NumPad4
    NumPad5
    NumPad6
    NumPad7
    NumPad8
    NumPad9
    O
    Oem1
    Oem102
    Oem2
    Oem3
    Oem4
    Oem5
    Oem6
    Oem7
    Oem8
    OemBackslash
    OemClear
    OemCloseBrackets
    OemComma
    OemMinus
    OemOpenBrackets
    OemPeriod
    OemPipe
    OemPlus
    OemQuestion
    OemQuotes
    OemSemicolon
    OemTilde
    P
    Pa1
    Packet
    PageDown
    PageUp
    Pause
    Play
    Print
    PrintScreen
    Prior
    Process
    Q
    R
    RButton
    RControl
    RMenu
    RShift
    RWin
    Return
    Right
    S
    Scroll
    Select
    SelectMedia
    Separator
    Shift
    Sleep
    Space
    Subtract
    T
    Tab
    U
    Up
    V
    VolumeDown
    VolumeMute
    VolumeUp
    W
    X
    XButton1
    XButton2
    Y
    Z
    Zoom
}

// Returns a "shortcut" display name
public func getKeyName(mdf: KeyModifiers, key: Key)->String
{
    var result: retval

    if mdf & .Alt do
        result += "Alt"

    if mdf & .Control
    {
        if result.length do
            result += "+"
        result += "Ctrl"
    }

    if mdf & .Shift
    {
        if result.length do
            result += "+"
        result += "Shift"
    }

    if result.length do
        result += "+"
    var k = key

    switch key
    {
    case NumPad0:
        result += "0"
    case NumPad1:
        result += "1"
    case NumPad2:
        result += "2"
    case NumPad3:
        result += "3"
    case NumPad4:
        result += "4"
    case NumPad5:
        result += "5"
    case NumPad6:
        result += "6"
    case NumPad7:
        result += "7"
    case NumPad8:
        result += "8"
    case NumPad9:
        result += "9"
    case Add:
        result += "+"
    case Subtract:
        result += "-"
    default:
        result += Reflection.getEnumName(Input.Key, &k, fullname: false)
    }

    return result
}
