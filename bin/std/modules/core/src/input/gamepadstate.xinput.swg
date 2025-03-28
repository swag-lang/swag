#global #if WINDOWS
#global public
#global namespace Input
using XInput, Win32, Core.Errors

#run
{
    using GamePadState, GamePadButton
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_A)]              = A
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_B)]              = B
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_X)]              = X
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_Y)]              = Y
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_LEFT_THUMB)]     = LeftStick
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_RIGHT_THUMB)]    = RightStick
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_START)]          = Start
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_BACK)]           = Back
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_LEFT_SHOULDER)]  = LeftShoulder
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_RIGHT_SHOULDER)] = RightShoulder
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_UP)]        = DPadUp
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_DOWN)]      = DPadDown
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_LEFT)]      = DPadLeft
    mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_RIGHT)]     = DPadRight
}

impl GamePadState
{
    internal var mapButtons: [@countof(GamePadButton)] GamePadButton = undefined

    // Compute the current state of the given pad index
    mtd update(padIndex: u32) throw
    {
        var state: XINPUT_STATE = undefined
        let res    = XInputGetState(padIndex, &state)
        switch res
        {
        case ERROR_SUCCESS:
            break
        case ERROR_DEVICE_NOT_CONNECTED:
            throw Swag.SystemError{"not connected"}
        default:
            Win32.throwWinError(res)
        }

        // GamePadButton pressed state
        for i in 16'u16:
            pressed[mapButtons[i]] = state.Gamepad.wButtons & cast(u16) (1 << i)

        // Move
        using GamePadButton, state.Gamepad
        position[LeftStick].x    = sThumbLX / 32767.0'f32
        position[LeftStick].y    = sThumbLY / 32767.0'f32
        position[RightStick].x   = sThumbRX / 32767.0'f32
        position[RightStick].y   = sThumbRY / 32767.0'f32
        position[LeftTrigger].y  = bLeftTrigger / 255.0'f32
        position[RightTrigger].y = bRightTrigger / 255.0'f32

        // Simulate pressed with #move
        #[Swag.Inline]
        mtd getToler(btn: GamePadButton)->f32
        {
            const SmallToler = 0.7'f32
            const BigToler   = 0.9'f32
            return pressed[btn] ? SmallToler : BigToler
        }

        pressed[LeftThumbstickDown]  = position[LeftStick].y < -getToler(self, LeftThumbstickDown)
        pressed[LeftThumbstickUp]    = position[LeftStick].y > getToler(self, LeftThumbstickUp)
        pressed[LeftThumbstickLeft]  = position[LeftStick].x < -getToler(self, LeftThumbstickLeft)
        pressed[LeftThumbstickRight] = position[LeftStick].x > getToler(self, LeftThumbstickRight)

        pressed[RightThumbstickDown]  = position[RightStick].y < -getToler(self, RightThumbstickDown)
        pressed[RightThumbstickUp]    = position[RightStick].y > getToler(self, RightThumbstickUp)
        pressed[RightThumbstickLeft]  = position[RightStick].x < -getToler(self, RightThumbstickLeft)
        pressed[RightThumbstickRight] = position[RightStick].x > getToler(self, RightThumbstickRight)

        pressed[LeftTrigger]  = position[LeftTrigger].y > getToler(self, LeftTrigger)
        pressed[RightTrigger] = position[RightTrigger].y > getToler(self, RightTrigger)
    }
}
