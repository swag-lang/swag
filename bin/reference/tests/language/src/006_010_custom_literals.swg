/**
# User-Defined Literals

User-defined literals allow you to extend the meaning of literals in the language, enabling the creation of custom types that can be initialized directly using literal values with specific suffixes. This feature is particularly useful for defining types like `Duration`, where different units of time (e.g., seconds, milliseconds, minutes) can be intuitively represented and manipulated.

# Literal Suffixes
A literal suffix is a string of characters that immediately follows a numeric literal to specify a particular unit or type. For example, in `4'ms`, `'ms'` is the suffix indicating that the value is in milliseconds.

To define user-defined literals, you typically provide:

1. A structure representing the custom type (e.g., `Duration`).
2. Methods that handle the conversion of the literal values based on the suffix provided.

# Defining a Custom Type: `Duration`
The `Duration` type is a struct designed to represent time intervals, internally stored in seconds. The type allows initialization through various time units like seconds, milliseconds, minutes, and hours.
*/

// Represents a delay, expressed in seconds.
struct Duration
{
    timeInSeconds: f32 // The duration in seconds
}

impl Duration
{
    // Method to initialize Duration using milliseconds directly (without suffix)
    #[Swag.ConstExpr, Swag.Implicit, Swag.Inline]
    mtd opAffect(valueMs: s32)
    {
        me.timeInSeconds = valueMs / 1000.0
    }
}

/**
To convert a given value and suffix to another value, you will use the 'opAffectLiteral' special function.
*/

impl Duration
{
    // Method to handle user-defined literals with a suffix (e.g., 5's, 500'ms)
    #[Swag.ConstExpr, Swag.Implicit, Swag.Inline]
    mtd(suffix: string) opAffectLiteral(value: s32)
    {
        // This method is triggered when a literal with a suffix is used.
        // The `suffix` parameter indicates the unit of the literal.
        // The `value` parameter is the numeric part of the literal.

        // Check if the suffix is 's' (seconds)
        #if suffix == "s" do
            // Directly assign the value as seconds
            .timeInSeconds = value
        #elif suffix == "ms" do
            // Convert milliseconds to seconds and assign
            .timeInSeconds = value / 1000.0
        #elif suffix == "min" do
            // Convert minutes to seconds (1 min = 60 seconds)
            .timeInSeconds = value * 60.0
        #elif suffix == "h" do
            // Convert hours to seconds (1 hour = 3600 seconds)
            .timeInSeconds = value * 3600.0
        #else do
            // Raise a compile-time error if an invalid suffix is encountered
            #error("invalid duration literal suffix '" ++ suffix ++ "'")
    }
}

/**
You can then use the suffix right after the literal value, as long as the type 'Duration' is specified.
*/

func toto(x: Duration) {}

#test
{
    let delay1: Duration = 5's        // Represents 5 seconds
    let delay2: Duration = 500'ms     // Represents 500 milliseconds
    let delay3: Duration = 2'min      // Represents 2 minutes
    let delay4: Duration = 1'h        // Represents 1 hour

    // Use the `Duration` type in functions
    toto(5'ms)
    toto(100'h)
}
