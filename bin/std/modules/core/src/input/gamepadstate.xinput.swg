#global marked
#global #if WINDOWS
#global public
#global namespace Input
using XInput, Win32, Core.Errors

#run
{
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_A)]              = GamePadButton.A
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_B)]              = GamePadButton.B
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_X)]              = GamePadButton.X
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_Y)]              = GamePadButton.Y
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_LEFT_THUMB)]     = GamePadButton.LeftStick
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_RIGHT_THUMB)]    = GamePadButton.RightStick
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_START)]          = GamePadButton.Start
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_BACK)]           = GamePadButton.Back
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_LEFT_SHOULDER)]  = GamePadButton.LeftShoulder
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_RIGHT_SHOULDER)] = GamePadButton.RightShoulder
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_UP)]        = GamePadButton.DPadUp
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_DOWN)]      = GamePadButton.DPadDown
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_LEFT)]      = GamePadButton.DPadLeft
    GamePadState.mapButtons[Math.trailingZeros(XINPUT_GAMEPAD_DPAD_RIGHT)]     = GamePadButton.DPadRight
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
        self.position[GamePadButton.LeftStick].x    = state.Gamepad.sThumbLX / 32767.0'f32
        self.position[GamePadButton.LeftStick].y    = state.Gamepad.sThumbLY / 32767.0'f32
        self.position[GamePadButton.RightStick].x   = state.Gamepad.sThumbRX / 32767.0'f32
        self.position[GamePadButton.RightStick].y   = state.Gamepad.sThumbRY / 32767.0'f32
        self.position[GamePadButton.LeftTrigger].y  = state.Gamepad.bLeftTrigger / 255.0'f32
        self.position[GamePadButton.RightTrigger].y = state.Gamepad.bRightTrigger / 255.0'f32

        // Simulate pressed with #move
        #[Swag.Inline]
        mtd getToler(btn: GamePadButton)->f32
        {
            const SmallToler = 0.7'f32
            const BigToler   = 0.9'f32
            return self.pressed[btn] ? SmallToler : BigToler
        }

        self.pressed[GamePadButton.LeftThumbstickDown]  = self.position[GamePadButton.LeftStick].y < -getToler(self, GamePadButton.LeftThumbstickDown)
        self.pressed[GamePadButton.LeftThumbstickUp]    = self.position[GamePadButton.LeftStick].y > getToler(self, GamePadButton.LeftThumbstickUp)
        self.pressed[GamePadButton.LeftThumbstickLeft]  = self.position[GamePadButton.LeftStick].x < -getToler(self, GamePadButton.LeftThumbstickLeft)
        self.pressed[GamePadButton.LeftThumbstickRight] = self.position[GamePadButton.LeftStick].x > getToler(self, GamePadButton.LeftThumbstickRight)

        self.pressed[GamePadButton.RightThumbstickDown]  = self.position[GamePadButton.RightStick].y < -getToler(self, GamePadButton.RightThumbstickDown)
        self.pressed[GamePadButton.RightThumbstickUp]    = self.position[GamePadButton.RightStick].y > getToler(self, GamePadButton.RightThumbstickUp)
        self.pressed[GamePadButton.RightThumbstickLeft]  = self.position[GamePadButton.RightStick].x < -getToler(self, GamePadButton.RightThumbstickLeft)
        self.pressed[GamePadButton.RightThumbstickRight] = self.position[GamePadButton.RightStick].x > getToler(self, GamePadButton.RightThumbstickRight)

        self.pressed[GamePadButton.LeftTrigger]  = self.position[GamePadButton.LeftTrigger].y > getToler(self, GamePadButton.LeftTrigger)
        self.pressed[GamePadButton.RightTrigger] = self.position[GamePadButton.RightTrigger].y > getToler(self, GamePadButton.RightTrigger)
    }
}
