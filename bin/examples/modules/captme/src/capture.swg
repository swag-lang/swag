using Core, Gui, Pixel

struct Capture
{
    width, height: s32
}

impl Capture
{
    mtd paint(painter: *Painter, rect: Math.Rectangle)
    {
        painter.fillRect(rect, Argb.Gray)
        painter.fillRect(rect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
    }
}