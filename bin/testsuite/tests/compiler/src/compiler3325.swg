//#global testerror
#test
{
    #[Swag.Compiler]
    func init()->uint
    {
        sum := 0'uint
        loop i: 3
        {
            var T: typeinfo = ?
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