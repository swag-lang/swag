//#global skip
var G = 0

#[Swag.Macro]
//func assert(expr: bool, str: string = "toto")
func assert(expr: #code bool, str: string = "toto")
{
    let b = #inject(expr)
    @assert(b)
}

#[Swag.Inline]
func xx(val: typeinfo)->bool
{
    @assert(val == s32)
    return true
}

func(T) toto()
{
    G = 55
    assert(xx(T))
}

//#[Swag.PrintBc]
#test
{
    toto's32()
}
