#if swag.WINDOWS
{
    #foreignlib "xinput"
	using std.os, std.input, std.math

	namespace std.input.os
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

		private var g_mapButtons: [@countof(GamePadButton)] GamePadButton = ?

		#run
		{
			g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_A)] = GamePadButton.A
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_B)] = GamePadButton.B
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_X)] = GamePadButton.X
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_Y)] = GamePadButton.Y
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_LEFT_THUMB)] = GamePadButton.LeftStick
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_RIGHT_THUMB)] = GamePadButton.RightStick
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_START)] = GamePadButton.Start
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_BACK)] = GamePadButton.Back
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_LEFT_SHOULDER)] = GamePadButton.LeftShoulder
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_RIGHT_SHOULDER)] = GamePadButton.RightShoulder
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_DPAD_UP)] = GamePadButton.DPadUp
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_DPAD_DOWN)] = GamePadButton.DPadDown
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_DPAD_LEFT)] = GamePadButton.DPadLeft
            g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_DPAD_RIGHT)] = GamePadButton.DPadRight
		}

		#[swag.foreign("xinput")]
		{
			func XInputGetState(dwUserIndex: DWORD,  pState: *XINPUT_STATE)->DWORD;
            func XInputSetState(dwUserIndex: DWORD,  pVibration: *XINPUT_VIBRATION)->DWORD;
		}
    }

    namespace std.input
	{
        public func setGamePadVibration(padIndex: u32, left, right: f32)->bool
        {
            using std.input.os
            var vibration: XINPUT_VIBRATION = ?
            vibration.wLeftMotorSpeed = cast(WORD) (0xFFFF * saturate(left))
            vibration.wRightMotorSpeed = cast(WORD) (0xFFFF * saturate(right))
            return XInputSetState(padIndex, &vibration) == std.os.ERROR_SUCCESS
        }

        impl GamePadState
        {
            public func update(using self, padIndex: u32)->bool
            {
                using std.input.os
                var state: XINPUT_STATE = ?
                if XInputGetState(padIndex, &state) != std.os.ERROR_SUCCESS
                    return false

                // Button pressed state
                loop i: 16
                    pressed[cast(u32) g_mapButtons[i]] = state.Gamepad.wButtons & (1 << i)

                movement[cast(u32) GamePadButton.LeftStick].x = state.Gamepad.sThumbLX / 32767.0'f32
                movement[cast(u32) GamePadButton.LeftStick].y = state.Gamepad.sThumbLY / 32767.0'f32
                movement[cast(u32) GamePadButton.RightStick].x = state.Gamepad.sThumbRX / 32767.0'f32
                movement[cast(u32) GamePadButton.RightStick].y = state.Gamepad.sThumbRY / 32767.0'f32
                movement[cast(u32) GamePadButton.LeftTrigger].y = state.Gamepad.bLeftTrigger / 255.0'f32
                movement[cast(u32) GamePadButton.RightTrigger].y = state.Gamepad.bRightTrigger / 255.0'f32

                return true
            }
        }
	}
}
