#[Swag.Inline]
func toto()
{
    var x = "true"
    //@print("toto")
}

//#[Swag.PrintBc]
#test
{
    var zz = 666

    toto()

    {
        var z = "ZZZZ"
        //@print("toto")
    }

    /*@print("toto")
    @print("toto")
    @print("toto")
    @print("toto")
    @print("toto")
    @print("toto")
    @print("toto")
    @print("toto")
    @print("toto")

    {
        var x = 1
        x = 10
        {
            var z = 2
            z = 20
            {
                var a = "toto"
                a = "titi"
            }
        }
    }*/
}
