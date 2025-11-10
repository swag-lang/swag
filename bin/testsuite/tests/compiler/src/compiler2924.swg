#[Swag.Overload]
func(T) toto(x: T)
{
    @assert(false)
}

#[Swag.Overload]
func(T) toto(x: const [..] T) {}

#test
{
    var x = 0
    toto(@mkslice(cast(const [*] s32) &x, 1))
}
