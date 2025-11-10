#global testerror "Err0378"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}
