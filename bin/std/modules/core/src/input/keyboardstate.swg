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
        self.updateNative()
        for i in @countof(Key):
            self.pressed[i] = self.native.isKeyPressed(cast(Key) i)
    }

    // Reset keyboard state
    mtd clear()
    {
        Memory.clear(&self.pressed, #sizeof(self.pressed))
    }
}
