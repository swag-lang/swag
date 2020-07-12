#module compiler1714
#unittest error

#test
{
    {
        typealias point: string
        func toto(x: point)
        {
            typealias point: s32
            var y: point
        }

        toto("string")
    }
}