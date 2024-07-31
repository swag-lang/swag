#global testerror "Err0151"
struct(T) A
{
    x: s32
}

#test
{
    let a = @sizeof(A)
}