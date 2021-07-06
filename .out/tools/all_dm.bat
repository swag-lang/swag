call x64_dm.bat --cfg:debug --tag:Pixel.FullTest
call x64_dm.bat --cfg:fast-debug --tag:Pixel.FullTest
call x64_dm.bat --cfg:release --tag:Pixel.FullTest
call llvm_dm.bat --cfg:debug --cfg-optim-bc:true
