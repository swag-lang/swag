interface TOTO
{
    func print(me);
}

#assert(#sizeof(TOTO) == 2 * #sizeof(*void))
