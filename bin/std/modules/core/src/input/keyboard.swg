#global marked
#global public
#global namespace Input

// Allows getting keystrokes from keyboard
struct Keyboard
{
    previousState:     KeyboardState
    currentState:      KeyboardState
    pressedRepeat:     [@countof(Key)] bool
    pressedTime:       [@countof(Key)] u32

    // Delay, in milliseconds, before a repeated key stroke starts
    pressedRepeatStartTimeMs: u32 = 400
    // Delay, in miliseconds, between two repeated key strokes
    pressedRepeatTimeMs: u32 = 50
    // Allow key stroke repetition if true
    canRepeat: bool = true
}

impl Keyboard
{
    // Compute current state of the keyboard (all keys)
    mtd update()
    {
        self.previousState = self.currentState
        self.currentState.update()

        // Repetition
        let now = Time.nowMilliseconds()
        for i in @countof(Key)
        {
            if !self.currentState.isKeyPressed(cast(Key) i)
            {
                self.pressedRepeat[i] = false
                self.pressedTime[i]   = 0
            }
            elif self.canRepeat
            {
                // First pressed time
                if !self.pressedTime[i]:
                    self.pressedTime[i] = now

                // First repetition
                if !self.pressedRepeat[i]
                {
                    if now - self.pressedTime[i] >= self.pressedRepeatStartTimeMs
                    {
                        self.pressedRepeat[i]         = true
                        self.pressedTime[i]           = now
                        self.previousState.pressed[i] = false
                    }
                }
                elif now - self.pressedTime[i] >= self.pressedRepeatTimeMs
                {
                    self.pressedTime[i]           = now
                    self.previousState.pressed[i] = false
                }
            }
        }
    }

    // Reset the keyboard state
    mtd clear()
    {
        self.previousState.clear()
        self.currentState.clear()
        Memory.clear(&self.pressedRepeat, #sizeof(self.pressedRepeat))
        Memory.clear(&self.pressedTime, #sizeof(self.pressedTime))
    }

    // Determines whether given key is currently being pressed
    #[Swag.Overload]
    mtd isKeyPressed(key: Key)->bool
    {
        return self.currentState.pressed[key]
    }

    // Determines whether given key has just been pressed
    mtd isKeyJustPressed(key: Key)->bool
    {
        return self.currentState.pressed[key] and !self.previousState.pressed[key]
    }

    // Determines whether given key is currently being released
    mtd isKeyReleased(key: Key)->bool
    {
        return !self.currentState.pressed[key]
    }

    // Determines whether given key has just been released
    mtd isKeyJustReleased(key: Key)->bool
    {
        return !self.currentState.pressed[key] and self.previousState.pressed[key]
    }

    // Returns the currently pressed key modifiers
    mtd getPressedModifiers()->KeyModifiers
    {
        var modifiers: Input.KeyModifiers = .Zero
        if self.isKeyPressed(Input.Key.LShift):
            modifiers |= .Shift
        if self.isKeyPressed(Input.Key.RShift):
            modifiers |= .Shift
        if self.isKeyPressed(Input.Key.LControl):
            modifiers |= .Control
        if self.isKeyPressed(Input.Key.RControl):
            modifiers |= .Control
        if self.isKeyPressed(Input.Key.LMenu):
            modifiers |= .Alt
        if self.isKeyPressed(Input.Key.RMenu):
            modifiers |= .Alt
        return modifiers
    }
}
