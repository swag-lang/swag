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
        me.previousState = me.currentState
        me.currentState.update()

        // Repetition
        let now = Time.nowMilliseconds()
        for i in @countof(Key)
        {
            if !me.currentState.isKeyPressed(cast(Key) i)
            {
                me.pressedRepeat[i] = false
                me.pressedTime[i]   = 0
            }
            elif me.canRepeat
            {
                // First pressed time
                if !me.pressedTime[i]:
                    me.pressedTime[i] = now

                // First repetition
                if !me.pressedRepeat[i]
                {
                    if now - me.pressedTime[i] >= me.pressedRepeatStartTimeMs
                    {
                        me.pressedRepeat[i]         = true
                        me.pressedTime[i]           = now
                        me.previousState.pressed[i] = false
                    }
                }
                elif now - me.pressedTime[i] >= me.pressedRepeatTimeMs
                {
                    me.pressedTime[i]           = now
                    me.previousState.pressed[i] = false
                }
            }
        }
    }

    // Reset the keyboard state
    mtd clear()
    {
        me.previousState.clear()
        me.currentState.clear()
        Memory.clear(&me.pressedRepeat, #sizeof(me.pressedRepeat))
        Memory.clear(&me.pressedTime, #sizeof(me.pressedTime))
    }

    // Determines whether given key is currently being pressed
    #[Swag.Overload]
    mtd isKeyPressed(key: Key)->bool
    {
        return me.currentState.pressed[key]
    }

    // Determines whether given key has just been pressed
    mtd isKeyJustPressed(key: Key)->bool
    {
        return me.currentState.pressed[key] and !me.previousState.pressed[key]
    }

    // Determines whether given key is currently being released
    mtd isKeyReleased(key: Key)->bool
    {
        return !me.currentState.pressed[key]
    }

    // Determines whether given key has just been released
    mtd isKeyJustReleased(key: Key)->bool
    {
        return !me.currentState.pressed[key] and me.previousState.pressed[key]
    }

    // Returns the currently pressed key modifiers
    mtd getPressedModifiers()->KeyModifiers
    {
        var modifiers: Input.KeyModifiers = .Zero
        if me.isKeyPressed(Input.Key.LShift):
            modifiers |= .Shift
        if me.isKeyPressed(Input.Key.RShift):
            modifiers |= .Shift
        if me.isKeyPressed(Input.Key.LControl):
            modifiers |= .Control
        if me.isKeyPressed(Input.Key.RControl):
            modifiers |= .Control
        if me.isKeyPressed(Input.Key.LMenu):
            modifiers |= .Alt
        if me.isKeyPressed(Input.Key.RMenu):
            modifiers |= .Alt
        return modifiers
    }
}
