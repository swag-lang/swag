#global skip

// A bunch of #global can start a source file

// Skip the content of the file, like this one (but must be a valid swag file)
#global skip

// All symbols in the file will be public/protected/private
#global public
#global protected
#global private

// All symbols in the file will go the namespace "toto"
#global namespace "toto"

// A #if for the whole file
#global if DEBUG == true

// Some attributes can be assigned to the full file
#global #[swag.safety("", true)]

// The file will be exported for external usage
// It's like putting everything in public, except that the file will
// be copied in its totality in the public folder
#global export

// Link with a given external library
#global foreignlib "windows.lib"
