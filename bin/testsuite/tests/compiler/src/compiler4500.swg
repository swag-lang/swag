using Swag

struct BitArray
{
    buffer:        [*] u32
    allocator:     IAllocator
    count:         u64
    capacity:      u64
}

impl BitArray
{
    mtd opAffect(what: const [..] bool)
    {
        let len = @countof(what)
        me.reserve(len)
    }

    #[Swag.NoInline]
    //#[Swag.PrintBc]
    //#[Swag.Optimize(.All, false)]
    mtd reserve(maxBits: u64)
    {
        me.count    = maxBits
        me.capacity = maxBits >> 5
        if me.capacity << 5 < maxBits do
            me.capacity += 1

        me.capacity *= 4
        @assert(me.capacity > 0)
    }
}

#test
{
    var x: BitArray = [true, false, true, false]
}
