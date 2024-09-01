/*#[Swag.Mixin]
func checkInteger(t: typeinfo, l = #callerlocation)->bool
{
    if t.flags & .Integer
        return true
    @compilererror("expected an integer, '" ++ t.name ++ "' provided", l)
    return false
}

#[Swag.Mixin]
func checkFloat(t: typeinfo, l = #callerlocation)->bool
{
    if t.flags & .Integer
        return true
    @compilererror("expected a float, '" ++ t.name ++ "' provided", l)
    return false
}

func(T) add(x, y: T)->T
    where checkInteger(T, @location(x)) or
              checkFloat(T, @location(x))
{
    return x + y
}

#test
{
    var v = add(1.0'f32, 2)
}*/
#global testerror "type bool not supported"
struct(T) S
{
    #if T != s32: #error "type " ++ #typeof(T).name ++ " not supported"
    x: T
}

#test
{
    var s: S'bool
}