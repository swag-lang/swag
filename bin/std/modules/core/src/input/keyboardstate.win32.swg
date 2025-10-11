#global #if WINDOWS
#global namespace Input
using Win32

public struct KeyboardStateNative
{
    nativeState: [256] BYTE
}

impl KeyboardStateNative
{
    mtd updateNative()
    {
        discard GetKeyboardState(.nativeState)
    }

    mtd isKeyPressed(key: Input.Key)->bool
    {
        let vkey = Keyboard.mapKeyToVKey[key]
        if vkey == 0 do
            return false
        return .nativeState[vkey] & 0x80 ? true : false
    }
}
