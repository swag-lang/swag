struct Vector
{
    x: s32
}

impl Vector
{
    //#[Swag.printbc, Swag.safety("", false)]
    func max(x, y: Vector)->Vector
    {
        var result: retval = ?
        zz := @max(x.x, y.x)
        @print(zz)
        return result
    }
}

//#[Swag.optim("", false)]
//#[Swag.printbc]
#test
{
    x := Vector{200}
    y := Vector{100}
    x = Vector.max(x, y)
    //discard Vector.min(x, y)
}