# TernaryTree

The ternarytree.hpp file define a C++ template class to create and use ternary search trees.

## How to use it

#### 1. Download the repository

Download the repository with your browser or with git using :

```
git clone https://github.com/QuentinP-dev/ternarytree.git
```

#### 2. Link the header file to your project

Since this project only contains one header file, you have nothing more to do than include the [ternarytree.hpp](/include/ternarytree.hpp)
file to your project like this :
```c++
#include "path/to/the/file/ternarytree.hpp"
```

#### 3. Introduction

##### a. Create a TernaryTree

The TernaryTree class is defined like this :
```c++
template <class OP,class STORED> class TernaryTree
```

OP is the type with which the tree will perform sorting operations. It can be any type as long as the operators '<' '==' and '>' are defined for that type.

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
##### b. Cursor's operations

The TernaryTree class works with a cursor that point to a node of the tree.

Get and set operations with respectivly return and modify the variables of the node pointed by this cursor.

There is several operations like goToBigger (which set the cursor to the bigger node) or resetCursor (which set the cursor to the root of the tree) to manipulate this cursor.

Fonctions find, add and saveToFile have a pre-set parameter which make then perform a resetCursor before starting. If this parameter if set to false, the fonction saveToFile for example, will only save the subtree that have the node pointed by the cursor for root.

#### c. Add and find an element

A ternary tree can be seen as a multi layer binary tree. So if the operation are made on an OP type, we will need a collection of OP to run an add or find function.

The TernaryTree class uses iterators to get each element of the given collection so you can pass any container you want as long as it does support iterator operation.

For example, let s say we want to associate phones numbers to there owner. We will do the sorting operation on each digit of the phone number to have a fastest acces time to the owner. The owner's name will be saved in a std::string.

```c++
TernaryTree<int,std::string> phone_to_name;
```
Now if we want to add a phone number - owner association we need to creation a collection of int to pass the phone number to the fonction. Here we will use the std::vector container

```c++
std::vector<int> phone_number{0,8,8,4,6,9,2,6,7,3};
```

Now we can add it to our tree :

```c++
//we just created our tree so the cursor is already on the root
//we add the phone_number to the tree's structure 
phone_to_name.add(phone_number); // the cursor is now set to the matching node
//we set the STORED variable of the node pointed by the cursor
phone_to_name.set("John");
```
And another one :

```c++
phone_number={0,4,7,3,4,8,6,1,4,0};
//we want to add this from the root of our tree and not happending it to the number of John
phone_to_name.resetCursor();
phone_to_name.add(phone_number);
phone_to_name.set("Mike");
```
Note that you can also pass the STORED variable to the add fonction and since the add function already call the resetCursor function when its reset_cursor parameter is true, we actually only need the write :

```c++
phone_number={0,4,7,3,4,8,6,1,4,0};
phone_to_name.add(phone_number,"Mike");
```
The find fonction works pretty in the same way, it will test if the giving collection of OP is in the tree structure or not and place the cursor to the BEST matching node (the cursor wont be on the matching node if the collection of OP is not in the tree's structure) 

##### d. Save and load a TernaryTree to a file

The TernaryTree class allow you to save and load your trees in files. Since the tree have to save template types (and it can be custom objects), the saveToFile and loadFromFile fonctions take as a parameters functions to convert OP and STORED type into std::string and vice versa.

Note that, due to parsing operation and no matter how you save your OP and STORED type, the std::string you will get to build your OP and STORED variables will only contains spaces as delimiter character.

A little example to be more clear, if you have :

```c++
struct Stored
{
	int a;
	char b;
};

std::string storedToString(const Stored& s)
{
	std::string r_str;
	r_str+="a : ";
	r_str+=s.a;
	r_str+="\nb : ";
	r_str+=s.b;
	return r_str;
}
```
You will have in the file something like :
```
a : 8
b : p
```

But the std::string given to your STORED stringToStored(const std::string& str) function will be :
```
a : 8 b : p
```

Note that std::stringstream can be really usefull to make thoose fonctions done.


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
