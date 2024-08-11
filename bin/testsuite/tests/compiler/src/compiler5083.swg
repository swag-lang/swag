#global testerror "Err0108"
struct(T) A
{
    x: s32
}

#test
{
    let a = @alignof(A)
}