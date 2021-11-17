#global public
using Core

struct Timer
{
    timeMs:     s32
    target:     *Wnd
    stopwatch:  Time.Stopwatch
}

impl Timer
{
    #[Swag.Inline]
    mtd restart()
    {
        stopwatch.reset()
    }
}