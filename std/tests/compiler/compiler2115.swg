struct(T) Arr
{

}

interface IT
{
    toto: func(self)
}

impl IT for Arr
{
    //#[swag.printbc]
    func toto(self)
    {
        @print("@typeof(T).name")
    }
}

//#[swag.printbc]
#test
{
    var x: Arr's32
    var t = cast(IT) x
    t.toto()
}