struct T {}

impl T
{
    mtd opDrop()
    {
        @assert(me == &G)
    }
}

var G: T
