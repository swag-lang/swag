#test
{
    enum Color
    {
        Red
        Green
        Blue
    }

    var a = Color.Green
    @assert(a == 1)
}

func toto()
{
}

#run toto()