#global testerror "Err0458"
struct A {}

#[Swag.PrintBc]
func toto()
{
    var x: *A
    if moveref x == null
    {
    }
}
