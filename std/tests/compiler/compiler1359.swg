interface IPrintable
{
	print: func(self)
}

struct TITI
{
    val: s64 = 666
}

impl IPrintable for TITI
{
    //#[swag.printbc]
    func print(using self)
    {
        @print("TITI\n")
    }
}

//#[swag.printbc]
func printWhatever(_itf: *IPrintable)
{
    _itf.print()
}

//#[swag.printbc]
#test
{   
    var titi: TITI
    var itf: IPrintable
    itf = titi
    printWhatever(itf)
}