#global skip
var G = 0

#[Swag.Macro]
func assert(expr: code, str: string = "toto")
{
    //@print("before")
    let b = #inject expr
    @assert(b == G)
    //@print("after")
}

#[Swag.Inline]
func xx(val: s32)->s32
{
    G = 55
    return val
}

#[Swag.Macro]
func xx1(val: s32)->s32
{
    G = 55
    return val
}

//#[Swag.PrintBc]
#test
{

    assert(xx(55))
    assert(xx1(56))
}