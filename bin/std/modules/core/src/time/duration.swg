#global public
#global namespace Time
using Swag

// Represents a delay, expressed in seconds.
struct Duration
{
    timeInSeconds: f32 // The duration in seconds
}

impl Duration
{
    #[ConstExpr, Implicit, Inline]
    mtd opAffect(valueMs: s32)
    {
        me.timeInSeconds = valueMs / 1000.0
    }

    #[ConstExpr, Implicit, Inline]
    mtd(suffix: string) opAffectLiteral(value: s32)
    {
        #if suffix == "s":
            me.timeInSeconds = value
        #elif suffix == "ms":
            me.timeInSeconds = value / 1000.0
        #elif suffix == "min":
            me.timeInSeconds = value * 60.0
        #elif suffix == "h":
            me.timeInSeconds = value * 3600.0
        #else:
            #error("invalid duration literal suffix '" ++ suffix ++ "'")
    }

    #[Inline]
    {
        // Returns the value in milliseconds
        mtd const toMs()->u32 => cast(u32) (me.timeInSeconds * 1000)

        // Returns a duration initialized with milliseconds
        func fromMs(valueMs: u32)->Duration => Duration{valueMs / 1000.0}
    }
}
