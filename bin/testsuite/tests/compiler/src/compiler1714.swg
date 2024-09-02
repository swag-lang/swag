
#global testerror "Err0027"

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