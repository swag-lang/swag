#global testerror "Err0745"
struct(T) A
{
    x:   s32
}

#test
{
    let a = #alignof(A)
}
