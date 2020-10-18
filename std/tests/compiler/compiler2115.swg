struct(T) Arr
{

}

interface IT
{
    toto: func(self)
}

impl IT for Arr
{
    func toto(self)
    {
        @print("@typeof(T).name")
    }
}

#test
{
    var x: Arr's32
    //var t = cast(IT) x
}