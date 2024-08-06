#global testerror "Err0127"
struct(T) A
{
    x: s32
}

#test
{
    let a = @sizeof(A)
}