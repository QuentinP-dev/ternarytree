# TernaryTree

The ternarytree.hpp file defines a C++ template class to create and use ternary search trees.

## How to use it

#### 1. Download the repository

Download the repository with your browser or with git using :

```
git clone https://github.com/QuentinP-dev/ternarytree.git
```

#### 2. Link the header file to your project

Since this project only contains one header file, you don't have anything else to do than include the [ternarytree.hpp](/include/ternarytree.hpp)
file to your project like this :
```c++
#include "path/to/the/file/ternarytree.hpp"
```

## Brief overview 

#### 1. Create a TernaryTree

The TernaryTree class is defined as :
```c++
template <class OP,class STORED> class TernaryTree
```

*OP* is the type with which the tree will perform sorting operations. It can be any type as long as the operators '<' '==' and '>' are defined for that type.

*STORED* is the type that will be stored in the tree's nodes. It can be anything.

For example :

```c++
// ternarytree declaration example
TernaryTree<int,bool> ternarytree1;

// another example
TernaryTree<char,std::vector<double> > ternarytree2;

// here Myclass::operator==(const MyClass& a,const MyClass& b);
// Myclass::operator<(const MyClass& a,const MyClass& b);
// Myclass::operator>(const MyClass& a,const MyClass& b);
// must be defined.
TernaryTree<MyClass1,MyClass2> ternarytree3;
```
#### 2. Cursor's operations

The TernaryTree class works with a cursor that points to a node of the tree.

**get** and **set** operations will respectivly return and modify the variables of the node pointed by this cursor.

There are several operations like **goToBigger** (which set the cursor to the bigger node) or **resetCursor** (which set the cursor to the root of the tree) to handle this cursor.

Fonctions **find**, **add**, **saveToFile** and **remove** have a pre-set parameter which makes them perform a **resetCursor** before starting. If this parameter if set to false, the functions will only work on the subtree that has the node pointed by the cursor as root.
If you do want to run those functions on the subtree, then add *false* at the end of the function call.

#### 3. Add and find an element

A ternary tree may be seen as a multi layer binary tree. As the operations are made on an *OP* type, a collection of *OP* is needed to run an **add**, **find** or a **remove** function.

The TernaryTree class uses iterators to get each element of the given collection so you can pass any container you want to those functions, as long as the container does support iterator's operations.

For example, let's say we want to associate phones numbers to their owners. We will do the sorting operation on each digit of the phone number to have the fastest access to the owner. The owner's name will be saved in a *std::string*.

```c++
TernaryTree<int,std::string> phone_to_name;
```
Now if we want to add a phone number - owner association, we need to create a collection of *int* to pass the phone number to the fonction. Here we will use the *std::vector* container

```c++
std::vector<int> phone_number{0,8,8,4,6,9,2,6,7,3};
```

Now we can add the association to our tree :

```c++
//As we just created our tree, the cursor is already on the root
//we add the phone_number to the tree's structure 
phone_to_name.add(phone_number); // the cursor now points to the matching node
//we set the STORED variable of the node pointed by the cursor
phone_to_name.set("John");
```
And now another one :

```c++
phone_number={0,4,7,3,4,8,6,1,4,0};
//we want to add this from the root of our tree 
//and not to append it at the subtree already created by John's number
phone_to_name.resetCursor();
phone_to_name.add(phone_number);
phone_to_name.set("Mike");
```

Note that it's also possible to pass the *STORED* variable to the **add** function, and since the **add** function already calls the **resetCursor** function when its *reset_cursor* parameter is true, we actually need to write only :

```c++
phone_number={0,4,7,3,4,8,6,1,4,0};
phone_to_name.add(phone_number,"Mike");
```

Now if we want to remove John's number, we can do as :

```c++
phone_number_of_john={0,4,7,3,4,8,6,1,4,0};
//move the cursor to the node that match the best the John's number
if(phone_to_name.find(phone_number_of_john)) // true if the number is on the tree structure
{
	//remove the node pointed by the cursor
	phone_to_name.remove();
}
```
We can also do this in one call :
```c++
phone_number_of_john={0,4,7,3,4,8,6,1,4,0};
//here remove will first find the John's number and then remove it from the tree
if(phone_to_name.remove(phone_number_of_john))
{
	//the number has been removed
}
```

#### 4. Save and load a TernaryTree to a file

The TernaryTree class allows you to save and load your trees in files. Since the tree has to save template types (which can be custom objects), the **saveToFile** and **loadFromFile** functions take as parameters functions to convert *OP* and *STORED* type into *std::string* and vice versa.

Note that, due to parsing operations, the *std::string* you will get to build your *OP* and *STORED* variables will only contain spaces as delimiter character, no matter how you have saved your *OP* and *STORED* type in the file.

As an example, if you have :

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

But the *std::string* given to your *STORED* **stringToStored**(const *std::string*& str) function will be :
```
a : 8 b : p
```

Note that *std::stringstream* can be really useful to have those functions done.

#### 5. Light ternary trees

The functions **optimize** and **remove** need additional variables to work. It means that each node of the tree will use 25 bytes to store those variables.
If you do not need the **optimize** and the **remove** functions, you can save your memory by defining the keyword TERNARYTREE_LIGHT.
```c++
//must be before the inclution of ternarytree.hpp
#define TERNARYTREE_LIGHT
#include "path/to/the/file/ternarytree.hpp"
```

Note that the **loadFromFile** and **saveToFile** functions can be used to cast a light ternary tree into a normal one and vice versa.


## Build the documentation

If you have doxygen and cmake installed, you can build the documentation by running the [CMakeLists.txt](/CMakeLists.txt)

*(example for a Linux system)*
```
cmake CMakeLists.txt -B build/
cd build/
make
```

## Contact

If you find any bugs or issues using this code, please feel free to send me an email at :
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
