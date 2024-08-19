#global testerror "Err0122"
struct(T) A
{
    x: s32
}

#test
{
    let a = @alignof(A)
}