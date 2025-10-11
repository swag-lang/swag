enum A { R, G, B }

func toto(x: s32) {}

func titi()
{
    func toto(x: A) {}

    toto(.R)
}
