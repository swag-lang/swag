#global if WINDOWS
#global public
#global namespace Input
using XInput, Win32

private var g_mapButtons: [@countof(GamePadButton)] GamePadButton = ?
#run
{
    using GamePadButton
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_A)] = A
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_B)] = B
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_X)] = X
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_Y)] = Y
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_LEFT_THUMB)] = LeftStick
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_RIGHT_THUMB)] = RightStick
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_START)] = Start
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_BACK)] = Back
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_LEFT_SHOULDER)] = LeftShoulder
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_RIGHT_SHOULDER)] = RightShoulder
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_UP)] = DPadUp
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_DOWN)] = DPadDown
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_LEFT)] = DPadLeft
    g_mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_RIGHT)] = DPadRight
}

// Returns true if the corresponding 'padIndex' is connected
func isConnected(padIndex: u32)->bool
{
    var state: XINPUT_STATE = ?
    return XInputGetState(padIndex, &state) == ERROR_SUCCESS
}

// Set the left and right vibration of the given 'padIndex'
func setVibration(padIndex: u32, left, right: f32) throw
{
    var vibration: XINPUT_VIBRATION = ?
    vibration.wLeftMotorSpeed = cast(WORD) (0xFFFF * Math.saturate(left))
    vibration.wRightMotorSpeed = cast(WORD) (0xFFFF * Math.saturate(right))
    res := XInputSetState(padIndex, &vibration)
    switch res
    {
    case ERROR_SUCCESS:                 return
    case ERROR_DEVICE_NOT_CONNECTED:    throw GamePadError.DeviceNotConnected
    default:                            try Win32.setError(res)
    }
}

impl GamePadState
{
    // Compute the current state of the given pad index
    func update(using self, padIndex: u32) throw
    {
        var state: XINPUT_STATE = ?
        res := XInputGetState(padIndex, &state)
        switch res
        {
        case ERROR_SUCCESS:                 break
        case ERROR_DEVICE_NOT_CONNECTED:    throw GamePadError.DeviceNotConnected
        default:                            try Win32.setError(res)
        }

        // GamePadButton pressed state
        loop i: 16'u16
            pressed[g_mapButtons[i]] = state.Gamepad.wButtons & cast(u16)(1 << i)

        // Move
        using GamePadButton, state.Gamepad
        position[LeftStick].x = sThumbLX / 32767.0'f32
        position[LeftStick].y = sThumbLY / 32767.0'f32
        position[RightStick].x = sThumbRX / 32767.0'f32
        position[RightStick].y = sThumbRY / 32767.0'f32
        position[LeftTrigger].y = bLeftTrigger / 255.0'f32
        position[RightTrigger].y = bRightTrigger / 255.0'f32

        // Simulate pressed with,move
        #[Swag.inline]
        func getToler(using self, btn: GamePadButton)->f32
        {
            const SmallToler = 0.7'f32
            const BigToler = 0.9'f32
            return pressed[btn] ? SmallToler : BigToler
        }

        pressed[LeftThumbstickDown] = position[LeftStick].y < -getToler(self, LeftThumbstickDown)
        pressed[LeftThumbstickUp] = position[LeftStick].y > getToler(self, LeftThumbstickUp)
        pressed[LeftThumbstickLeft] = position[LeftStick].x < -getToler(self, LeftThumbstickLeft)
        pressed[LeftThumbstickRight] = position[LeftStick].x > getToler(self, LeftThumbstickRight)

        pressed[RightThumbstickDown] = position[RightStick].y < -getToler(self, RightThumbstickDown)
        pressed[RightThumbstickUp] = position[RightStick].y > getToler(self, RightThumbstickUp)
        pressed[RightThumbstickLeft] = position[RightStick].x < -getToler(self, RightThumbstickLeft)
        pressed[RightThumbstickRight] = position[RightStick].x > getToler(self, RightThumbstickRight)

        pressed[LeftTrigger] = position[LeftTrigger].y > getToler(self, LeftTrigger)
        pressed[RightTrigger] = position[RightTrigger].y > getToler(self, RightTrigger)
    }
}
