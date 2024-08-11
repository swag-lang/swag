#global testerror "Err0112"
struct(T) A
{
    x: s32
}

#test
{
    let a = @sizeof(A)
}