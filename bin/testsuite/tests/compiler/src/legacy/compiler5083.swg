#global testerror "Err0235"
struct(T) A
{
    x:   s32
}

#test
{
    let a = #alignof(A)
}
