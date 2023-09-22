#global testerror "Err0814"
struct(T) A
{
    x: s32
}

#test
{
    let a = @alignof(A)
}