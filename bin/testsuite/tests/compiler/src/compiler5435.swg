#global skip
struct A {}

#[Swag.PrintBc]
func toto(x: A)
{
    if ref x == null
    {
    }
}
