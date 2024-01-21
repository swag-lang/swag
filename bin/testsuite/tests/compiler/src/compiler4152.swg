#global testwarning "Wrn0006"
#global #[Swag.Warn("Wrn0006", Swag.WarnLevel.Enable)]
#test
{
    let a = 0
    var f = closure|a|(){
    }
}