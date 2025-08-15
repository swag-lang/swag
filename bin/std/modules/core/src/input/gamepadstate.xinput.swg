#global marked
#global #if WINDOWS
#global public
#global namespace Input
using XInput, Win32, Core.Errors

#run
{
    with GamePadButton
    {
        with GamePadState
        {
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_A)]              = .A
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_B)]              = .B
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_X)]              = .X
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_Y)]              = .Y
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_LEFT_THUMB)]     = .LeftStick
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_RIGHT_THUMB)]    = .RightStick
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_START)]          = .Start
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_BACK)]           = .Back
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_LEFT_SHOULDER)]  = .LeftShoulder
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_RIGHT_SHOULDER)] = .RightShoulder
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_UP)]        = .DPadUp
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_DOWN)]      = .DPadDown
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_LEFT)]      = .DPadLeft
            .mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_RIGHT)]     = .DPadRight
        }
    }
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
            self.pressed[mapButtons[i]] = state.Gamepad.wButtons & cast(u16) (1 << i)

        // Move
        with GamePadButton
        {
            self.position[.LeftStick].x    = state.Gamepad.sThumbLX / 32767.0'f32
            self.position[.LeftStick].y    = state.Gamepad.sThumbLY / 32767.0'f32
            self.position[.RightStick].x   = state.Gamepad.sThumbRX / 32767.0'f32
            self.position[.RightStick].y   = state.Gamepad.sThumbRY / 32767.0'f32
            self.position[.LeftTrigger].y  = state.Gamepad.bLeftTrigger / 255.0'f32
            self.position[.RightTrigger].y = state.Gamepad.bRightTrigger / 255.0'f32
        }

        // Simulate pressed with #move
        #[Swag.Inline]
        mtd getToler(btn: GamePadButton)->f32
        {
            const SmallToler = 0.7'f32
            const BigToler   = 0.9'f32
            return self.pressed[btn] ? SmallToler : BigToler
        }

        with GamePadButton
        {
            self.pressed[.LeftThumbstickDown]  = self.position[.LeftStick].y < -getToler(self, .LeftThumbstickDown)
            self.pressed[.LeftThumbstickUp]    = self.position[.LeftStick].y > getToler(self, .LeftThumbstickUp)
            self.pressed[.LeftThumbstickLeft]  = self.position[.LeftStick].x < -getToler(self, .LeftThumbstickLeft)
            self.pressed[.LeftThumbstickRight] = self.position[.LeftStick].x > getToler(self, .LeftThumbstickRight)

            self.pressed[.RightThumbstickDown]  = self.position[.RightStick].y < -getToler(self, .RightThumbstickDown)
            self.pressed[.RightThumbstickUp]    = self.position[.RightStick].y > getToler(self, .RightThumbstickUp)
            self.pressed[.RightThumbstickLeft]  = self.position[.RightStick].x < -getToler(self, .RightThumbstickLeft)
            self.pressed[.RightThumbstickRight] = self.position[.RightStick].x > getToler(self, .RightThumbstickRight)

            self.pressed[.LeftTrigger]  = self.position[.LeftTrigger].y > getToler(self, .LeftTrigger)
            self.pressed[.RightTrigger] = self.position[.RightTrigger].y > getToler(self, .RightTrigger)
        }
    }
}
