#[Swag.Inline]
func toto()
{
    var x = "true"
}

//#[Swag.PrintBc]
#test
{
    var zz = 666

    toto()

    {
        var z = "ZZZZ"
    }
}
