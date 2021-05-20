#global testwarning
#global testwarning
#global testwarning
#global testwarning

#[Swag.deprecated("use this instead")]
enum AA
{

}

enum BB
{
    #[Swag.deprecated("use this instead")]
    A
}

#[Swag.deprecated("use this instead")]
struct CC
{

}

var cc: CC

#[Swag.deprecated("use this instead")]
func titi()
{
    a := 0
    a += 1
    a += 1
}

//#[Swag.printbc]
#test
{
    var x: AA = ?
    y := BB.A
    titi()
}
