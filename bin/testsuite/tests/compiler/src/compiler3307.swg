#global testerror "Err0519"
struct Duration
{
    val: f32
}

impl Duration
{
    mtd(K, V) opAffect(v: K)
    {
    }
}

#test
{
    var x: Duration = 5.0'f32
}
