interface TOTO
{
    print: func()
}

#assert @sizeof(TOTO) == 2 * @sizeof(*void)