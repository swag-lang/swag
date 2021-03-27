#global testwarning

#[swag.deprecated("you should use this instead")]
func titi()
{
    a := 0
    a += 1
    a += 1
}

#test
{
    titi()
}