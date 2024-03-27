using Swag

struct BitArray
{
    buffer:        ^u32
    allocator:     IAllocator
    count:         u64
    capacity:      u64
}

impl BitArray
{
    mtd opAffect(what: const [..] bool)
    {
        let len = @countof(what)
        reserve(len)
    }

    #[Swag.NoInline]
    //#[Swag.PrintBc]
    //#[Swag.Optim("", false)]
    mtd reserve(maxBits: u64)
    {
        count    = maxBits
        capacity = maxBits >> 5
        if capacity << 5 < maxBits do
            capacity += 1

        capacity *= 4
        @assert(capacity > 0)
    }
}

#test
{
    var x: BitArray = [true, false, true, false]
}
