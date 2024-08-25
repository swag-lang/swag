struct Thing
{
    number: s32
}

var G = 0

impl Thing
{
    //#[Swag.PrintBc]
    func initxxx(number: s32)->Thing
    {
        //@print("Thing", number, "\n")
        return Thing{number: number}
    }

    mtd opDrop()
    {
        //@print("~Thing", number, "\n")
        G += 1
    }

#[Swag.NoInline]
    mtd(op: string) opBinary(other: Self)->Self
    {
        return {number + other.number}
    }
}

struct Team
{
    a: Thing
}

//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
#test
{
    {
        var team = Team{
            a: Thing.initxxx(1) + Thing.initxxx(2),
        }
    }

    @assert(G == 3)
}