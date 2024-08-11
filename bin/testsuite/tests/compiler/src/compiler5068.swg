#global testerror "Err0143"

#test
{
    struct A{}
    var a: A
    let b = cast,bit(u32) a
}