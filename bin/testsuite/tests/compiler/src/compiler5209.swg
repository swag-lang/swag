struct A
{
    x:     s64 = 66
    y:     s64 = 66
}

impl A
{
    mtd opPostCopy()
    {
        @assert(false)
    }
}

//#[Swag.PrintBc]
func toto(val: any)
{
    let err = cast(const &A) val
    @assert(err.x == 42)
}

#test
{
    toto(A{42})
}
