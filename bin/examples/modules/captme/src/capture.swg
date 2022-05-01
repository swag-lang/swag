using Core, Gui, Pixel

struct Capture
{
    width, height: s32
    forms:  Array'IForm
}

impl Capture
{
    mtd paint(painter: *Painter, rect: Math.Rectangle)
    {
        painter.fillRect(rect, Argb.Gray)
        painter.fillRect(rect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))

        zoom := rect.width / width

        painter.setParams(.Antialiased)
        visit i: forms
            i.paint(painter, rect.x, rect.y, zoom)
    }
}