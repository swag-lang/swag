var G = 0

#[swag.mixin]
func cstr(x: string)
{
    defer G = 1
}

//#[swag.printbc]
#test
{
    {
        cstr("toto")
    }

    @assert(G == 1)
}