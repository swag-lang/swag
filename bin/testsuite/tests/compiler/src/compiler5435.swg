#global testerror "Err0407"
struct A {}

#[Swag.PrintBc]
func toto()
{
    var x: *A
    if moveref x == null
    {
    }
}
