
#global testerror "Err0027"

#test
{
    {
        typealias point = string
        func toto(x: point)
        {
            typealias point = s32
            var y: point
        }

        toto("string")
    }
}