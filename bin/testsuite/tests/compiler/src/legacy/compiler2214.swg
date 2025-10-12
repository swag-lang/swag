struct A
{
    x    = 0's64
}

var G = 0

impl A
{
    func opDrop(me)
    {
        G += 1
    }

    func opPostMove(me)
    {
        @assert(false)
    }

    func opPostCopy(me)
    {
        @assert(false)
    }
}

#[Swag.Optimize(.All, true)]
func toto()->A
{
    var res: retval
    res.x = 1
    return res
}

//#[Swag.PrintBc]
#[Swag.Optimize(.All, true)]
func titi()->A
{
    return toto()
}

//#[Swag.PrintBc]
#[Swag.Optimize(.All, true), Swag.Warning("Wrn0003", .Disable)]
#test
{
    if @bytecode and @compiler.getBuildCfg().byteCodeOptimizeLevel > Swag.BuildCfgByteCodeOptim.O1
    {
        var x = titi()
        @assert(x.x == 1)
        @assert(G == 0)
    }

    //@assert(G == 1)
}
