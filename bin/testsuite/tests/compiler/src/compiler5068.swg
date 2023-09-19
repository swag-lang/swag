#global testerror "Err0032"

#test
{
    struct A{}
    var a: A
    let b = cast,bit(u32) a
}