#global testerror "Err0120"
struct(T) A
{
    x: s32
}

#test
{
    let a = @alignof(A)
}