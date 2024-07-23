//#global testerror

#[Swag.Compiler, Swag.ConstExpr]
func toCheck(T: typeinfo, N: u32)->bool
{
    return N > 0 and N <= 4 and T == f32
}

func(T, N: u32) toto()
where
{
    #if !toCheck(T, N) #do #error "N is out of range"
    return true
}
{
}

#test
{
    toto'(f32, 4)()
}