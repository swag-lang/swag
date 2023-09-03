#global testerror "Err0478"
struct Duration
{
}

impl Duration
{
    mtd(T) opCast()->T
    {
        return 0's32
    }
}

#test
{
    var x: Duration
    var y = cast(s32) x
}
