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
        let newConnected = isConnected(me.padIndex)
        if !newConnected
        {
            if me.connected:
                clear(me)
            me.connected = false
        }
        else
        {
            me.connected     = true
            me.previousState = me.currentState
            try me.currentState.update(me.padIndex)
        }
    }

    // Reset the previous and current state
    mtd clear()
    {
        me.previousState.clear()
        me.currentState.clear()
    }

    // Determines whether specified input device button is pressed
    mtd isButtonPressed(button: GamePadButton)->bool
    {
        return me.currentState.pressed[button]
    }

    // Determines whether specified input device button has just been pressed
    mtd isButtonJustPressed(button: GamePadButton)->bool
    {
        return me.currentState.pressed[button] and !me.previousState.pressed[button]
    }

    // Determines whether specified input device button is released (not pressed)
    mtd isButtonReleased(button: GamePadButton)->bool
    {
        return !me.currentState.pressed[button]
    }

    // Determines whether specified input device button has just been released
    mtd isButtonJustReleased(button: GamePadButton)->bool
    {
        return !me.currentState.pressed[button] and me.previousState.pressed[button]
    }

    // Get the analog position of the given button
    mtd getPosition(button: GamePadButton)->Point
    {
        return me.currentState.position[button]
    }

    // Set the vibration motor speeds
    #[Swag.Overload]
    mtd setVibration(left, right: f32) throw
    {
        try setVibration(me.padIndex, left, right)
    }
}
