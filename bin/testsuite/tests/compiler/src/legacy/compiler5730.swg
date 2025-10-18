#global testerror "Err0031"

interface I
{
}
struct A {}
impl I for A
{
}

#test
{
    let a:   A
    let itf: I = a

    //let b = @as(A, @kindof(itf), @dataof(itf))
    //let c = itf is A
    let d = itf as s32
}
