#if std.WINDOWS
{
    using xinput, win32, std.gamepad

    private var g_mapButtons: [@countof(Button)] Button = ?

    #run
    {
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_A)] = Button.A
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_B)] = Button.B
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_X)] = Button.X
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_Y)] = Button.Y
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_LEFT_THUMB)] = Button.LeftStick
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_RIGHT_THUMB)] = Button.RightStick
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_START)] = Button.Start
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_BACK)] = Button.Back
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_LEFT_SHOULDER)] = Button.LeftShoulder
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_RIGHT_SHOULDER)] = Button.RightShoulder
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_DPAD_UP)] = Button.DPadUp
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_DPAD_DOWN)] = Button.DPadDown
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_DPAD_LEFT)] = Button.DPadLeft
        g_mapButtons[math.bitIndex(XINPUT_GAMEPAD_DPAD_RIGHT)] = Button.DPadRight
    }

    namespace std.gamepad
	{
        // Returns true if the corresponding `padIndex` is connected
		public func isConnected(padIndex: u32)->bool
		{
			var state: XINPUT_STATE = ?
			return XInputGetState(padIndex, &state) != ERROR_SUCCESS
		}

        // Set the left and right vibration of the given `padIndex`
        public func setVibration(padIndex: u32, left, right: f32)->bool
        {
            var vibration: XINPUT_VIBRATION = ?
            vibration.wLeftMotorSpeed = cast(WORD) (0xFFFF * math.saturate(left))
            vibration.wRightMotorSpeed = cast(WORD) (0xFFFF * math.saturate(right))
            return XInputSetState(padIndex, &vibration) == ERROR_SUCCESS
        }

        public impl State
        {
            // Compute the current state of the given pad index
            func update(using self, padIndex: u32)->bool
            {
                var state: XINPUT_STATE = ?
                if XInputGetState(padIndex, &state) != ERROR_SUCCESS
                    return false

                // Button pressed state
                loop i: 16
                    pressed[cast(u32) g_mapButtons[i]] = state.Gamepad.wButtons & (1 << i)

				// Move
                using Button, state.Gamepad
                position[cast(u32) LeftStick].x = sThumbLX / 32767.0'f32
                position[cast(u32) LeftStick].y = sThumbLY / 32767.0'f32
                position[cast(u32) RightStick].x = sThumbRX / 32767.0'f32
                position[cast(u32) RightStick].y = sThumbRY / 32767.0'f32
                position[cast(u32) LeftTrigger].y = bLeftTrigger / 255.0'f32
                position[cast(u32) RightTrigger].y = bRightTrigger / 255.0'f32

				// Simulate pressed with move
				#[swag.inline]
				func getToler(self, btn: Button)->f32
				{
					const SmallToler = 0.7'f32
					const BigToler = 0.9'f32
					return self.pressed[cast(u32) btn] ? SmallToler : BigToler
				}

				pressed[cast(u32) LeftThumbstickDown] = position[cast(u32) LeftStick].y < -getToler(self, LeftThumbstickDown)
				pressed[cast(u32) LeftThumbstickUp] = position[cast(u32) LeftStick].y > getToler(self, LeftThumbstickUp)
				pressed[cast(u32) LeftThumbstickLeft] = position[cast(u32) LeftStick].x < -getToler(self, LeftThumbstickLeft)
				pressed[cast(u32) LeftThumbstickRight] = position[cast(u32) LeftStick].x > getToler(self, LeftThumbstickRight)

				pressed[cast(u32) RightThumbstickDown] = position[cast(u32) RightStick].y < -getToler(self, RightThumbstickDown)
				pressed[cast(u32) RightThumbstickUp] = position[cast(u32) RightStick].y > getToler(self, RightThumbstickUp)
				pressed[cast(u32) RightThumbstickLeft] = position[cast(u32) RightStick].x < -getToler(self, RightThumbstickLeft)
				pressed[cast(u32) RightThumbstickRight] = position[cast(u32) RightStick].x > getToler(self, RightThumbstickRight)

                pressed[cast(u32) LeftTrigger] = position[cast(u32) LeftTrigger].y > getToler(self, LeftTrigger)
                pressed[cast(u32) RightTrigger] = position[cast(u32) RightTrigger].y > getToler(self, RightTrigger)
                return true
            }
        }
	}
}
