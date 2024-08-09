#global testerror "Err0091"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}