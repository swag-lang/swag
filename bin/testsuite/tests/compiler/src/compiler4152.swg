#global testwarning "Wrn0006"
#global #[Swag.Warning("Wrn0006", Swag.WarningLevel.Enable)]
#test
{
    let a = 0
    var f = closure|a|(){
    }
}