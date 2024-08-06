#global testerror "Err0182"

#test
{
    struct A{}
    var a: A
    let b = cast,bit(u32) a
}