#global skip
struct AA {}
struct BB {}

func(T) create()
{
    create(0)
}

func(T) create(x: s32)
{
}

#test
{
    create'AA(0)
    create'BB(0)
    create'AA()
}