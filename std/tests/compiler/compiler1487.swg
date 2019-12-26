#[swag.macro]
func(op: bool) toto()
{
    #if op
        @print("true\n")
    #else
        @print("false\n")
}

#test
{
    toto!true()
    toto!false()
}