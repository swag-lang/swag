#global testerror "Err0166"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}