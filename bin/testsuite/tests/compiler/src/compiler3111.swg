struct Vector2 { x, y: f32 }

const Max = Vector2{1, 1}
#test
{
    var x = Max
    x.x = 0
}
