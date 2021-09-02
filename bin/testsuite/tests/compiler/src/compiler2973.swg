enum X: const [4] u8
{
    A = @[1, 2, 3, 4]
    B = @[10, 20, 30, 40]
}

#test
{
    ptr := @typeof(X)
    cpt := 0
    visit p: ptr.values
    {
        ptr1 := cast(const*u8) p.value
        cpt += dref ptr1
    }

    @assert(cpt == 11)
}