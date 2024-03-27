#global testerror "Err0149"
struct(T) A
{
    x: s32
}

#test
{
    let a = @alignof(A)
}
