struct A
{
}
impl A
{
    mtd toString() => "toto"
}

//#[Swag.PrintBc]
#test
{
    var x: A
    visit c: x.toString()
    {
    }
}
