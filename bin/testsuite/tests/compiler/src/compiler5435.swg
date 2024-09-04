#global testerror "Err0327"
struct A {}

#[Swag.PrintBc]
func toto()
{
    var x: *A
    if moveref x == null
    {
    }
}
