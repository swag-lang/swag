#global testerror "Err0545"

func toto() => "toto"

#test
{
    let a = #hastag(toto())
}
