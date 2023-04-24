using Swag

struct BitArray
{
    buffer:     ^u32
    allocator:  IAllocator
    count:      u64
    capacity:   u64
}

impl BitArray
{
    mtd opAffect(what: const [..] bool)
    {
        len := @countof(what)
        reserve(len)
    }

#[Swag.NoInline]
//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
    mtd reserve(maxBits: u64)
    {
        count = maxBits
        capacity = maxBits >>,over 5
        if capacity <<,over 5 < maxBits
            capacity += 1

        capacity *= 4
        @assert(capacity > 0)
    }
}

#test
{
    var x: BitArray = [true, false, true, false]
}