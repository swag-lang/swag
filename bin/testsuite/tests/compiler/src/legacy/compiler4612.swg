#global testerror "Err0093"
struct A
{
    x:   s32
}

#test
{
    let a: A
    a.x = 0
}
