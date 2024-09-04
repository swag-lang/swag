#global testerror "Err0116"
struct(T) A
{
    x: s32
}

#test
{
    let a = #alignof(A)
}