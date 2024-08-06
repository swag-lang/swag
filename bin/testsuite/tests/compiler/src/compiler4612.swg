#global testerror "Err0099"
struct A {
    x: s32
}

#test
{
    let a: A
    a.x = 0
}