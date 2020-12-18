interface toto
{
    print: func(self)
}

#assert @sizeof(toto) == @sizeof(*void) * 2