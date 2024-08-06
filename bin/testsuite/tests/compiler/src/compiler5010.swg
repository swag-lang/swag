#global testerror "Err0093"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}