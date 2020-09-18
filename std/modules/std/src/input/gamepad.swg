namespace std.input
{
    using std.math

    // Represents a gamepad
    public struct GamePad
    {
        previousState:  GamePadState
        currentState:   GamePadState

        // The pad index associated with that GamePad instance
        padIndex: u32

        // true if the corresponding padIndex is connected
        readonly connected: bool
    }

    public impl GamePad
    {
        // Update the GamePad current state
        func update(using self)
        {
            newConnected := std.input.isGamePadConnected(padIndex)
            if !newConnected
            {
                if connected
                    clear(self)
                connected = false
                return;
            }

            connected = true
            previousState = currentState
            currentState.update(padIndex)
        }

        // Reset the previous and current state
        func clear(using self)
        {
            previousState.clear()
            currentState.clear()
        }

        // Determines whether specified input device button is pressed
        func isButtonPressed(using self, button: GamePadButton)->bool
        {
            return currentState.pressed[cast(u32) button]
        }

        // Determines whether specified input device button has just been pressed
        func isButtonJustPressed(using self, button: GamePadButton)->bool
        {
            return currentState.pressed[cast(u32) button] && !previousState.pressed[cast(u32) button]
        }

        // Determines whether specified input device button is released (not pressed)
        func isButtonReleased(using self, button: GamePadButton)->bool
        {
            return !currentState.pressed[cast(u32) button]
        }

        // Determines whether specified input device button has just been released
        func isButtonJustReleased(using self, button: GamePadButton)->bool
        {
            return !currentState.pressed[cast(u32) button] && previousState.pressed[cast(u32) button]
        }

        // Get the analog position of the given button
        func getPosition(using self, button: GamePadButton)->Point
        {
            return currentState.position[cast(u32) button]
        }

        // Set the vibration motor speeds
        func setVibration(using self, left, right: f32)->bool
        {
            return std.input.setGamePadVibration(padIndex, left, right)
        }
    }
}
