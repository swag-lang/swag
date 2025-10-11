interface toto
{
    func print(me);
}

#assert(#sizeof(toto) == #sizeof(*void) * 2)
