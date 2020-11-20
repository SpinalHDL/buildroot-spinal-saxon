# SpinalHDL SaxonSoC Buildroot Integration
> This package allows the user to build Linux environments for the Spinal SaxonSoC

## Usage
Create a directory

```
mkdir <project dir>; cd <project dir> 
```

Download the _latest_ buildroot version

```
git clone https://github.com/buildroot/buildroot.git
```

Download the _latest_ Spinal SaxonSoC Package

```
git clone https://github.com/svancau/buildroot-spinal-saxon.git
```

Create a build directory

```
mkdir <target build dir>; cd <target build dir> 
```

Select the board support
> Board defconfig can be found in the _configs_ directory of this package

```
make -O=$PWD BR2_EXTERNAL=<SaxonSoc Package> -C <path to buildroot> <Board_defconfig>
```

Customize to your needs

```
make menuconfig
```

Build

```
make 
```

> Enjoy, the build results are in the build directory/images

