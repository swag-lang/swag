struct AA
{
}

impl AA
{
    func opCount(using self)->u32 => 4

    func opIndex(using self, idx: u32)->string
    {
        switch idx
        {
            case 0: return "AA"
            case 1: return "BBB"
            case 2: return "CCCC"
            case 3: return "DDDDD"
        }

        return ""
    }
}

func toto(x: string...)
{
}

//#[swag.printbc]
#test
{
    var x: AA
    toto(@spread(x))
}