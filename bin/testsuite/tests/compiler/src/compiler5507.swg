#global testerror "Err0170"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}