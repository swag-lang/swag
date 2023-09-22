#global testerror "Err0812"
struct(T) A
{
    x: s32
}

#test
{
    let a = @sizeof(A)
}