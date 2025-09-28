#global testerror "Err0485"

struct Utf8
{
    size: u32
}

impl Utf8
{
    func reserveSize(me)
    {
        Utf8.size = 0
    }
}

#test
{
    var x: Utf8
    x.reserveSize()
}
