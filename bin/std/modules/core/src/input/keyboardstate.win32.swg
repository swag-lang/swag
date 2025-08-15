#global marked
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
        discard GetKeyboardState(self.nativeState)
    }

    mtd isKeyPressed(key: Input.Key)->bool
    {
        let vkey = Keyboard.mapKeyToVKey[key]
        if vkey == 0:
            return false
        return self.nativeState[vkey] & 0x80 ? true : false
    }
}
