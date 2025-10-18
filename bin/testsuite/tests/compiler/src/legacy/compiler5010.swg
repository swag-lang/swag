#global testerror "Err0307"

#[Swag.NoCopy]
struct A {}

#test
{
    var a: A
    @postcopy(&a)
}
