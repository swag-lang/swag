struct Thing
{
    number: s32
}

impl Thing
{
    func init(number: s32)->Thing
    {
        //@print("Thing", number, "\n")
        return Thing{number: number}
    }

    mtd opDrop()
    {
        //@print("~Thing", number, "\n")
    }
}

struct Team { a, b: Thing }

#run
{
    var team = Team{a: Thing.init(1), b: Thing.init(2)}
}
