#global testerror "Err0081"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}