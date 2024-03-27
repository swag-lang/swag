namespace std
{
    namespace math
    {
        typealias Float = f32
        func isZeroEpsilon(x, y: Float) => true
    }
}

namespace std
{
    namespace math
    {
        struct Vector2
        {
            x, y: Float
        }

        impl Vector2
        {
            func isZeroEpsilon(using self, eps: Float = cast(Float) 1)->bool
            {
                return isZeroEpsilon(x, eps)
            }
        }
    }
}
