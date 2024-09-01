#global testerror "Err0143"
struct(T) A
{
    x: s32
}

#test
{
    let a = #alignof(A)
}