using Swag
enum TOTO
{
    A = 666
    B = 100
}

func fromName(name: string)
{
    const t = #typeof(TOTO)
    for t.values
    {
        if t.values[@index].name == name
        {
        }
    }
}
