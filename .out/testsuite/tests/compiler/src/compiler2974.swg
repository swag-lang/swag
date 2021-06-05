enum X: const [..] u32
{
    A = @[1, 2, 3, 4]
    B = @[10, 20]
}

#test
{
    ptr := @typeof(X)
    cpt := 0'uint
    visit p: ptr.values
    {
        ptr1 := cast(const [..] u32) p.value
        @assert(@countof(ptr1) == 4 or @countof(ptr1) == 2)
        cpt += @countof(ptr1)
    }

    @assert(cpt == 6)

    ptr1 := cast(const [..] u32) ptr.values[0].value
    @assert(ptr1[0] == 1)
    @assert(ptr1[1] == 2)
    @assert(ptr1[2] == 3)
    @assert(ptr1[3] == 4)

    ptr2 := cast(const [..] u32) ptr.values[1].value
    @assert(ptr2[0] == 10)
    @assert(ptr2[1] == 20)
}