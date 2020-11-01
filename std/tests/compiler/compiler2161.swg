namespace std.debug
{
    func assert(v: bool)
    {

    }
}

namespace std.file
{
    struct S
    {

    }

    impl S
    {
        func titi(using self)=>true
    }
}

namespace std.file
{
    func tata(x: S)
    {
        debug.assert(x.titi())
    }
}
