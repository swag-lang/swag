#global public
#global namespace Time

struct FrameTiming
{
    dtMin:          f32 = 1 / 100000.0
    dtMax:          f32 = 1

    dt:             f32 = 1 / 60.0         // Current delta time, in seconds
    frameCount:     u32                    // Frame counter

    prevTick:       u64
    paused:         bool
}

impl FrameTiming
{
    // Update frame timers
    mtd update()
    {
        let newTick = nowPrecise()
        if paused:
            return

        frameCount += 1

        // First frame
        if !prevTick
        {
            prevTick = newTick
            return
        }

        // Delta time
        let diff = newTick - prevTick
        dt = cast(f32) (cast(f64) diff / preciseFrequency())
        dt = Math.clamp(dt, dtMin, dtMax)

        // Setup for next frame
        prevTick = newTick
    }

    // Pause frame timing & count
    mtd pause()
    {
        paused = true
    }

    // Unpause frame timing & count
    mtd unpause()
    {
        if !paused:
            return
        paused   = false
        prevTick = nowPrecise()
    }
}
