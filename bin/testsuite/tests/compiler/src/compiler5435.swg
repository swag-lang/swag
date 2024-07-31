#global testerror "Err0465"
struct A {}

#[Swag.PrintBc]
func toto()
{
    var x: *A
    if moveref x == null
    {
    }
}
