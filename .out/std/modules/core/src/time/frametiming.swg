#global public
#global namespace time

struct FrameTiming
{
    dtMin:      f32 = 1 / 100000.0
    dtMax:      f32 = 1

    dt:         f32 = 1 / 60.0      // Current delta time, in seconds
    frameCount: u32                 // Frame counter

    prevTick:   u64
    paused:     bool
}

impl FrameTiming
{
    // Update frame timers
    func update(using self)
    {
        newTick := nowPrecise()
        if paused
            return

        frameCount += 1

        // First frame
        if !prevTick
        {
            prevTick = newTick
            return
        }

        // Delta time
        diff := newTick - prevTick
        dt = cast(f32) (cast(f64) diff / preciseFrequency())
        dt = Math.clamp(dt, dtMin, dtMax)

        // Setup for next frame
        prevTick = newTick
    }

    // Pause frame timing & count
    func pause(using self)
    {
        paused = true
    }

    // Unpause frame timing & count
    func unpause(using self)
    {
        if !paused
            return
        paused = false
        prevTick = nowPrecise()
    }
}