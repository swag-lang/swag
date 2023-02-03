//#global skip
struct A
{
}

interface I
{
    toto: func(self)
}

func(T) titi()
    #validif @typeof(T).fullname != null
{
}

impl I for A
{
    mtd toto()
    {
        titi's32()
    }
}

#test
{
    titi'A()
}