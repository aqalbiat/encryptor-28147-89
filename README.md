# encryptor-28147-89
This is the repositery where I publish my software implementation of encryptor-28147-89.
It is the standard of encryption from Soviet Union that is used to encrypt some data.
The PDF variant of the documentation is located in **document** folder of this repo.

## Environment 
This software was implemented in with these tools. All the commands in this README is executed in Microsoft PowerShell

````
PS C:\Your\Directory\encryptor> python --version
Python 3.13.0
PS C:\Your\Directory\encryptor> g++ --version
g++.exe (Rev3, Built by MSYS2 project) 13.2.0
Copyright (C) 2023 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
````

## Build
There is the special script written called **b.bat** that builds the encryptor

````
PS C:\Your\Directory\encryptor> ./b.bat
````

## Usage
The software is implemented as a CLI tool. After running **b.bat** there will be **app.exe** file in the **objects** folder.
The command to type in order to utilize it:
````
PS C:\Your\Directory\encryptor> ./objects/app.exe -i [IN_FILE] -o [OUT_FILE] -r [REGIME] -conf [CONF_FILE] (-dec|-enc) 
````
* IN_FILE    -> input file/directory consisting data in hexodecimal numbers with 64 bit long number in each line
* OUT_FILE   -> output file/directory to which the software must print out the resultant data
* REGIME     -> regime of encryption/decryption, currently, only two available: simple, gamma
* CONF_FILE  -> a file/directory consisting data in hexodecimal numbers with 64 bit long number in each line. 
                Configuration data must follow the format:
                .X0.X1  ->  1st line,   concatination of two 32 bit numbers
                .X2.X3  ->  2nd line,   concatination of two 32 bit numebrs
                .X4.X5  ->  3rd line,   concatination of two 32 bit numebrs
                .X6.X7  ->  4th line,   concatination of two 32 bit numbers
                .K1     ->  5th line,   one whole 64 bit number
                .K2     ->  6th line,   one whole 64 bit number
                .K3     ->  7th line,   one whole 64 bit number
                .K4     ->  8th line,   one whole 64 bit number
                .K5     ->  9th line,   one whole 64 bit number
                .K6     ->  10th line,  one whole 64 bit number
                .K7     ->  11th line,  one whole 64 bit number
                .K8     ->  12th line,  one whole 64 bit number
                .S      ->  13th line,  one whole 64 bit number
                .C1.C2  ->  14th line,  concatination of two 32 bit numbers
* (-dec|-enc) -> choose one of the processes to apply, -dec for decryption, -enc for encryption

## Testing
In order to execute all the tests, type command
````
PS C:\Your\Directory\encryptor> python test_all.py
````
This command runs 10 tests, where first five are for **simple** regime and next five are for **gamma** regime.

## Auxalary files
If you want to generate new configs for tests you may type:
In order to execute all the tests, type command
````
PS C:\Your\Directory\encryptor> python create_configs.py > .\storage\configs.txt
````
If one needs to generate some new input data, then there is one option
In order to execute all the tests, type command
````
PS C:\Your\Directory\encryptor> python generate.py N > .\your\directory\your_file.txt
````
## About upload.bat file
It is just helper for git push. Does not play a crucial role in using/testing of this CLI-tool. 