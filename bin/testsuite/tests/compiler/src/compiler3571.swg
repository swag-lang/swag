#global testerror "Err0343"

struct A
{
}

impl A
{
}

#test
{
    var xxxx: A
    xxxx += func(a, b)
    {
    }
}
