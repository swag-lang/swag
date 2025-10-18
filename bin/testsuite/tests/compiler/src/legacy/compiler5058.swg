#global testerror "Err0365"

func toto() => "toto"

#test
{
    let a = #hastag(toto())
}
