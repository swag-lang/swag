#global testerror

func toto() => 666

#test
{
    y := 0
    x := toto() try
}