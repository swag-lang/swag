..\bin\swag build -w:../bin/examples --backend:x64 --rebuild %1 %2 %3 %4

..\bin\swag_devmode clean --script
..\bin\swag run -w:..\bin\examples -m:wnd    --bytecode --args:"swag.test"
..\bin\swag run -w:..\bin\examples -m:captme --bytecode --args:"swag.test"
..\bin\swag run -w:..\bin\examples -m:draw   --bytecode --args:"swag.test"
..\bin\swag run -w:..\bin\examples -m:img    --bytecode --args:"swag.test"
..\bin\swag run -w:..\bin\examples -m:wnd    --args:"swag.test"
..\bin\swag run -w:..\bin\examples -m:captme --args:"swag.test"
..\bin\swag run -w:..\bin\examples -m:draw   --args:"swag.test"
..\bin\swag run -w:..\bin\examples -m:img    --args:"swag.test"