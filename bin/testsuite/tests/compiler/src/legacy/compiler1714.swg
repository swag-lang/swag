#global testerror "Err0024"

#test
{
    {
        alias point = string
        func toto(x: point)
        {
            alias point = s32
            var y: point
        }

        toto("string")
    }
}
