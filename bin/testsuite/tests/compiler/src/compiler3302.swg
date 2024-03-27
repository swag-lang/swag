#global testerror "Err0342"
struct Duration
{
    val: f32
}

impl Duration
{
}

#test
{
    var x: Duration
    x = 1.0's
    @assert(x.val == 1000.0)
}
