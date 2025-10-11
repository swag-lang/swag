#global public
#global namespace Input
using Math

// Represents a gamepad
struct GamePad
{
    previousState:     GamePadState
    currentState:      GamePadState

    // The pad index associated with that GamePad instance
    padIndex: u32

    // true if the corresponding padIndex is connected
    connected: bool
}

impl GamePad
{
    // Update the GamePad current state
    mtd update() throw
    {
        let newConnected = isConnected(.padIndex)
        if !newConnected
        {
            if .connected do
                clear(me)
            .connected = false
        }
        else
        {
            .connected     = true
            .previousState = .currentState
            try .currentState.update(.padIndex)
        }
    }

    // Reset the previous and current state
    mtd clear()
    {
        .previousState.clear()
        .currentState.clear()
    }

    // Determines whether specified input device button is pressed
    mtd isButtonPressed(button: GamePadButton)->bool
    {
        return .currentState.pressed[button]
    }

    // Determines whether specified input device button has just been pressed
    mtd isButtonJustPressed(button: GamePadButton)->bool
    {
        return .currentState.pressed[button] and !.previousState.pressed[button]
    }

    // Determines whether specified input device button is released (not pressed)
    mtd isButtonReleased(button: GamePadButton)->bool
    {
        return !.currentState.pressed[button]
    }

    // Determines whether specified input device button has just been released
    mtd isButtonJustReleased(button: GamePadButton)->bool
    {
        return !.currentState.pressed[button] and .previousState.pressed[button]
    }

    // Get the analog position of the given button
    mtd getPosition(button: GamePadButton)->Point
    {
        return .currentState.position[button]
    }

    // Set the vibration motor speeds
    #[Swag.Overload]
    mtd setVibration(left, right: f32) throw
    {
        try setVibration(.padIndex, left, right)
    }
}
