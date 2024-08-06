#global testerror "Err0124"
struct(T) A
{
    x: s32
}

#test
{
    let a = @alignof(A)
}