#global testerror "Err0101"

#[Swag.NoCopy]
struct A
{
}

#test
{
    var a: A
    @postcopy(&a)
}
