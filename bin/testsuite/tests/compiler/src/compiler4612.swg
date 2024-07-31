#global testerror "Err0101"
struct A {
    x: s32
}

#test
{
    let a: A
    a.x = 0
}