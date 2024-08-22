#global skip

/**
# Main Function (#main)

The `#main` function serves as the entry point of the program. It is analogous to the `main()` function in languages like C or C++. However, unlike C, this function can only be defined **once** per module. 

This function is only meaningful in the context of an executable program. It is where the program's execution begins.
*/
#main
{
}

/**
# Handling Program Arguments

Unlike the C function `main()`, the `#main` function in this language does not take arguments directly. However, you can retrieve command-line arguments using the intrinsic `@args`, which returns a slice containing all the program arguments.

Here's an example of how you might use it:
*/
#main
{
    // Retrieve the program arguments
    var myArgs = @args()
    
    // Get the count of arguments
    var count = @countof(myArgs)
    
    // Example of handling a specific argument
    if myArgs[0] == "fullscreen"
    {
        // Logic for fullscreen mode
        ...
    }
}

/**
# Pre-Main Function (#premain)

The `#premain` function is invoked after all the modules have executed their `#init` functions, but before the `#main` function is called. 

This is typically used to perform any setup that must occur after initialization but before the main program logic begins.
*/
#premain
{
}

/**
# Initialization Function (#init)

The `#init` function is called at runtime during the module initialization phase. You can define multiple `#init` functions within the same module, allowing you to perform different initialization tasks.

However, the order in which these functions execute within the same module is undefined. They will run as the module is loaded.
*/
#init
{
}

/**
# Drop Function (#drop)

The `#drop` function is the counterpart to `#init`. It is called when a module is unloaded at runtime. 

You can define multiple `#drop` functions, just like `#init`, to handle different cleanup tasks. The execution order within the same module is also undefined, but `#drop` functions are guaranteed to run in the reverse order of the corresponding `#init` functions.
*/
#drop
{
}

/**
# Test Function (#test)

The `#test` function is a special function used within the `tests/` folder of the workspace. All `#test` functions are executed only when the program is run in test mode.

This function is essential for validating the functionality of your code in a controlled environment before deployment.
*/
#test
{
}
