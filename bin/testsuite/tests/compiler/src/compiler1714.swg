
#global testerror "Err0017"

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