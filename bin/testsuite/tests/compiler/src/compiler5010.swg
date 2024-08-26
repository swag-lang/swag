#global testerror "Err0102"

#[Swag.NoCopy]
struct A{}

#test
{
    var a: A
    @postcopy(&a)
}