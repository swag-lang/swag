public interface toto
{
    print: func()
}

#assert @sizeof(toto) == @sizeof(*void) * 2