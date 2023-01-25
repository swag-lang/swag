#global testerror

func(T) toto() #selectifonce @typeof(T) == f32
{
}

func(T) toto() #selectifonce @typeof(T) == f64
{
}

#test
{
    toto's32()
}