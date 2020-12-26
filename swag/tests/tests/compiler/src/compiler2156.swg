
#unittest error

struct A
{

}

impl A
{
    func toto(using self, x: A)->s32
    {
        return 1
    }

    func toto(x, y: A)->s32
    {
        return 2
    }
}

#test
{
    var a, b: A
    @assert(a.toto(a, b) == 1)
}