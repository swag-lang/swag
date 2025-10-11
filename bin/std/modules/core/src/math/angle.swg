#global public
#global namespace Math
using Swag

// An angle representation struct that stores angles in radians.
// Provides convenient conversion between radians and degrees with literal suffix support.
struct Angle
{
    rad: f32 // The angle value stored in radians
}

impl Angle
{
    // Assign a radian value directly to the angle.
    // Allows implicit conversion from floating-point values representing radians.
    #[ConstExpr, Implicit, Inline]
    mtd opAffect(valueRad: f32)
    {
        .rad = valueRad
    }

    // Assign angle values with unit suffixes (rad or deg).
    // Supports literal syntax like '90'deg' or '1.57'rad' for convenient angle creation.
    #[ConstExpr, Implicit, Inline]
    mtd(suffix: string) opAffectLiteral(value: f32)
    {
        #if suffix == "rad" do
            .rad = value
        #elif suffix == "deg" do
            .rad = toRadians(value)
        #else do
            #error("invalid angle literal suffix [[" ++ suffix ++ "]]")
    }

    // Convert the angle from radians to degrees.
    // Returns the angle value in degrees as a floating-point number.
    #[Inline]
    mtd const toDegrees()->f32 => toDegrees(.rad)
}
