//#global skip
struct A
{
}

interface I
{
    func toto(self);
}

func(T) titi()
    #validif @typeof(T).fullname != null
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