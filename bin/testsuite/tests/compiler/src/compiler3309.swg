#global skip
//#global testerror
struct Duration
{
    val: f32
}

impl Duration
{
    mtd(suffix: string, T) opAffectSuffix(v: T)
    {
    }
}

#test
{
    var x: Duration = 5's
}
