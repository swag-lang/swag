#global testerror "Err0168"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}