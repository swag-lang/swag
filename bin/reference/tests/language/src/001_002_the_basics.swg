/**
Swag is a modern and flexible programming language designed for ease of use, combining a strong type system with an intuitive syntax. In this section, we’ll walk through the fundamental concepts of Swag, using examples to illustrate how to work with variables, functions, control flow, and more.

# Constants and Variables

In Swag, you use `const` to define constants, `var` to define mutable variables, and `let` to define immutable variables (single-assignment variables that behave like constants after initialization). Constants are immutable and must be initialized when declared, while variables declared with `var` can be modified after their initial assignment.
*/
#test
{
    const Pi = 3.14159    // A constant value, cannot be changed
    var radius = 10       // A mutable variable, can be modified
    let height = 100      // An immutable variable, assigned once
}

/**
# Mutability and Reassignment

Variables declared with `var` can be reassigned, allowing you to update their values as needed:
*/

#test
{
    var counter = 0
    counter = 5         // Reassigning the value of 'counter'
}

/**
Constants declared with `const` and immutable variables declared with `let` cannot be reassigned:
*/

#test
{
    const MaxCount = 10
    // MaxCount = 12  // Error: Cannot assign to constant

    let width = 50
    // width = 60     // Error: Cannot reassign an immutable variable
}

/**
# Type Inference

Swag allows you to omit the type declaration if the compiler can infer it from the assigned value:
*/

#test
{
    let greeting = "Hello, Swag!"  // Type inferred as 'string'
    var age = 25                   // Type inferred as 's32' (signed 32-bit integer)
}

/**
If needed, you can also explicitly declare the type:
*/

#test
{
    let name: string = "Alice"
    var score: s32 = 100
}

/**
# Constants and Immutability

Swag’s `let` and `const` keywords emphasize immutability. The `let` keyword is particularly useful when you want to define a value that should only be assigned once but may need to be initialized at runtime, as opposed to `const`, which is usually evaluated at compile-time.

# Printing Values

Swag provides the intrinsic `@print` function for displaying output in the console. Here's an example:
*/

const CanPrint = false  // Must be set to 'true' to show the result of '@print'

func print(x: ...)
{
    if CanPrint:
        @print(x)
}

#test
{
    let name = "Swag"
    print("Welcome to ", name, " language!\n")
}

/**
This will print: `Welcome to Swag language!`

# Comments

Swag supports both single-line and multi-line comments to help you annotate and document your code:
*/

#test
{
    // This is a single-line comment

    /*
    This is a multi-line comment
    that spans multiple lines.
    */
}

/**
# Semicolons

Semicolons in Swag are optional at the end of a statement. They are primarily used when you want to write multiple statements on the same line:
*/

#test
{
    let a = 5; let b = 10; print(a + b, "\n")  // Outputs 15
}

/**
# Integers and Floating-Point Numbers

Swag provides several types for representing numbers, including signed and unsigned integers, as well as floating-point numbers:
*/

#test
{
    let maxHeight: u32 = 500    // Unsigned 32-bit integer
    let distance: f64 = 42.195  // Double-precision floating-point number

    // You can perform arithmetic operations just as expected:
    let result = maxHeight + 50
    print("New height: ", result, "\n")
}

/**
# Strings

Strings in Swag are UTF-8 encoded, which allows them to handle international text easily:
*/

#test
{
    let message = "Hello, 世界!"  // A string with Unicode characters
    print(message, "\n")
}

/**
# Tuples

Tuples allow you to group multiple values into a single compound value. The values in a tuple can be of different types, making tuples a flexible tool for returning multiple results from a function or bundling data.
*/

#test
{
    let person = {"Alice", 30, true}
    let (name, age, isActive) = person

    print(name, " is ", age, " years old. Active: ", isActive, "\n")

    // You can also access tuple elements by their index:
    print(person.item0, " is ", person.item1, " years old.\n")
}

/**
# Control Flow

Swag offers flexible control structures, such as 'if', 'for', and 'switch', to manage how your program executes based on different conditions. One key feature of Swag is that curly braces '{}' are not mandatory for control blocks. Instead, you can use ':' to define a block, much like in Python.
*/

#test
{
    let temperature = 30
    if temperature > 25:
        print("It's hot!\n")
    else:
        print("It's cool!\n")
}

/**
In this example, ':' replaces curly braces to define the start of the 'if' and 'else' blocks.
You can still use curly braces for control blocks if you prefer:
*/

#test
{
    let temperature = 30
    if temperature > 25 {
        print("It's hot!\n")
    }
    else {
        print("It's cool!\n")
    }
}

/**
Swag also has flexible loop constructs like `for` and `while`:
*/

#test
{
    // Iterate 10 times
    for 10
    {
        print("Iteration: ", @index, "\n")
    }

    for var i: u32 = 0; i < 10; i += 2
    {
        print("Iteration: ", i, "\n")
    }

    var countdown: u32 = 5
    while countdown > 0
    {
        print("T-minus ", countdown, "\n")
        countdown -= 1
    }
}

/**
# `foreach` Loop

Swag provides the `foreach` loop for iterating over collections such as arrays. This loop automatically assigns each item to a variable as it iterates:
*/

#test
{
    let numbers = [1, 2, 3, 4, 5]
    foreach num in numbers {
        print("Number: ", num, "\n")
    }
}

/**
In this example, the `foreach` loop goes through each element of the array `numbers` and prints it.

# Functions

Functions in Swag encapsulate reusable blocks of code. Here’s a simple function that adds two numbers:
*/

#test
{
    func add(a: s32, b: s32) -> s32 {
        return a + b
    }

    let result = add(10, 5)
    print("Result: ", result, "\n")  // Outputs 15
}

/**
Functions in Swag can return multiple values using tuples:
*/

#test
{
    func splitName() -> {x: string, y: string}
    {
        return {"a", "b"}
    }

    let (firstName, lastName) = splitName()
    @assert(firstName == "a")
    @assert(lastName == "b")
}

/**
# Error Handling

Swag provides robust error handling mechanisms using `throw`, `try`, and `catch`, allowing you to handle errors in a flexible and controlled manner. Unlike traditional exception handling, Swag treats errors as specialized return values rather than exceptions.

# Propagating Errors with `throw` and `try`

In Swag, a function can signal that it might encounter an error by being annotated with the `throw` keyword. This indicates that the function might raise an error during execution, and any function calling it must use the `try` keyword to handle the potential error.
*/

func divide(a: s32, b: s32) -> s32 throw
{
    if b == 0
    {
        throw Swag.BaseError{"Division by zero!"}
    }

    return a / b
}

#test
{
    let result = try divide(10, 2)  // Propagates the error if one occurs
    print("Result: ", result, "\n") // Outputs 5
}


/**
If an error is raised inside the `divide` function, it will be propagated to the caller. The `try` keyword ensures that the caller handles or propagates the error further.

# Handling Errors with `catch`

Instead of propagating errors, you can catch and handle them within the function using the `catch` keyword. When an error occurs, Swag provides the error through the `@err` intrinsic, which can be checked and handled.
*/

#test
{
    let result = catch divide(10, 0)
    if @err != null {
        print("Error: ", @err, "\n")
    } else {
        print("Result: ", result, "\n")
    }
}

/**
In this case, the `catch` keyword is used to trap the error and prevent it from being propagated. If an error occurs, it is available through `@err`, and you can handle it gracefully.

# Dismissing Errors with `trycatch`

The `trycatch` construct allows you to dismiss an error and continue execution without propagating it. When using `trycatch`, Swag will assign a default value to the variable if an error occurs, allowing the program to continue without disruption.
*/

func safeDivide(a: s32, b: s32) -> s32 {
    return trycatch divide(a, b)
}

#test
{
    let result = safeDivide(10, 0)
    print("Result: ", result, "\n") // Outputs 0, the default value
}

/**
In this example, if an error occurs in `divide`, the program continues, and `result` is assigned the default value for its type (0 in this case). This approach is useful when you want to handle errors locally without propagating them.
*/