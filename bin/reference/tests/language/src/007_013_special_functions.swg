#global skip

/**
# Main Function (`#main`)

The `#main` function is the primary entry point for the program, similar to the `main()`
function in languages like C or C++. This function is unique within each module, meaning
you can only define it **once** per module.

In the context of an executable program, `#main` is where the program's execution begins.
Any code placed within this function will be the first to execute when the program runs.
*/
#main
{
}

/**
# Handling Program Arguments

Unlike the `main()` function in C, the `#main` function in this language does not
take arguments directly. Instead, command-line arguments can be retrieved using the
intrinsic `@args`, which provides a slice containing all the arguments passed to the program.

Here’s an example demonstrating how to work with command-line arguments:
*/
#main
{
    // Retrieve the program arguments using the @args intrinsic
    var myArgs = @args

    // Determine the number of arguments passed
    var count = @countof(myArgs)

    // Handle a specific argument, for example, enabling fullscreen mode
    if myArgs[0] == "fullscreen"
    {
        // Logic to initiate fullscreen mode would go here
        ...
    }
}

/**
# Pre-Main Function (`#premain`)

The `#premain` function is executed after all `#init` functions across all modules have
completed, but before the `#main` function begins.

This function is typically used for tasks that need to be performed after module
initialization, yet before the main program logic is executed. It's useful for setup
tasks that depend on the initial state of the program being fully established.
*/
#premain
{
}

/**
# Initialization Function (`#init`)

The `#init` function is executed at runtime during the module initialization phase.
You can define multiple `#init` functions within the same module, allowing different
parts of the module to initialize independently.

The execution order of `#init` functions within the same module is undefined,
so you should not rely on a specific sequence of initialization tasks. However,
all `#init` functions will execute before any code in the `#main` or `#premain` functions.
*/
#init
{
}

/**
# Drop Function (`#drop`)

The `#drop` function acts as a cleanup function and is called when a module is unloaded
at runtime. It serves as the counterpart to `#init`, ensuring that any resources allocated
during initialization are properly released.

Just like `#init`, you can define multiple `#drop` functions within a module, and
the order of their execution is undefined. However, `#drop` functions are guaranteed
to run in the reverse order of their corresponding `#init` functions, ensuring a
logical cleanup process.
*/
#drop
{
}

/**
# Test Function (`#test`)

The `#test` function is a specialized function designed for testing purposes.
It is typically used within the `tests/` folder of your workspace and is executed
only when the program is run in test mode.

This function is crucial for validating the correctness and functionality of your
code in a controlled environment before it is deployed or released. It allows you
to define test cases and assertions to ensure that your code behaves as expected.
*/
#test
{
}
