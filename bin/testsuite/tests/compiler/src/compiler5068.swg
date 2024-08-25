#global testerror "Err0155"

#test
{
    struct A{}
    var a: A
    let b = cast<bit>(u32) a
}