#global testerror "Err0193"
struct(T) A
{
    x:   s32
}

#test
{
    let a = #sizeof(A)
}
