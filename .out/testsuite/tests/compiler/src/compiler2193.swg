var G = 0

#[Swag.Mixin]
func cstr(x: string)
{
    defer G = 1
}

//#[Swag.PrintBc]
#test
{
    {
        cstr("toto")
    }

    @assert(G == 1)
}