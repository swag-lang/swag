//#global skip
struct Duration
{
    val: f32
}

impl Duration
{
    #[Swag.Implicit]
    mtd(suffix: string) opAffectLiteral(v: f32)
    {
        @assert(v == 6)
    }
}

func toto(x: Duration) {}

#test
{
    toto(6's)
}
