/*#[Swag.Overload]
func toto(x: s32)->s32
{
    return x
}

#[Swag.Overload]
func toto(x: s64)->s64
{
    return x * 2
}

alias titi = toto

#test
{
    @assert(titi(1's32) == 1)
    @assert(titi(2's64) == 4)
}*/
