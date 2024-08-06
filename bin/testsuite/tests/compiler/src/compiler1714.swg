
#global testerror "Err0028"

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