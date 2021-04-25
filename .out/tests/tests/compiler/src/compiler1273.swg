var g: u8 = cast(u8) "c"'rune

var charCategory: [255] u8 = ?

//#[swag.printbc]
func getCategory(c: u32)->u8
{
    return charCategory[c]
}