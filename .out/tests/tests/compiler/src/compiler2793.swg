#global testwarning
#global testwarning
#global testwarning
#global testwarning

#[swag.deprecated("use this instead")]
enum AA
{

}

enum BB
{
    #[swag.deprecated("use this instead")]
    A
}

#[swag.deprecated("use this instead")]
struct CC
{

}

var cc: CC

#[swag.deprecated("use this instead")]
func titi()
{
    a := 0
    a += 1
    a += 1
}

#test
{
    var x: AA = ?
    y := BB.A
    titi()
}