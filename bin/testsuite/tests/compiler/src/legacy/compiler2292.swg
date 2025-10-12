struct AA
{
    x:   s64 = 1
}

var G: s32

impl AA
{
    func opPostMove(me)
    {
        G = 1
    }

    func opPostCopy(me)
    {
        @assert(false)
    }

    func opDrop(me)
    {
        //@print(x)
        //@assert(x == 1)
        me.x = 666
        //@print("drop")
    }
}

func toto()->{ x: AA }
{
    var result: retval
    return result
}

//#[Swag.PrintBc]
#[Swag.Optimize(.ByteCode, true)]
#test
{
    if @bytecode and @compiler.getBuildCfg().byteCodeOptimizeLevel > Swag.BuildCfgByteCodeOptim.O1
    {
        //var x: AA
        var (x) = toto()
        @assert(G == 0)
    }
}
