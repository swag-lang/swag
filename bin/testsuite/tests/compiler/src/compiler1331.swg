interface toto
{
    func print(self)
}

#assert @sizeof(toto) == @sizeof(*void) * 2