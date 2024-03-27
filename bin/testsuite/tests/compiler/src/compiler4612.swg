#global testerror "Err0104"
struct A {
    x: s32
}

#test
{
    let a: A
    a.x = 0
}