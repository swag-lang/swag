#global testerror "Err0084"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}