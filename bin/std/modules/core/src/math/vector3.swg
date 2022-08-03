#global namespace Math
#global public

#[Serialization.PodFinal]
struct Vector3
{
    x, y, z: f32
}

impl Vector3
{
    const Zero: Vector3{0, 0, 0}
    const One:  Vector3{1, 1, 1}
}