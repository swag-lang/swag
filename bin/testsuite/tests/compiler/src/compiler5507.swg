#global testerror "Err0254"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}