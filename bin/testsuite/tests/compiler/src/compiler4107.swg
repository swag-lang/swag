struct T
{
}

impl T
{
    mtd opDrop()
    {
        @assert(self == &G)
    }
}

var G: T