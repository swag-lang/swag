//#global skip
func(T) toto()
{
    func titi()
    {
        func txtx()
        {
            tata()
        }

        txtx()
    }

    func tata()
    {
        var x: T
        @assert(x == 0)
    }

    titi()
}

#test
{
    toto's32()
    toto'f32()
}
