#global testwarning
#global testwarning
#global testwarning
#global testwarning

#[Swag.Deprecated("use this instead")]
enum AA
{

}

enum BB
{
    #[Swag.Deprecated("use this instead")]
    A
}

#[Swag.Deprecated("use this instead")]
struct CC
{

}

var cc: CC

#[Swag.Deprecated("use this instead")]
func titi()
{
    a := 0
    a += 1
    a += 1
}

//#[Swag.PrintBc]
#test
{
    var x: AA = ?
    y := BB.A
    titi()
}
