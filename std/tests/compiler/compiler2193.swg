var G = 0

#[swag.mixin]
func cstr(x: string)
{
    //defer G = 1
    defer @print("coucou")
}

#[swag.printbc]
#test
{
    cstr("toto")
}