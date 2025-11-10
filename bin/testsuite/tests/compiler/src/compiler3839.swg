struct(T) A {}

impl A
{
    mtd toto()
    {
        @assert(T == *u32)
    }
}

#test
{
    var x: A'*void
    var y: A'*u32
    y.toto()
}
