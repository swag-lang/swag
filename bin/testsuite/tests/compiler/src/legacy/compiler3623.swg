using Swag
struct Angle
{
    rad: f32
}

impl Angle
{
    #[ConstExpr, Implicit, Inline]
    mtd opAffect(valueRad: s32)
    {
        me.rad = valueRad
    }

    #[ConstExpr, Implicit, Inline]
    mtd(suffix: string) opAffectLiteral(value: s32)
    {
        me.rad = value * 2
    }
}

func toto(x: Angle)
{
    @assert(x.rad == -180)
}

#test
{
    toto(-90'deg)
}
