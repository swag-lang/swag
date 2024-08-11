#global testerror "Err0110"
struct(T) A
{
    x: s32
}

#test
{
    let a = @sizeof(A)
}