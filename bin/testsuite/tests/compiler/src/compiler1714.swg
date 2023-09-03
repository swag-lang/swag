
#global testerror "Err0116"

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