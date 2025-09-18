#global public
using Core, Pixel

struct Item
{
    name:          RichString
    icon:          Icon
    id:            WndId
    userData0:     *void
    userData1:     *void
}

#[Swag.EnumFlags]
enum PaintItemFlags
{
    Zero     = 0
    Checked
    Pressed
    Hot
    Focus
    Disabled
}
