#global testwarning
#global #[Swag.Warn("wrn0006", Swag.WarnLevel.Enable)]
#test
{
    a := 0
    f := closure|a|(){
    }
}