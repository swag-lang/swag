struct Vector2
{
    x: f32
}

#test
{
    size := 10's32
    v := Vector2{1.0 + size}
    @print(v.x)
}