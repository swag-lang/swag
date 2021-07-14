//#global testerror

#[Swag.compiler, Swag.ConstExpr]
func check(T: typeinfo, N: u32)->bool
{
    return N > 0 and N <= 4 and T == f32
}

func(T, N: u32) toto()
#selectif
{
    #assert(check(T, N), "N is out of range")
    return true
}
{
}

#test
{
    toto'(f32, 4)()
}