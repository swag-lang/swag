#global testerror "Err0148"
struct(T) A
{
    x: s32
}

#test
{
    let a = @sizeof(A)
}