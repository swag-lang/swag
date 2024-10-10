#global testerror "Err0561"
struct Duration
{
    val: f32
}

impl Duration
{
    #[Swag.Implicit]
    mtd(suffix: string) opAffectLiteral(v: f32)
    {
    }
}

func toto(x: Duration) {}

#test
{
    toto(6.0)
}
