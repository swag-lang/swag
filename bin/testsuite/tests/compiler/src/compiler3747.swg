#[Swag.Mixin]
func titi(x: string)
{
    defer @assert(x == "a")
}

func(T) toto() throw
{
    titi("a")
}

#test
{
    try toto's32()
}
