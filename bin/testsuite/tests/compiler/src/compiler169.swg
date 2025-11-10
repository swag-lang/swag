using Swag

#[ConstExpr]
func main()->s32
{
    //@print("toto\n");
    return 666
}

#run
{
    var r = main()
}
