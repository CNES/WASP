# WASP - Weighted Average Synthesis Processor

Image processing chain based on OTB to create composite cloud-free images of Sentinel-2 or Venus Level-2A products distributed by the [Theia Land data centre](https://theia.cnes.fr/atdistrib/rocket/#/home).

## Inputs, algorithm and outputs
The processing chain creates a composite image of multiple Level-2A products produced with MAJA(LINK). A database of over 150.000 freely distributed products is available under:
* [Level-2A products](https://theia.cnes.fr/atdistrib/rocket/#/search?page=1&collection=SENTINEL2&processingLevel=LEVEL2A)

The algorithm to combine the inputs into one single synthesis is described [here](http://www.cesbio.ups-tlse.fr/multitemp/?p=13976).
Once synthesized, a Level-3 product is generated further described in the [format description](http://www.cesbio.ups-tlse.fr/multitemp/?page_id=14019).
A collection of Level-3 products can be found under:
* [Level-3A products](https://theia.cnes.fr/atdistrib/rocket/#/search?page=1&collection=SENTINEL2&processingLevel=LEVEL3A)

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

The program has the following dependencies:
* OTB 6.2 (https://orfeo-toolbox.org)
* Python 2.7 or >3.5 (https://www.python.org/downloads/)
* CMake >3.7.2 (https://cmake.org)
* GCC >6.3.0 (https://gcc.gnu.org) or any other C/C++ compiler for your system

### Pre-compiled binaries

WASP binaries are available for Linux distributions under the following link:

#### [**Download**](https://logiciels.cnes.fr/en/content/wasp)


To install it, execute:

```
./WASP-1.0.run --target /path/to/install
```

### Compiling from source

In order to install the software, first install CMake, the c/c++ compiler and the python interpreter (optional). Then follow the tutorial on how to install OTB via the Superbuild to the desired location:

*https://www.orfeo-toolbox.org/SoftwareGuide/SoftwareGuidech2.html#x16-220002.1.2*

Note: You do not need to activate/deactivate any modules. The default configuration works out of the box.
Then proceed and install WASP:
```
cd $WASP_DIR
mkdir build && install
cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/OTB
```

Eventually you have to specify the location to your compiler by setting ```CMAKE_C_COMPILER``` and ```CMAKE_CXX_COMPILER```.
Then, run
```
make && make install
```

## Run the software

In order to run the processing chain, the script WASP located in the bin/ folder after installation has to be called.
For an explanation on which inputs the program takes, simply execute

```
./bin/WASP --help
```

### Example

First, you need to download and unzip the Level-2 products of your choice. Have a look at [theia_download](https://github.com/olivierhagolle/theia_download) if you don't know how to do this.
The following command line creates a composite centered around the 2018/05/02 out of 9 Level-2A products from a 46-day period:

```
./bin/WASP \
--input ./SENTINEL2A_20180525-103024-462_L2A_T32ULV_D_V1-
7/SENTINEL2A_20180525-103024-462_L2A_T32ULV_D_V1-7_MTD_ALL.xml \
./SENTINEL2A_20180518-104024-461_L2A_T32ULV_D_V1-7/SENTINEL2A_20180518-104024-
461_L2A_T32ULV_D_V1-7_MTD_ALL.xml \
./SENTINEL2A_20180508-104025-460_L2A_T32ULV_D_V1-7/SENTINEL2A_20180508-104025-
460_L2A_T32ULV_D_V1-7_MTD_ALL.xml \
./SENTINEL2A_20180515-103024-459_L2A_T32ULV_D_V1-7/SENTINEL2A_20180515-103024-
459_L2A_T32ULV_D_V1-7_MTD_ALL.xml \
./SENTINEL2A_20180505-103125-791_L2A_T32ULV_D_V1-7/SENTINEL2A_20180505-103125-
791_L2A_T32ULV_D_V1-7_MTD_ALL.xml \
./SENTINEL2A_20180415-103544-548_L2A_T32ULV_D_V1-7/SENTINEL2A_20180415-103544-
548_L2A_T32ULV_D_V1-7_MTD_ALL.xml \
./SENTINEL2B_20180420-103302-810_L2A_T32ULV_D_V1-7/SENTINEL2B_20180420-103302-
810_L2A_T32ULV_D_V1-7_MTD_ALL.xml \
./SENTINEL2A_20180418-104512-083_L2A_T32ULV_D_V1-7/SENTINEL2A_20180418-104512-
083_L2A_T32ULV_D_V1-7_MTD_ALL.xml \
./SENTINEL2A_20180412-105510-678_L2A_T32ULV_D_V1-7/SENTINEL2A_20180412-105510-
678_L2A_T32ULV_D_V1-7_MTD_ALL.xml \
--out . \
--date 20180502 \
--synthalf 23 \
```

Once the processing finishes, the following line should be displayed:
```
================= WASP synthesis of 9 inputs finished in xxxx.xx s =================
```

The resulting product can be then found under
```
./SENTINEL2X-20180502-000000-000_L3A_T32ULV_C_V1-1
```

Click [here](http://www.cesbio.ups-tlse.fr/multitemp/?page_id=14019) for the format description.

## Built With

* [OTB](https://orfeo-toolbox.org) - The Orfeo toolbox

## Copyright

For the initial work performed within the [Sen2Agri](http://www.esa-sen2agri.org/) project :
* **CS Romania**
* **Catholic University of Leuven (UCL)**
* **Centre National d'Etudes Spatiales (CNES)**

Special thanks to the developers of Sen2Agri:
Cosmin Udroiu, Laurentiu Nicola, Alex Grosu, Lucian Barbulescu and Anca Trasca

For the development of WASP :
* **Centre National d'Etudes Spatiales (CNES)** 

## License

This project is licensed under the GPLv3 License - see the [LICENSE.md](LICENSE.md) file for details