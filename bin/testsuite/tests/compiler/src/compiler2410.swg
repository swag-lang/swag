#global testerror "Err0113"

func(T) toto() #validif @typeof(T) == f32
{
}

func(T) toto() #validif @typeof(T) == f64
{
}

#test
{
    toto's32()
}