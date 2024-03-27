#[Swag.ConstExpr, Swag.Mixin]
func isF32(t: typeinfo)->bool
{
    //@compilererror("toto", @location(t))
    return t == f32
}

//#global testerror
#test
{
    //var g: bool = isF32(bool)
    func toto(x: bool = isF32(s32))
    {
        @assert(x == false)
    }

    toto()
}
