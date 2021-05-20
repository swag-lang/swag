#global export
#global foreignlib "xinput"

using Swag, Win32

const XINPUT_GAMEPAD_DPAD_UP          = 0x0001'u16
const XINPUT_GAMEPAD_DPAD_DOWN        = 0x0002'u16
const XINPUT_GAMEPAD_DPAD_LEFT        = 0x0004'u16
const XINPUT_GAMEPAD_DPAD_RIGHT       = 0x0008'u16
const XINPUT_GAMEPAD_START            = 0x0010'u16
const XINPUT_GAMEPAD_BACK             = 0x0020'u16
const XINPUT_GAMEPAD_LEFT_THUMB       = 0x0040'u16
const XINPUT_GAMEPAD_RIGHT_THUMB      = 0x0080'u16
const XINPUT_GAMEPAD_LEFT_SHOULDER    = 0x0100'u16
const XINPUT_GAMEPAD_RIGHT_SHOULDER   = 0x0200'u16
const XINPUT_GAMEPAD_A                = 0x1000'u16
const XINPUT_GAMEPAD_B                = 0x2000'u16
const XINPUT_GAMEPAD_X                = 0x4000'u16
const XINPUT_GAMEPAD_Y                = 0x8000'u16

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
