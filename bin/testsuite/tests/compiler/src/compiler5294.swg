struct(T) Array
{
    count: u64 = 5
}

impl Array
{
    mtd opCount()->u64
    {
        return 2
    }

    mtd opSlice(a, b: u64)->const [..] T
    {
        return [1, 2, 3]
    }

    mtd opDrop()
    {
        @assert(count == 5)
    }
}

func toto()->Array'u32
{
    var r: retval
    return r
}

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    foreach v in toto()[1..]:
        @assert(v > 0)
}