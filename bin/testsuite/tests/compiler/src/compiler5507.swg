#global testerror "err0259"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}