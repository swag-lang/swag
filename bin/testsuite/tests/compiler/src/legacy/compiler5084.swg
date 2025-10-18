#global testerror "Err0271"
struct(T) A
{
    x:   s32
}

#test
{
    let a = #sizeof(A)
}
