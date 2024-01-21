#global testerror "Err0649"
struct Duration
{
    val: f32
}

impl Duration
{
    #[Swag.Implicit]
    mtd(suffix: string) opAffectSuffix(v: f32)
    {
    }
}

func toto(x: Duration) {}

#test
{
    toto(6.0)
}
