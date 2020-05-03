# TernaryTree

The ternarytree.hpp file define a C++ template class to create and use ternary search trees.

## How to use it

#### 1. Download the repository

Download the repository with your browser or with git using :

```
git clone https://github.com/QuentinP-dev/ternarytree.git
```

#### 2. Link the header file to your project

Since this project only contain one header file, you have nothing more to do than include the [ternarytree.hpp](/include/ternarytree.hpp)
file to your project like this :
```c++
#include "path/to/the/file/ternarytree.hpp"
```

#### 3. Introduction

##### a. Create a TernaryTree

The TernaryTree is defined like this TernaryTree<OP,STORED>.

OP is the type with which the tree will perform sorting operations. It can be any type as long as the operators "<" ">" and "==" are defined for that type.

STORED is the type that will stored in the tree's nodes. It can be anything.

```c++
// ternarytree declaration example
TernaryTree<int,bool> ternarytree1;

// another
TernaryTree<char,std::vector<double> > ternarytree2;

// here Myclass::operator==(const MyClass& a,const MyClass& b);
// Myclass::operator<(const MyClass& a,const MyClass& b);
// Myclass::operator>(const MyClass& a,const MyClass& b);
// must be defined.
TernaryTree<MyClass1,MyClass2> ternarytree3;
```
##### b. Add and find element

##### c. Save and load a TernaryTree to a file

## Build the documentation

If you have doxygen and cmake installed, you can build the documentation by running the [CMakeLists.txt](/CMakeLists.txt)

*(example for a Linux system)*
```
cmake CMakeLists.txt -B build/
cd build/
make
```

## Contact

If you find any bugs or issues using the code, feel free to send me an email at :
quentinp.dev@gmail.com

## Licence

GNU GENERAL PUBLIC LICENSE

Version 3, 29 June 2007

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See theGNU General Public License for more details.
