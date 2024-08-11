#global testerror "Err0083"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}