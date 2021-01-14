//#global testerror

#[swag.compiler, swag.constexpr]
func check(T: typeinfo, N: u32)->bool
{
    return N > 0 && N <= 4 && T == f32
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