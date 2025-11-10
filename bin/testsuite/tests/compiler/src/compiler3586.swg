//#global skip
struct A {}

interface I
{
    func toto(me);
}

func(T) titi()
    where #typeof(T).fullname != null
{
}

impl I for A
{
    mtd impl toto()
    {
        titi's32()
    }
}

#test
{
    titi'A()
}
