#global testwarning "wrn0006"
#global #[Swag.Warn("wrn0006", Swag.WarnLevel.Enable)]
#test
{
    let a = 0
    var f = closure|a|(){
    }
}