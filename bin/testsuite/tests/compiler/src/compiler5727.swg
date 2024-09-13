#global testerror "Err0728"

//interface I {}
struct A {}
//impl I for A {}

#test
{
    //let a: A
    //let itf: I = a

    //let b = @as(A, @kindof(itf), @dataof(itf))
    //let c = itf is A
    let d = 0 is A
}