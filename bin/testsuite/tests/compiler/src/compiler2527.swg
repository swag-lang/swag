#global testerror "Err0447"

func toto() => 666

#test
{
    var y = 0
    var x = toto() try
}