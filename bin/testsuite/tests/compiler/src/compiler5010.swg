#global testerror "Err0092"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}