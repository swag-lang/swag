#global testwarning
#global #[Swag.Warn("wrn0006", Swag.WarnLevel.Enable)]
#test
{
    var a = 0
    var f = closure|a|(){
    }
}