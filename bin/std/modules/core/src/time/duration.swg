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
        .timeInSeconds = valueMs / 1000.0
    }

    #[ConstExpr, Implicit, Inline]
    mtd(suffix: string) opAffectLiteral(value: s32)
    {
        #if suffix == "s" do
            .timeInSeconds = value
        #elif suffix == "ms" do
            .timeInSeconds = value / 1000.0
        #elif suffix == "min" do
            .timeInSeconds = value * 60.0
        #elif suffix == "h" do
            .timeInSeconds = value * 3600.0
        #else do
            #error("invalid duration literal suffix '" ++ suffix ++ "'")
    }

    #[Inline]
    {
        // Returns the value in milliseconds
        mtd const toMs()->u32 => cast(u32) (.timeInSeconds * 1000)

        // Returns a duration initialized with milliseconds
        func fromMs(valueMs: u32)->Duration => Duration{valueMs / 1000.0}
    }
}
