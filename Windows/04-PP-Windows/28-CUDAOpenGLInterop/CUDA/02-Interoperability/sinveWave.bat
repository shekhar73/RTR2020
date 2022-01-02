nvcc -c -o sineWave.cu.obj sineWave.cu
cl.exe /c /EHsc /I "C:\glew-2.1.0\include" /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.1\include OGLPP.cpp"
link.exe OGLPP.obj sineWave.cu.obj user32.lib gdi32.lib /LIBPATH:C:\glew-2.1.0\lib\Release\Win32\glew32.lib

