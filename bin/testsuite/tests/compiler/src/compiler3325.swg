//#global testerror
#test
{
    #[Swag.Compiler]
    func init()->u64
    {
        var sum = 0'u64
        loop i in 3
        {
            var T: typeinfo = undefined
            switch i
            {
            case 0: T = f32
            case 1: T = s8
            case 2: T = bool
            }

            sum += @countof(T.name)
        }

        return sum
    }

    const A = #run init()
    #assert A == 9
}