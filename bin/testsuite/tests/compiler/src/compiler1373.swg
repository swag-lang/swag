interface TOTO
{
    func print(self);
}

#assert(#sizeof(TOTO) == 2 * #sizeof(*void))