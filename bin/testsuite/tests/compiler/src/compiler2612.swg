struct(T) AA
{

}

attr ife(expr: bool);

impl AA
{
    #[Swag.ConstExpr]
    func titi(t: typeinfo)->bool
    {
        return t == s32
    }

    //#[ife(titi(@typeof(T)))]
    func toto()
    {

    }
}

#test
{
    var x: AA's32
    AA's32.toto()
}