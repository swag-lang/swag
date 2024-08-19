#global testerror "Err0155"

#test
{
    struct A{}
    var a: A
    let b = cast(u32) #bit a
}