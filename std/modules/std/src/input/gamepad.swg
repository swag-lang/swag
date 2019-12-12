namespace std.input
{
    using std.math

	/// Represents a gamepad
    public struct GamePad
    {
        previousState:  GamePadState
        currentState:   GamePadState
        padIndex:       u32
    }

    impl GamePad
    {
        public
        {
			/// Update the GamePad current state
            func update(using self)
            {
                previousState = currentState
                currentState.update(padIndex)
            }

			/// Reset the previous and current state
            func clear(using self)
            {
                previousState.clear()
                currentState.clear()
                padIndex = 0
            }

			/// Determines whether specified input device button is pressed
            func isButtonPressed(using self, button: GamePadButton)->bool
            {
                return currentState.pressed[cast(u32) button]
            }

			/// Determines whether specified input device button has just been pressed
            func isButtonJustPressed(using self, button: GamePadButton)->bool
            {
                return currentState.pressed[cast(u32) button] && !previousState.pressed[cast(u32) button]
            }

			/// Determines whether specified input device button is released (not pressed)
            func isButtonReleased(using self, button: GamePadButton)->bool
            {
                return !currentState.pressed[cast(u32) button]
            }

			/// Determines whether specified input device button has just been released
            func isButtonJustReleased(using self, button: GamePadButton)->bool
            {
                return !currentState.pressed[cast(u32) button] && previousState.pressed[cast(u32) button]
            }

			/// Get the analog position of the given button
            func getPosition(using self, button: GamePadButton)->Point
            {
                return currentState.getPosition(button)
            }
        }
    }
}
