#[swag.inline]
func toto()
{
    x := "true"
    //@print("toto")
}

//#[swag.printbc]
#test
{
    zz := 666

    toto()

    {
        z := "ZZZZ"
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
        x := 1
        x = 10
        {
            z := 2
            z = 20
            {
                a := "toto"
                a = "titi"
            }
        }
    }*/
}