#global testerror "Err0146"
struct(T) A
{
    x: s32
}

#test
{
    let a = @alignof(A)
}