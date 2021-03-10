func(T) min(x, y: T) => x < y ? x : y

#test
{
    x := min(1's32, 2's32)
    y := min(1'f32, 2'f32)
    x1 := min(1's32, 2's32, 3's32)
}

func(T) min(x, y, z: T) => min(min(x, y), z)
