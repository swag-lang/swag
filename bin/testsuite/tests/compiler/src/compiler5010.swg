#global testerror "Err0090"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}