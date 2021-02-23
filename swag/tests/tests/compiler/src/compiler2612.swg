struct(T) AA
{

}

attr ife(expr: bool);

impl AA
{
    #[swag.constexpr]
    func titi(t: typeinfo)->bool
    {
        return t == s32
    }

    #[ife(titi(@typeof(s32)))]
    func toto()
    {

    }
}

#test
{
    var x: AA's32
}