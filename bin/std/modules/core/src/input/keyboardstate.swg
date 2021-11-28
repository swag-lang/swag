#global public
#global namespace Input

// Represents one keyboard state
struct KeyboardState
{
    using native:   KeyboardStateNative
    pressed:        [@countof(Key)] bool
}

impl KeyboardState
{
    // Compute current keyboard state
    mtd update()
    {
        updateNative()
        loop i: @countof(Key)
            pressed[i] = Keyboard.isKeyPressed(cast(Key) i)
    }

    // Reset keyboard state
    mtd clear()
    {
        Memory.clear(&pressed, cast(uint) @sizeof(pressed))
    }
}
