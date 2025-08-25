#global marked
#global public
#global namespace Input

// Represents one keyboard state
struct KeyboardState
{
    using native:     KeyboardStateNative
    pressed:          [@countof(Key)] bool
}

impl KeyboardState
{
    // Compute current keyboard state
    mtd update()
    {
        me.updateNative()
        for i in @countof(Key):
            me.pressed[i] = me.native.isKeyPressed(cast(Key) i)
    }

    // Reset keyboard state
    mtd clear()
    {
        Memory.clear(&me.pressed, #sizeof(me.pressed))
    }
}
