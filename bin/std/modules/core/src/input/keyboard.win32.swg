#global #if WINDOWS
#global namespace Input
using Swag, Win32

impl Keyboard
{
    var mapKeyToVKey: [256] u8 = undefined
    var mapVKeyToKey: [256] u8 = undefined

    #[Compiler]
    func setupArray(key: Input.Key, vkey: u8)
    {
        mapKeyToVKey[key]  = vkey
        mapVKeyToKey[vkey] = cast(u8) key
    }

    public func keyToVirtualKey(key: Key)->VirtualKey
    {
        return cast(VirtualKey) mapKeyToVKey[key]
    }

    public func virtualKeyToKey(vkey: VirtualKey)->Key
    {
        if vkey > 255 do
            return Key.None
        return cast(Key) mapVKeyToKey[vkey]
    }
}

#run
{
    using Keyboard, Key
    setupArray(A, 'A')
    setupArray(Add, cast(u8) VK_ADD)
    setupArray(Apps, cast(u8) VK_APPS)
    setupArray(Attn, cast(u8) VK_ATTN)
    setupArray(B, 'B')
    setupArray(Back, cast(u8) VK_BACK)
    setupArray(BrowserBack, cast(u8) VK_BROWSER_BACK)
    setupArray(BrowserFavorites, cast(u8) VK_BROWSER_FAVORITES)
    setupArray(BrowserForward, cast(u8) VK_BROWSER_FORWARD)
    setupArray(BrowserHome, cast(u8) VK_BROWSER_HOME)
    setupArray(BrowserRefresh, cast(u8) VK_BROWSER_REFRESH)
    setupArray(BrowserSearch, cast(u8) VK_BROWSER_SEARCH)
    setupArray(BrowserStop, cast(u8) VK_BROWSER_STOP)
    setupArray(C, 'C')
    setupArray(Cancel, cast(u8) VK_CANCEL)
    setupArray(Capital, cast(u8) VK_CAPITAL)
    setupArray(CapsLock, cast(u8) VK_CAPITAL)
    setupArray(Clear, cast(u8) VK_CLEAR)
    setupArray(Control, cast(u8) VK_LCONTROL)
    setupArray(Crsel, cast(u8) VK_CRSEL)
    setupArray(D, 'D')
    setupArray(D0, '0')
    setupArray(D1, '1')
    setupArray(D2, '2')
    setupArray(D3, '3')
    setupArray(D4, '4')
    setupArray(D5, '5')
    setupArray(D6, '6')
    setupArray(D7, '7')
    setupArray(D8, '8')
    setupArray(D9, '9')
    setupArray(Decimal, cast(u8) VK_DECIMAL)
    setupArray(Delete, cast(u8) VK_DELETE)
    setupArray(Divide, cast(u8) VK_DIVIDE)
    setupArray(Down, cast(u8) VK_DOWN)
    setupArray(E, 'E')
    setupArray(End, cast(u8) VK_END)
    setupArray(Enter, cast(u8) VK_RETURN)
    setupArray(EraseEof, cast(u8) VK_EREOF)
    setupArray(Escape, cast(u8) VK_ESCAPE)
    setupArray(Execute, cast(u8) VK_EXECUTE)
    setupArray(Exsel, cast(u8) VK_EXSEL)
    setupArray(F, 'F')
    setupArray(F1, cast(u8) VK_F1)
    setupArray(F10, cast(u8) VK_F10)
    setupArray(F11, cast(u8) VK_F11)
    setupArray(F12, cast(u8) VK_F12)
    setupArray(F13, cast(u8) VK_F13)
    setupArray(F14, cast(u8) VK_F14)
    setupArray(F15, cast(u8) VK_F15)
    setupArray(F16, cast(u8) VK_F16)
    setupArray(F17, cast(u8) VK_F17)
    setupArray(F18, cast(u8) VK_F18)
    setupArray(F19, cast(u8) VK_F19)
    setupArray(F2, cast(u8) VK_F2)
    setupArray(F20, cast(u8) VK_F20)
    setupArray(F21, cast(u8) VK_F21)
    setupArray(F22, cast(u8) VK_F22)
    setupArray(F23, cast(u8) VK_F23)
    setupArray(F24, cast(u8) VK_F24)
    setupArray(F3, cast(u8) VK_F3)
    setupArray(F4, cast(u8) VK_F4)
    setupArray(F5, cast(u8) VK_F5)
    setupArray(F6, cast(u8) VK_F6)
    setupArray(F7, cast(u8) VK_F7)
    setupArray(F8, cast(u8) VK_F8)
    setupArray(F9, cast(u8) VK_F9)
    setupArray(FinalMode, cast(u8) VK_FINAL)
    setupArray(G, 'G')
    setupArray(H, 'H')
    setupArray(HanguelMode, cast(u8) VK_HANGEUL)
    setupArray(HangulMode, cast(u8) VK_HANGEUL)
    setupArray(HanjaMode, cast(u8) VK_HANJA)
    setupArray(Help, cast(u8) VK_HELP)
    setupArray(Home, cast(u8) VK_HOME)
    setupArray(I, 'I')
    setupArray(IMEAccept, cast(u8) VK_ACCEPT)
    setupArray(IMEAceept, cast(u8) VK_ACCEPT)
    setupArray(IMEConvert, cast(u8) VK_CONVERT)
    setupArray(IMEModeChange, cast(u8) VK_MODECHANGE)
    setupArray(IMENonConvert, cast(u8) VK_NONCONVERT)
    setupArray(Insert, cast(u8) VK_INSERT)
    setupArray(J, 'J')
    setupArray(JunjaMode, cast(u8) VK_JUNJA)
    setupArray(K, 'K')
    setupArray(KanaMode, cast(u8) VK_KANA)
    setupArray(KanjiMode, cast(u8) VK_KANJI)
    setupArray(L, 'L')
    setupArray(LButton, cast(u8) VK_LBUTTON)
    setupArray(LControl, cast(u8) VK_LCONTROL)
    setupArray(LMenu, cast(u8) VK_LMENU)
    setupArray(LShift, cast(u8) VK_LSHIFT)
    setupArray(LWin, cast(u8) VK_LWIN)
    setupArray(LaunchApplication1, cast(u8) VK_LAUNCH_APP1)
    setupArray(LaunchApplication2, cast(u8) VK_LAUNCH_APP2)
    setupArray(LaunchMail, cast(u8) VK_LAUNCH_MAIL)
    setupArray(Left, cast(u8) VK_LEFT)
    setupArray(LineFeed, cast(u8) 0x0A)
    setupArray(M, 'M')
    setupArray(MButton, cast(u8) VK_MBUTTON)
    setupArray(MediaNextTrack, cast(u8) VK_MEDIA_NEXT_TRACK)
    setupArray(MediaPlayPause, cast(u8) VK_MEDIA_PLAY_PAUSE)
    setupArray(MediaPreviousTrack, cast(u8) VK_MEDIA_PREV_TRACK)
    setupArray(MediaStop, cast(u8) VK_MEDIA_STOP)
    setupArray(MenuCtrl, cast(u8) VK_MENU)
    setupArray(Multiply, cast(u8) VK_MULTIPLY)
    setupArray(N, 'N')
    setupArray(Next, cast(u8) VK_NEXT)
    setupArray(NoName, cast(u8) VK_NONAME)
    setupArray(NumLock, cast(u8) VK_NUMLOCK)
    setupArray(NumPad0, cast(u8) VK_NUMPAD0)
    setupArray(NumPad1, cast(u8) VK_NUMPAD1)
    setupArray(NumPad2, cast(u8) VK_NUMPAD2)
    setupArray(NumPad3, cast(u8) VK_NUMPAD3)
    setupArray(NumPad4, cast(u8) VK_NUMPAD4)
    setupArray(NumPad5, cast(u8) VK_NUMPAD5)
    setupArray(NumPad6, cast(u8) VK_NUMPAD6)
    setupArray(NumPad7, cast(u8) VK_NUMPAD7)
    setupArray(NumPad8, cast(u8) VK_NUMPAD8)
    setupArray(NumPad9, cast(u8) VK_NUMPAD9)
    setupArray(O, 'O')
    setupArray(Oem1, cast(u8) VK_OEM_1)
    setupArray(Oem102, cast(u8) VK_OEM_102)
    setupArray(Oem2, cast(u8) VK_OEM_2)
    setupArray(Oem3, cast(u8) VK_OEM_3)
    setupArray(Oem4, cast(u8) VK_OEM_4)
    setupArray(Oem5, cast(u8) VK_OEM_5)
    setupArray(Oem6, cast(u8) VK_OEM_6)
    setupArray(Oem7, cast(u8) VK_OEM_7)
    setupArray(Oem8, cast(u8) VK_OEM_8)
    setupArray(OemBackslash, cast(u8) VK_OEM_102)
    setupArray(OemClear, cast(u8) VK_OEM_CLEAR)
    setupArray(OemCloseBrackets, cast(u8) VK_OEM_6)
    setupArray(OemMinus, cast(u8) VK_OEM_MINUS)
    setupArray(OemOpenBrackets, cast(u8) VK_OEM_4)
    setupArray(OemPeriod, cast(u8) VK_OEM_PERIOD)
    setupArray(OemPipe, cast(u8) VK_OEM_5)
    setupArray(OemQuestion, cast(u8) VK_OEM_2)
    setupArray(OemQuotes, cast(u8) VK_OEM_7)
    setupArray(OemSemicolon, cast(u8) VK_OEM_1)
    setupArray(OemComma, cast(u8) VK_OEM_COMMA)
    setupArray(OemPlus, cast(u8) VK_OEM_PLUS)
    setupArray(OemTilde, cast(u8) VK_OEM_3)
    setupArray(P, 'P')
    setupArray(Pa1, cast(u8) VK_PA1)
    setupArray(Packet, cast(u8) VK_PACKET)
    setupArray(PageDown, cast(u8) VK_NEXT)
    setupArray(PageUp, cast(u8) VK_PRIOR)
    setupArray(Pause, cast(u8) VK_PAUSE)
    setupArray(Play, cast(u8) VK_PLAY)
    setupArray(Print, cast(u8) VK_PRINT)
    setupArray(PrintScreen, cast(u8) VK_SNAPSHOT)
    setupArray(Prior, cast(u8) VK_PRIOR)
    setupArray(Process, cast(u8) VK_PROCESSKEY)
    setupArray(Q, 'Q')
    setupArray(R, 'R')
    setupArray(RButton, cast(u8) VK_RBUTTON)
    setupArray(RControl, cast(u8) VK_RCONTROL)
    setupArray(RMenu, cast(u8) VK_RMENU)
    setupArray(RShift, cast(u8) VK_RSHIFT)
    setupArray(RWin, cast(u8) VK_RWIN)
    setupArray(Return, cast(u8) VK_RETURN)
    setupArray(Right, cast(u8) VK_RIGHT)
    setupArray(S, 'S')
    setupArray(Scroll, cast(u8) VK_SCROLL)
    setupArray(Select, cast(u8) VK_SELECT)
    setupArray(SelectMedia, cast(u8) VK_LAUNCH_MEDIA_SELECT)
    setupArray(Separator, cast(u8) VK_SEPARATOR)
    setupArray(Shift, cast(u8) VK_LSHIFT)
    setupArray(Sleep, cast(u8) VK_SLEEP)
    setupArray(Space, cast(u8) VK_SPACE)
    setupArray(Subtract, cast(u8) VK_SUBTRACT)
    setupArray(T, 'T')
    setupArray(Tab, cast(u8) VK_TAB)
    setupArray(U, 'U')
    setupArray(Up, cast(u8) VK_UP)
    setupArray(V, 'V')
    setupArray(VolumeDown, cast(u8) VK_VOLUME_DOWN)
    setupArray(VolumeMute, cast(u8) VK_VOLUME_MUTE)
    setupArray(VolumeUp, cast(u8) VK_VOLUME_UP)
    setupArray(W, 'W')
    setupArray(X, 'X')
    setupArray(XButton1, cast(u8) VK_XBUTTON1)
    setupArray(XButton2, cast(u8) VK_XBUTTON2)
    setupArray(Y, 'Y')
    setupArray(Z, 'Z')
    setupArray(Zoom, cast(u8) VK_ZOOM)
}

public impl Keyboard
{
    // Returns true if the given key is pressed
    #[Swag.Overload]
    func isKeyPressed(key: Key)->bool
    {
        let vkey = Keyboard.mapKeyToVKey[key]
        if vkey == 0 do
            return false
        return GetAsyncKeyState(cast(s32) vkey) < 0
    }

    // Try to transform the given key to the corresponding rune
    mtd keyToRune(key: Key)->rune
    {
        let virt = keyToVirtualKey(key)
        var uc:  [5] WCHAR
        let res  = ToUnicode(virt, MapVirtualKeyW(virt, MAPVK_VK_TO_VSC), .currentState.nativeState, uc, 4, 0)
        if res == 1 do
            return cast(rune) uc[0]
        return 0'rune
    }
}
