#[Swag.mixin]
func scopeLock()
{
    a := 4
    defer G = a
}

var G = 0

//#[Swag.printbc]
#test
{
    {
        scopeLock();
        @assert(a == 4)
        defer a += 1
    }

    @assert(G == 5)
}