struct AA
{
    x: s64 = 1
}

var G: s32

impl AA
{
    func opPostMove(using self)
    {
        G = 1
    }

    func opPostCopy(using self)
    {
        @assert(false)
    }

    func opDrop(using self)
    {
        //@print(x)
        //@assert(x == 1)
        x = 666
    }
}

func toto()->{x: AA}
{
    var result: retval
    return result
}

//#[Swag.PrintBc]
#[Swag.Optim("bytecode", true)]
#test
{
    {
        //var x: AA
        var (x) = toto()
        @assert(G == 0)
    }
}
