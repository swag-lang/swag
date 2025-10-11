#global public
#global namespace Time

// Holds timing information for frame-based updates
struct FrameTiming
{
    dtMin:          f32 = 1 / 100000.0     // Minimum delta time allowed (in seconds)
    dtMax:          f32 = 1                // Maximum delta time allowed (in seconds)

    dt:             f32 = 1 / 60.0         // Current delta time, in seconds
    frameCount:     u32                    // Frame counter

    prevTick:       u64                    // Tick of the previous frame
    paused:         bool                   // Whether timing is currently paused
}

impl FrameTiming
{
    // Updates the delta time and frame count
    mtd update()
    {
        let newTick = nowPrecise()
        if .paused do
            return

        .frameCount += 1

        // First frame
        if !.prevTick
        {
            .prevTick = newTick
            return
        }

        // Delta time
        let diff = newTick - .prevTick
        .dt = cast(f32) (cast(f64) diff / preciseFrequency())
        .dt = Math.clamp(.dt, .dtMin, .dtMax)

        // Setup for next frame
        .prevTick = newTick
    }

    // Pauses frame timing and frame count
    mtd pause()
    {
        .paused = true
    }

    // Resumes frame timing and resets the tick reference
    mtd unpause()
    {
        if !.paused do
            return
        .paused   = false
        .prevTick = nowPrecise()
    }
}
