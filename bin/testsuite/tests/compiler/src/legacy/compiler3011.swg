struct Vector
{
    x: s32
}

impl Vector
{
    //#[Swag.PrintBc, Swag.Safety("", false)]
    func max(x, y: Vector)->Vector
    {
        var result: retval// = undefined
        var zz = @max(x.x, y.x)
        //@print(zz)
        return result
    }
}

//#[Swag.Optimize("", false)]
//#[Swag.PrintBc]
#test
{
    var x = Vector{200}
    var y = Vector{100}
    discard Vector.max(x, y)
    //discard Vector.min(x, y)
}