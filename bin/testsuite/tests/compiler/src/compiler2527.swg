#global testerror "Syn0037"

func toto() => 666

#test
{
    var y = 0
    var x = toto() try
}