//#global skip
struct A {}

var G = 0
impl A
{
    mtd titi()
    {
        G = 1
    }
}

#[Swag.Macro]
func xx()->A
{
    return {}
}

#[Swag.Inline]
func toto()
{
    return xx().titi()
}

//#[Swag.PrintBc]
#test
{
    toto()
}
