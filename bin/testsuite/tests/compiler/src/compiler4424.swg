struct(T) A {}

var G = 0

impl A
{
    #[Swag.Overload]
    mtd toto(x: &&T)
    {
        G = 1
    }

    #[Swag.Overload]
    mtd toto(x: T)
    {
        @assert(false)
    }
}

#test
{
    var a: A's32
    var x  = 0's32
    a.toto(moveref &x)
    @assert(G == 1)
}
