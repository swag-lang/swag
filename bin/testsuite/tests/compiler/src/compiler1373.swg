interface TOTO
{
    print: func(self)
}

#assert @sizeof(TOTO) == 2 * @sizeof(*void)