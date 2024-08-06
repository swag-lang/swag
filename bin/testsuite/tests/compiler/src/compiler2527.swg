#global testerror "Err0495"

func toto() => 666

#test
{
    var y = 0
    var x = toto() try
}