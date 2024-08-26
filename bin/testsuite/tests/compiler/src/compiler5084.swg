#global testerror "Err0145"
struct(T) A
{
    x: s32
}

#test
{
    let a = @sizeof(A)
}