const char* g_Runtime = R"(

namespace swag {

func @print: (value: s64);
func @print: (value: f64);
func @print: (value: char);
func @print: (value: string);
func @assert: (value: bool);

}
)";
