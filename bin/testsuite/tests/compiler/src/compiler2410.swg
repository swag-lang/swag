#global testerror

func(T) toto() #selectif @typeof(T) == f32
{
}

func(T) toto() #selectif @typeof(T) == f64
{
}

#test
{
    toto's32()
}