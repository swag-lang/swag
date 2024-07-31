#global testerror "Err0098"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}