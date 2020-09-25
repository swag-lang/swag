#import "win32"
#foreignlib "xinput"

using swag, win32

namespace xinput
{
    const XINPUT_GAMEPAD_DPAD_UP          = 0x0001
    const XINPUT_GAMEPAD_DPAD_DOWN        = 0x0002
    const XINPUT_GAMEPAD_DPAD_LEFT        = 0x0004
    const XINPUT_GAMEPAD_DPAD_RIGHT       = 0x0008
    const XINPUT_GAMEPAD_START            = 0x0010
    const XINPUT_GAMEPAD_BACK             = 0x0020
    const XINPUT_GAMEPAD_LEFT_THUMB       = 0x0040
    const XINPUT_GAMEPAD_RIGHT_THUMB      = 0x0080
    const XINPUT_GAMEPAD_LEFT_SHOULDER    = 0x0100
    const XINPUT_GAMEPAD_RIGHT_SHOULDER   = 0x0200
    const XINPUT_GAMEPAD_A                = 0x1000
    const XINPUT_GAMEPAD_B                = 0x2000
    const XINPUT_GAMEPAD_X                = 0x4000
    const XINPUT_GAMEPAD_Y                = 0x8000

    struct XINPUT_GAMEPAD
    {
        wButtons:		WORD
        bLeftTrigger:	BYTE
        bRightTrigger:	BYTE
        sThumbLX:		SHORT
        sThumbLY:		SHORT
        sThumbRX:		SHORT
        sThumbRY:		SHORT
    }

    struct XINPUT_STATE
    {
        dwPacketNumber:	DWORD
        Gamepad:		XINPUT_GAMEPAD
    }

    struct XINPUT_VIBRATION
    {
        wLeftMotorSpeed:    WORD
        wRightMotorSpeed:   WORD
    }

    #[foreign("xinput")]
    {
        func XInputGetState(dwUserIndex: DWORD,  pState: *XINPUT_STATE)->DWORD;
        func XInputSetState(dwUserIndex: DWORD,  pVibration: *XINPUT_VIBRATION)->DWORD;
    }
}
