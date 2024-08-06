#global testerror "Err0441"
struct A {}

#[Swag.PrintBc]
func toto()
{
    var x: *A
    if moveref x == null
    {
    }
}
