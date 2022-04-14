#global if WINDOWS
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
        discard GetKeyboardState(nativeState)
    }
}