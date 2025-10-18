#global testerror "Err0020"
struct A
{
    x:   s32
}

#test
{
    let a: A
    a.x = 0
}
