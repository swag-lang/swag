#global testerror "Err0154"
struct(T) A
{
    x: s32
}

#test
{
    let a = @sizeof(A)
}