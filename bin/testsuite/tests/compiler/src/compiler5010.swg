#global testerror "Err0012"

#[Swag.NoCopy]
struct A {}

#test
{
    var a: A
    @postcopy(&a)
}
