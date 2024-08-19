#global testerror "Err0744"
struct Duration
{
    val: f32
}

impl Duration
{
    mtd(T) opAffect(v: f32)
    {
    }
}

#test
{
    var x: Duration = 5.0'f32
}
