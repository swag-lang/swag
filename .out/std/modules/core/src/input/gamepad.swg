#global public
#global namespace Input
using Math

enum GamePadError: string
{
    DeviceNotConnected = "not connected"
}

// Represents a gamepad
struct GamePad
{
    previousState:  GamePadState
    currentState:   GamePadState

    // The pad index associated with that GamePad instance
    padIndex: u32

    // true if the corresponding padIndex is connected
    connected: bool
}

impl GamePad
{
    // Update the GamePad current state
    func update(using self) throw
    {
        newConnected := isConnected(padIndex)
        if !newConnected
        {
            if connected clear(self)
            connected = false
        }
        else
        {
            connected = true
            previousState = currentState
            try currentState.update(padIndex)
        }
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
        return currentState.pressed[button]
    }

    // Determines whether specified input device button has just been pressed
    func isButtonJustPressed(using self, button: GamePadButton)->bool
    {
        return currentState.pressed[button] and !previousState.pressed[button]
    }

    // Determines whether specified input device button is released (not pressed)
    func isButtonReleased(using self, button: GamePadButton)->bool
    {
        return !currentState.pressed[button]
    }

    // Determines whether specified input device button has just been released
    func isButtonJustReleased(using self, button: GamePadButton)->bool
    {
        return !currentState.pressed[button] and previousState.pressed[button]
    }

    // Get the analog position of the given button
    func getPosition(using self, button: GamePadButton)->Point
    {
        return currentState.position[button]
    }

    // Set the vibration motor speeds
    func setVibration(using self, left, right: f32) throw
    {
        try setVibration(padIndex, left, right)
    }
}
