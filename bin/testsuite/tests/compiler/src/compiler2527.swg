#global testerror "Err0452"

func toto() => 666

#test
{
    var y = 0
    var x = toto() try
}