struct A
{
    x: const ref s32 = undefined
}

#test
{
    c := 42
    a := A{&c}
    @assert(a.x == 42)
}