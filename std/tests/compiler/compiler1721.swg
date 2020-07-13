#module compiler1721
#unittest error

#test
{
    typealias a: s32
    {
        func A()
        {
            typealias a: s32
        }

        /*func A()
        {
            B()
        }

        func B()
        {

        }*/
    }
}