#global testerror "Err0172"

#test
{
    struct A{}
    var a: A
    let b = cast,bit(u32) a
}