# Sudoku

## OpenCV headers and libraries

**Project–>Properties->C/C++ Build->Settings->Tool Settings->GCC C++ Compiler->Includes**
	Include Paths (-l): /usr/local/include/opencv

**Project–>Properties->C/C++ Build->Settings->Tool Settings->GCC C++ Linker->Libraries**
	Library search path (-l): /usr/local/lib
	Libraries (-l): opencv_core opencv_imgcodecs opencv_imgcodecs opencv_highgui

## Extra libraries

 In order to use OCR algorithm you will need install this follow libraries:
```sh
$ sudo dnf -y install libstdc++ autoconf automake libtool autoconf-archive pkg-config gcc gcc-c++ make libjpeg-devel libpng-devel libtiff-devel zlib-devel
```


 * leptonica-devel

```sh
$ sudo dnf install leptonica-tools.x86_64 leptonica-devel.x86_64 
```

* Tesseract, for install this library follow this instructions:
https://github.com/tesseract-ocr/tesseract/blob/master/INSTALL.GIT.md
