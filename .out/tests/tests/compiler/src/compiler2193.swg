var G = 0

#[Swag.mixin]
func cstr(x: string)
{
    defer G = 1
}

//#[Swag.printbc]
#test
{
    {
        cstr("toto")
    }

    @assert(G == 1)
}