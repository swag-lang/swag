var g: bool = false

#[Swag.Inline]
func toto()->s32
{
    if g do
        return 1
    else do
        return 0
}

//#[Swag.PrintBc]
#test
{
    g = true
    @assert(toto() == 1)
    g = false
    @assert(toto() == 0)
}
