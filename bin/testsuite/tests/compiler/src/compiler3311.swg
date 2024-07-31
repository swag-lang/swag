#global testerror "Err0043"
struct Duration
{
    val: f32
}

impl Duration
{
    #[Swag.Implicit]
    mtd(suffix: string) opAffectLiteral(v: f32)
    {
        #assert false
    }
}

func toto(x: Duration) {}

#test
{
    toto(6.0's)
}
