#global testerror "Err0123"
struct(T) A
{
    x: s32
}

#test
{
    let a = @alignof(A)
}