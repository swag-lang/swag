#global testerror "Err0090"
struct A {
    x: s32
}

#test
{
    let a: A
    a.x = 0
}