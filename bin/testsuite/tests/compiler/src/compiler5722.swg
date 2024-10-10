#global testerror "Err0024"

#[Swag.Overload]
func toto(x: s32) 
{
    @print("s32")
}

#[Swag.Overload]
func toto(x: s64) 
{
    @print("s64")
}

alias A = toto

#test
{
    A(0's32)
}