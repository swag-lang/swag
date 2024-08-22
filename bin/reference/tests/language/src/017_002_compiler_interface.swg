/**
The other method to compile generated code is to use the function `compileString()` in the `@compiler()` interface. Of course this should be called at compile time, and mostly during a `#message` call.

Here is a real life example from the `Std.Ogl` module (opengl wrapper), which uses `#message` to track functions marked with a specific **user attribute** `Ogl.Extension`, and generates some code for each function that has been found.
*/

/**
First we declare a new specific attribute, which can then be associated with a function.
```swag
#[AttrUsage(AttributeUsage.Function)]
attr Extension()

// Here is an example of usage of that attribute.
#[Extension]
{
    func glUniformMatrix2x3fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix2x4fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix3x2fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix3x4fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix4x2fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix4x3fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
}
```
The following will be used to track the functions with that specific attribute.
```swag
struct OneFunc
{
    type: typeinfo
    name: string
}

#[Compiler]
var g_Functions: Array'OneFunc
```
This `#message` will be called for each function of the `Ogl` module.
```swag
#message(CompilerMsgMask.SemFunctions)
{
    let itf = @compiler()
    var msg = itf.getMessage()

    // If the function does not have our attribute, forget it
    if !Reflection.hasAttribute(msg.type, Extension)
        return

    // We just track all the functions with the given attribute
    g_Functions.add({msg.type, msg.name})
}
```
We will generate a `glInitExtensions` global function, so we register it as a placeholder.
```swag
#placeholder glInitExtensions
```
This is called once all functions of the module have been typed, and this is the main code generation.
```swag
#message(CompilerMsgMask.PassAfterSemantic)
{
    var builderVars: StringBuilder
    var builderInit: StringBuilder

    // Generate the `glInitExtensions` function
    builderInit.appendString("public func glInitExtensions()\n{\n");

    // Visit all functions we have registered, i.e. all functions with the `Ogl.Extension` attribute.
    visit e in g_Functions
    {
        let typeFunc = cast(const *TypeInfoFunc) e.type

        // Declare a lambda variable for that extension
        builderVars.appendFormat("var ext_%: %\n", e.name, typeFunc.name)

        // Make a wrapper function
        builderVars.appendFormat("public func %(", e.name)
        visit p, i in typeFunc.parameters
        {
            if i != 0 builderVars.appendString(", ")
            builderVars.appendFormat("p%: %", i, p.pointedType.name)
        }

        if typeFunc.returnType == void
            builderVars.appendFormat(")\n{\n")
        else
            builderVars.appendFormat(")->%\n{\n", typeFunc.returnType.name)
        builderVars.appendFormat("\treturn ext_%(", e.name)
        visit p, i in typeFunc.parameters
        {
            if i != 0 builderVars.appendString(", ")
            builderVars.appendFormat("p%", i)
        }

        builderVars.appendString(");\n}\n\n")

        // Initialize the variable with the getExtensionAddress
        builderInit.appendFormat("\text_% = cast(%) getExtensionAddress(@dataof(\"%\"))\n", e.name, typeFunc.name, e.name);
    }

    // Compile !!
    let itf = @compiler()
    var str = builderVars.toString()
    itf.compileString(str.toString())

    builderInit.appendString("}\n");
    str = builderInit.toString()
    itf.compileString(str.toString())
}
```
*/
