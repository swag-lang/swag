#global testerror "Err0493"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}