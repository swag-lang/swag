func toto(x: ...) => 666

#test
{
    loop 10
    {
        x := toto(1)
    }
}