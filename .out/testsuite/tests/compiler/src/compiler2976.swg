enum X: const [4] u8
{
    A = @[1, 2, 3, 4]
}

#test
{
    ptr := @typeof(X)

    ptr1 := cast(const *u8) ptr.values[0].value
    @assert(ptr1[0] == 1)
    @assert(ptr1[1] == 2)
    @assert(ptr1[2] == 3)
    @assert(ptr1[3] == 4)
}