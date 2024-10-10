#global testerror "Err0119"
struct(T) A
{
    x: s32
}

#test
{
    let a = #alignof(A)
}