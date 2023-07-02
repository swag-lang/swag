#global public
#global namespace Time
using Swag

struct Duration
{
    timeInSeconds: f32
}

impl Duration
{
    #[ConstExpr, Implicit, Inline]
    mtd opAffect(valueMs: s32)
    {
        timeInSeconds = valueMs / 1000.0
    }

    #[ConstExpr, Implicit, Inline]
    mtd(suffix: string) opAffectSuffix(value: s32)
    {
        #if suffix == "s"
            timeInSeconds = value
        #elif suffix == "ms"
            timeInSeconds = value / 1000.0
        #elif suffix == "min"
            timeInSeconds = value * 60.0
        #elif suffix == "h"
            timeInSeconds = value * 3600.0
        #else
            #error "invalid duration literal suffix '" ++ suffix ++ "'"
    }

    #[Inline]
    mtd const toMs()->u32 => cast(u32) (timeInSeconds * 1000)
}