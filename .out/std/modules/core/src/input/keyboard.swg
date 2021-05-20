#global public
#global namespace Input

// Allows getting keystrokes from keyboard
struct Keyboard
{
    previousState:  KeyboardState
    currentState:   KeyboardState
    pressedRepeat:  [@countof(Key)] bool
    pressedTime:    [@countof(Key)] u32

    // Delay, in milliseconds, before a repeated key stroke starts
    pressedRepeatStartTimeMs:   u32 = 400
    // Delay, in miliseconds, between two repeated key strokes
    pressedRepeatTimeMs:        u32 = 150
    // Allow key stroke repetition if true
    canRepeat:                  bool = true
}

impl Keyboard
{
    // Compute current state of the keyboard (all keys)
    func update(using self)
    {
        previousState = currentState
        currentState.update()

        // Repetition
        now := Time.nowMilliseconds()
        loop i: @countof(Key)
        {
            if !self.isKeyPressed(cast(Key) i)
            {
                pressedRepeat[i] = false
                pressedTime[i] = 0
            }
            else if(canRepeat)
            {
                // First pressed time
                if !pressedTime[i]
                    pressedTime[i] = now

                // First repetition
                if !pressedRepeat[i]
                {
                    if (now - pressedTime[i] >= pressedRepeatStartTimeMs)
                    {
                        pressedRepeat[i] = true
                        pressedTime[i]   = now
                        previousState.pressed[i] = false
                    }
                }
                else if now - pressedTime[i] >= pressedRepeatTimeMs
                {
                    pressedTime[i] = now
                    previousState.pressed[i] = false
                }
            }
        }
    }

    // Reset the keyboard state
    func clear(using self)
    {
        previousState.clear()
        currentState.clear()
        Memory.clear(&pressedRepeat, cast(uint) @sizeof(pressedRepeat))
        Memory.clear(&pressedTime, cast(uint) @sizeof(pressedTime))
    }

    // Determines whether given key is currently being pressed
    func isKeyPressed(using self, key: Key)->bool
    {
        return currentState.pressed[key]
    }

    // Determines whether given key has just been pressed
    func isKeyJustPressed(using self, key: Key)->bool
    {
        return currentState.pressed[key] and !previousState.pressed[key]
    }

    // Determines whether given key is currently being released
    func isKeyReleased(using self, key: Key)->bool
    {
        return !currentState.pressed[key]
    }

    // Determines whether given key has just been released
    func isKeyJustReleased(using self, key: Key)->bool
    {
        return !currentState.pressed[key] and previousState.pressed[key]
    }
}
