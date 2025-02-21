#[Swag.NoInline]
//#[Swag.Optimize("", false)]
func(C: s64) toto(a: s64)->s64
{
    return a * C
}

#test
{
    @assert(toto'1(2) == 2)
    @assert(toto'2(2) == 4)
    @assert(toto'3(2) == 6)
    @assert(toto'4(2) == 8)
    @assert(toto'5(2) == 10)
    @assert(toto'6(2) == 12)
}