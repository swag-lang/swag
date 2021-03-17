var G = 0

func(T) toto() #selectif @typeof(T) == f32
{
    G += 1
}

func(T) toto() #selectif @typeof(T) == s32
{
    G += 2
}

#test
{
    toto's32()
    toto'f32()
    @assert(G == 3)
}