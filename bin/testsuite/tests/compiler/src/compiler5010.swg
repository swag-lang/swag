#global testerror "Err0080"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}