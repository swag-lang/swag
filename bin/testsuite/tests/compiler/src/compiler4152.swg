#global testwarning "wrn0002"
#global #[Swag.Warn("wrn0002", Swag.WarnLevel.Enable)]
#test
{
    let a = 0
    var f = closure|a|(){
    }
}