#[Swag.ConstExpr]
func getType(x: s32)->typeinfo
{
    if x == 0
        return s32
    else
        return bool
}

#test
{
    var x: @mktype(getType(0))
    @assert(@typeof(x) == s32)
    var x1: @mktype(getType(1))
    @assert(@typeof(x1) == bool)
}