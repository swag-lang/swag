#global namespace Input

// Represents a virtual system key
public alias VirtualKey = u32

// Defines the keys on a keyboard
#[Swag.EnumIndex]
public enum Key
{
    // No key
    None
    // A key
    A
    // Add key
    Add
    // Application key
    Apps
    // Attn key
    Attn
    // B key
    B
    // BACKSPACE key
    Back
    // Browser Back key
    BrowserBack
    // Browser Favorites key
    BrowserFavorites
    // Browser Forward key
    BrowserForward
    // Browser Home key
    BrowserHome
    // Browser Refresh key
    BrowserRefresh
    // Browser Search key
    BrowserSearch
    // Browser Stop key
    BrowserStop
    C
    Cancel
    Capital
    CapsLock
    Clear
    ControlKey
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
    // F key
    F
    // F1 key
    F1
    // F10 key
    F10
    // F11 key
    F11
    // F12 key
    F12
    // F13 key
    F13
    // F14 key
    F14
    // F15 key
    F15
    // F16 key
    F16
    // F17 key
    F17
    // F18 key
    F18
    // F19 key
    F19
    // F2 key
    F2
    // F20 key
    F20
    // F21 key
    F21
    // F22 key
    F22
    // F23 key
    F23
    // F24 key
    F24
    // F3 key
    F3
    // F4 key
    F4
    // F5 key
    F5
    // F6 key
    F6
    // F7 key
    F7
    // F8 key
    F8
    // F9 key
    F9
    FinalMode
    // G key
    G
    // H key
    H
    HanguelMode
    HangulMode
    HanjaMode
    Help
    Home
    // I key
    I
    IMEAccept
    IMEAceept
    IMEConvert
    IMEModeChange
    IMENonConvert
    Insert
    // J key
    J
    JunjaMode
    // K key
    K
    KanaMode
    KanjiMode
    // L key
    L
    LButton
    LControlKey
    LMenu
    LShiftKey
    LWin
    LaunchApplication1
    LaunchApplication2
    LaunchMail
    Left
    LineFeed
    // M key
    M
    MButton
    MediaNextTrack
    MediaPlayPause
    MediaPreviousTrack
    MediaStop
    Menu
    Multiply
    // N key
    N
    Next
    NoName
    // NUM LOCK key
    NumLock
    // Numeric keypard 0 key
    NumPad0
    // Numeric keypard 1 key
    NumPad1
    // Numeric keypard 2 key
    NumPad2
    // Numeric keypard 3 key
    NumPad3
    // Numeric keypard 4 key
    NumPad4
    // Numeric keypard 5 key
    NumPad5
    // Numeric keypard 6 key
    NumPad6
    // Numeric keypard 7 key
    NumPad7
    // Numeric keypard 8 key
    NumPad8
    // Numeric keypard 9 key
    NumPad9
    // O key
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
    // P key
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
    ProcessKey
    // Q key
    Q
    // R key
    R
    RButton
    RControlKey
    RMenu
    RShiftKey
    RWin
    Return
    Right
    // S key
    S
    Scroll
    Select
    SelectMedia
    Separator
    ShiftKey
    // Computer Sleep key
    Sleep
    Snapshot
    // SPACEBAR key
    Space
    // Substract key
    Subtract
    // T key
    T
    // TAB key
    Tab
    // U key
    U
    // UP ARROW key
    Up
    // V key
    V
    // Volume Down key
    VolumeDown
    // Volume Mute key
    VolumeMute
    // Volume Up key
    VolumeUp
    // W key
    W
    // X key
    X
    // Mouse XButton1 button
    XButton1
    // Mouse XButton2 button
    XButton2
    // Y key
    Y
    // Z key
    Z
    // Zoom key
    Zoom
}
