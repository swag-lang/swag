interface I 
{
    mtd toto();
}

impl I for A
{
    mtd impl toto()
    {
        @print("toto")
    }
}

struct A {}

func titi()
{
    var a: A
    a.I.toto()
}