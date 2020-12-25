public namespace time;

struct Frame
{
    dtMin:      f32 = 1 / 100000.0
    dtMax:      f32 = 1

    dt:         f32 = 1 / 60.0      // Current delta time, in seconds
    counter:    u32                 // Frame counter

    prevTick:   u64
    paused:     bool
}

impl Frame
{
    // Update frame timers
    func update(using self)
    {
        if paused
            return

        counter += 1
        newTick := nowPrecise()

        // First frame
        if !prevTick
        {
            prevTick = newTick
            return
        }

        diff := newTick - prevTick
        dt = cast(f32) (cast(f64) diff / preciseFrequency())
        dt = math.clamp(dt, dtMin, dtMax)

        prevTick = newTick
    }

    func pause(using self)
    {
        paused = true
    }

    func unpause(using self)
    {
        paused = false
        prevTick = nowPrecise()
    }
}