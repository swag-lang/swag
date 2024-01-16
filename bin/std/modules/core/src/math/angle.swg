#global public
#global namespace Math
using Swag

struct Angle
{
    rad: f32
}

impl Angle
{
    #[ConstExpr, Implicit, Inline]
    mtd opAffect(valueRad: f32)
    {
        rad = valueRad
    }

    #[ConstExpr, Implicit, Inline]
    mtd(suffix: string) opAffectSuffix(value: f32)
    {
        #if suffix == "rad" #do
            rad = value
        #elif suffix == "deg" #do
            rad = toRadians(value)
        #else #do
            #error "invalid angle literal suffix [[" ++ suffix ++ "]]"
    }

    #[Inline]
    mtd const toDegrees()->f32 => toDegrees(rad)
}