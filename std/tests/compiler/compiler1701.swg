interface Print
{
    toto: func(self)
}

struct A
{
    x: f64 = 666   
}

impl Print for A
{
    func toto(using self)
    {
        @print(x)   
    }
}

func callAny(value: any)
{
    
}

#test
{
    var str: A
    callAny(str)
}