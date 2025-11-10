struct A {}
impl A
{
    mtd toString() => "toto"
}

//#[Swag.PrintBc]
#test
{
    var x: A
    foreach c in x.toString()
    {
    }
}
