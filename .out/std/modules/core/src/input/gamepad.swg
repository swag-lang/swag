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
    method update() throw
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
    method clear()
    {
        previousState.clear()
        currentState.clear()
    }

    // Determines whether specified input device button is pressed
    method isButtonPressed(button: GamePadButton)->bool
    {
        return currentState.pressed[button]
    }

    // Determines whether specified input device button has just been pressed
    method isButtonJustPressed(button: GamePadButton)->bool
    {
        return currentState.pressed[button] and !previousState.pressed[button]
    }

    // Determines whether specified input device button is released (not pressed)
    method isButtonReleased(button: GamePadButton)->bool
    {
        return !currentState.pressed[button]
    }

    // Determines whether specified input device button has just been released
    method isButtonJustReleased(button: GamePadButton)->bool
    {
        return !currentState.pressed[button] and previousState.pressed[button]
    }

    // Get the analog position of the given button
    method getPosition(button: GamePadButton)->Point
    {
        return currentState.position[button]
    }

    // Set the vibration motor speeds
    method setVibration(left, right: f32) throw
    {
        try setVibration(padIndex, left, right)
    }
}
