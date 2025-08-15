#global marked
#global public
#global namespace Input
using Math

// Represents a gamepad
struct GamePad
{
    previousState:     GamePadState
    currentState:      GamePadState

    padIndex:          u32              // The pad index associated with that GamePad instance
    connected:         bool             // The pad index associated with that GamePad instance
}

impl GamePad
{
    // Update the GamePad current state
    mtd update() throw
    {
        let newConnected = isConnected(self.padIndex)
        if !newConnected
        {
            if self.connected:
                clear(self)
            self.connected = false
        }
        else
        {
            self.connected     = true
            self.previousState = self.currentState
            try self.currentState.update(self.padIndex)
        }
    }

    // Reset the previous and current state
    mtd clear()
    {
        self.previousState.clear()
        self.currentState.clear()
    }

    // Determines whether specified input device button is pressed
    mtd isButtonPressed(button: GamePadButton)->bool
    {
        return self.currentState.pressed[button]
    }

    // Determines whether specified input device button has just been pressed
    mtd isButtonJustPressed(button: GamePadButton)->bool
    {
        return self.currentState.pressed[button] and !self.previousState.pressed[button]
    }

    // Determines whether specified input device button is released (not pressed)
    mtd isButtonReleased(button: GamePadButton)->bool
    {
        return !self.currentState.pressed[button]
    }

    // Determines whether specified input device button has just been released
    mtd isButtonJustReleased(button: GamePadButton)->bool
    {
        return !self.currentState.pressed[button] and self.previousState.pressed[button]
    }

    // Get the analog position of the given button
    mtd getPosition(button: GamePadButton)->Point
    {
        return self.currentState.position[button]
    }

    // Set the vibration motor speeds
    #[Swag.Overload]
    mtd setVibration(left, right: f32) throw
    {
        try setVibration(self.padIndex, left, right)
    }
}
