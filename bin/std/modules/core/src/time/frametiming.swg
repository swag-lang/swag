#global marked
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
        if me.paused:
            return

        me.frameCount += 1

        // First frame
        if !me.prevTick
        {
            me.prevTick = newTick
            return
        }

        // Delta time
        let diff = newTick - me.prevTick
        me.dt = cast(f32) (cast(f64) diff / preciseFrequency())
        me.dt = Math.clamp(me.dt, me.dtMin, me.dtMax)

        // Setup for next frame
        me.prevTick = newTick
    }

    // Pauses frame timing and frame count
    mtd pause()
    {
        me.paused = true
    }

    // Resumes frame timing and resets the tick reference
    mtd unpause()
    {
        if !me.paused:
            return
        me.paused   = false
        me.prevTick = nowPrecise()
    }
}
