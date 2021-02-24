struct(T) AA
{

}

impl AA
{
    #[swag.compileif(@typeof(T) == @typeof(s32))]
    func toto()
    {
        #assert @typeof(T) == s32
    }
}

#test
{
    var x: AA's32
    x.toto()
}