#global public
#global namespace Input

// Represents one keyboard state
struct KeyboardState
{
    pressed: [@countof(Key)] bool
}

impl KeyboardState
{
    // Compute current keyboard state
    mtd update()
    {
        loop i: @countof(Key)
        {
            pressed[i] = isKeyPressed(cast(Key) i)
        }
    }

    // Reset keyboard state
    mtd clear()
    {
        Memory.clear(&pressed, cast(uint) @sizeof(pressed))
    }
}
