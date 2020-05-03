
//********************************************************************
// ternarytree.hpp define a template class of ternary trees
// Copyright (C) 2020 Quentin Putaud
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses
//********************************************************************


/*!
*\file ternarytree.hpp
*\author Quentin Putaud
*\version 1.0
*\date 01/05/2020
*/


#ifndef TERNARYTREE_HPP_INCLUDED
#define TERNARYTREE_HPP_INCLUDED

#include <stdexcept>
#include <fstream>
#include <vector>


/*!
*\class TernaryTree ternarytree.hpp "ternarytree.hpp"
*\brief A template class to create and use ternary trees.
*/
template <class OP,class STORED>
class TernaryTree
{
private:

    /*!
    *\class Node ternarytree.hpp "ternarytree.hpp"
    *\brief A private class of TernaryTree to handle each node
    */
    class Node
    {
    public:
        OP _op;

        STORED _stored;

        Node* _bigger=nullptr;
        unsigned int _nb_bigger=0;

        Node* _smaller=nullptr;
        unsigned int _nb_smaller=0;

        Node* _next=nullptr;
        unsigned int _nb_next=0;

        /*!
        *\brief Destrutor of the Node class
        */
        ~Node()
        {
            if(_next!=nullptr)
                delete _next;
            if(_bigger!=nullptr)
                delete _bigger;
            if(_smaller!=nullptr)
                delete _smaller;
        }
    };

    Node* _first=nullptr;

    Node* _cursor=nullptr;

    /*!
    *\brief Private terminal recursive function.
    * Works from the subtree pointed by the cursor.
    * Test if a collection of OP is in the subtree's structure.
    * Set the cursor to the best matching node.
    *\param[in] ops const OP_COL& : a collection of OP.
    *\param[in] it OP_COL::const_iterator& : a const_iterator for the collection of OP.
    *\param[in] add bool : a parameter to update optimization variables. Default value is false.
    *\return bool : true if the collection of OP is in the subtree's structure, false if not.
    */
    template<class OP_COL>
    bool find(const OP_COL& ops,typename OP_COL::const_iterator& it,bool add=false)
    {
        if(_cursor==nullptr)
            return false;

        if(_cursor->_op==*it)
        {
            if(it+1==ops.end())
                return true;

            if(_cursor->_next==nullptr)
                return false;

            if(add)
                _cursor->_nb_next++;
            _cursor=_cursor->_next;
            return find(ops,++it,add);
        }

        if(_cursor->_op<*it)
        {
            if(_cursor->_bigger==nullptr)
                return false;

            if(add)
                _cursor->_nb_bigger++;
            _cursor=_cursor->_bigger;
            return find(ops,it,add);
        }

        if(_cursor->_op>*it)
        {
            if(_cursor->_smaller==nullptr)
                return false;

            if(add)
                _cursor->_nb_smaller++;
            _cursor=_cursor->_smaller;
            return find(ops,it,add);
        }

        return false;
    }


    /*!
    *\brief Private terminal recursive function.
    * Return the smallest leaf of a given subtree.
    *\param[in] current Node* : the current node.
    *\param[in] nb unsigned int : a paramater to update optimization variables. Default value is 0.
    *\return Node* : the smallest leaf of the given tree.
    */
    Node*  getSmallest(Node* current,unsigned int nb=0)
    {
        current->_nb_smaller+=nb;
        if(current->_smaller!=nullptr)
            return getSmallest(current->_smaller);
        return current;
    }

    /*!
    *\brief Private terminal recursive function.
    * Return the biggest leaf of a given subtree.
    *\param[in] current Node* : the current node.
    *\param[in] nb unsigned int : a paramater to update optimization variables. Default value is 0.
    *\return Node* : the biggest leaf of the given tree.
    */
    Node*  getBiggest(Node* current,unsigned int nb=0)
    {
        current->_nb_bigger+=nb;
        if(current->_bigger!=nullptr)
            return getBiggest(current->_bigger);
        return current;
    }

    /*!
    *\brief Private recursive function.
    * Optimize the tree's structure to reduce the mean acces time.
    *\param[in] current Node* : the current node.
    *\param[in] father_link Node** : a pointer to the father's link of the current node.
    *\return void
    */
    void optimize(Node* current,Node** father_link)
    {
        if(current!=nullptr)
        {
            if(current->_nb_next+current->_nb_smaller+1<current->_nb_bigger) // too much in bigger
            {
                current->_nb_bigger=0;
                (*father_link)=current->_bigger;
                getSmallest(current->_bigger,current->_nb_next+current->_nb_smaller)->_smaller=current;
                current->_bigger=nullptr;
            }
            else if(current->_nb_next+current->_nb_bigger+1<current->_nb_smaller) // too much in smaller
            {
                current->_nb_smaller=0;
                (*father_link)=current->_smaller;
                getBiggest(current->_smaller,current->_nb_next+current->_nb_bigger)->_bigger=current;
                current->_smaller=nullptr;
            }
            optimize(current->_smaller,&current->_smaller);
            optimize(current->_bigger,&current->_bigger);
            optimize(current->_next,&current->_next);
        }
    }

    /*!
    *\brief Private recursiv function.
    * Save the subtree in a file.
    *\param[in] current Node* : the current node.
    *\param[in,out] n unsigned int& : the id of the current node.
    *\param[in] f unsigned int : the father's id of the current node.
    *\param[in] l unsigned int : the type of link between the father's current node and the current node.
    *\param[in,out] output_file std::ofstream& : the file where the tree will be saved.
    *\param[in] OP_TO_STRING std::string(OP) : a function to cast an OP variable into an std::string.
    *\param[in] STORED_TO_STRING std::string(STORED) : a function to cast a STORED variable into an std::string.
    *\return void
    */
    void saveToFile(Node* current,unsigned int& n,unsigned int f,unsigned int l,std::ofstream& output_file,std::string OP_TO_STRING(OP),std::string STORED_TO_STRING(STORED))
    {
        if(current!=nullptr)
        {
            int current_n=n;
            n++;
            output_file << "<#> node ";
            output_file <<  current_n <<"\n";
            output_file << "<#> from ";
            output_file <<  f <<"\n";
            output_file << "<#> link ";
            output_file <<  l <<"\n";
            output_file << "<#> op ";
            output_file <<  OP_TO_STRING(current->_op) <<"\n";
            output_file << "<#> data ";
            output_file <<  STORED_TO_STRING(current->_stored) <<"\n";
            output_file << "<#> end\n\n";
            saveToFile(current->_smaller,n,current_n,1,output_file,OP_TO_STRING,STORED_TO_STRING);
            saveToFile(current->_bigger,n,current_n,2,output_file,OP_TO_STRING,STORED_TO_STRING);
            saveToFile(current->_next,n,current_n,3,output_file,OP_TO_STRING,STORED_TO_STRING);
        }
    }

    /*!
    *\brief Private function.
    * Load a tree from a file.
    *\param[in,out] nodes std::vector<Node*> : a vector of Node* used to create the links between nodes.
    *\param[in,out] input_file std::ifstream& : the file where the tree is saved.
    *\param[in] STRING_TO_OP OP(std::string) : a function to cast an std::string  into an OP variable.
    *\param[in] STRING_TO_STORED STORED(std::string) : a function to cast an std::string  into a STORED variable.
    *\return void
    */
    void loadFromFile(std::vector<Node*>& nodes,std::ifstream& input_file,OP STRING_TO_OP(std::string),STORED STRING_TO_STORED(std::string))
    {
        std::string r_str;

        unsigned int node_id;
        unsigned int father_id;
        unsigned int link_type;
        std::string str_op;
        std::string str_stored;

        bool dont_read=false;

        while(input_file)
        {
            if(dont_read)
                dont_read=false;
            else
                input_file >> r_str;

            if(r_str=="<#>")
            {
                input_file >> r_str;

                if(r_str=="node")
                    input_file >> node_id;
                else if(r_str=="from")
                    input_file >> father_id;
                else if(r_str=="link")
                    input_file >> link_type;
                else if(r_str=="op")
                {
                    input_file >> r_str;
                    while(r_str!="<#>")
                    {
                        if(str_op=="")
                            str_op+=r_str;
                        else
                            str_op+=" "+r_str;
                        input_file >> r_str;
                    }
                    dont_read=true;
                }
                else if(r_str=="data")
                {
                    input_file >> r_str;
                    while(r_str!="<#>")
                    {
                        if(str_stored=="")
                            str_stored+=r_str;
                        else
                            str_stored+=" "+r_str;
                        input_file >> r_str;
                    }
                    dont_read=true;
                }
                else if(r_str=="end")
                {
                    switch(link_type)
                    {
                    case 0: //root
                        _first=new Node;
                        nodes.push_back(_first);
                        break;
                    case 1: //smaller
                        nodes[father_id]->_smaller=new Node;
                        nodes.push_back(nodes[father_id]->_smaller);
                        break;
                    case 2: //bigger
                        nodes[father_id]->_bigger=new Node;
                        nodes.push_back(nodes[father_id]->_bigger);
                        break;
                    case 3: //next
                        nodes[father_id]->_next=new Node;
                        nodes.push_back(nodes[father_id]->_next);
                        break;
                    default: //
                        //not supposed to go here
                        break;
                    }
                    nodes[nodes.size()-1]->_op=STRING_TO_OP(str_op);
                    nodes[nodes.size()-1]->_stored=STRING_TO_STORED(str_stored);
                    node_id=0;
                    father_id=0;
                    link_type=0;
                    str_op="";
                    str_stored="";
                }
            }
        }
    }


public:

    /*!
    *\brief Default constructor.
    */
    TernaryTree(void) {}

    /*!
    *\brief Default constructor.
    */
    TernaryTree(const TernaryTree<OP,STORED> &) {}

    /*!
    *\brief Destructor.
    */
    ~TernaryTree()
    {
        if(_first!=nullptr)
            delete _first;
    }

    /*!
    *\brief Save the tree in a file.
    *\param[in] path const std::string& : path to the file where the tree will be saved.
    *\param[in] OP_TO_STRING std::string(OP) : a function to cast an OP variable into an std::string.
    *\param[in] STORED_TO_STRING std::string(STORED) : a function to cast a STORED variable into an std::string.
    *\return void
    */
    void saveToFile(const std::string& path,std::string OP_TO_STRING(OP),std::string STORED_TO_STRING(STORED))
    {
        std::ofstream output_file(path.c_str());
        unsigned int n=0;
        if(output_file)
        {
            saveToFile(_first,n,0,0,output_file,OP_TO_STRING,STORED_TO_STRING);
        }
        output_file.close();
    }

    /*!
    *\brief Load a tree from a file.
    *\param[in] path const std::string& : path to the file where the tree is saved.
    *\param[in] STRING_TO_OP OP(std::string) : a function to cast an std::string  into an OP variable.
    *\param[in] STRING_TO_STORED STORED(std::string) : a function to cast an std::string  into a STORED variable.
    *\return void
    */
    void loadFromFile(const std::string& path,OP STRING_TO_OP(std::string),STORED STRING_TO_STORED(std::string))
    {
        if(_first!=nullptr)
            delete _first;
        std::vector<Node*> nodes;
        std::ifstream input_file(path.c_str());
        if(input_file)
        {
            loadFromFile(nodes,input_file,STRING_TO_OP,STRING_TO_STORED);
        }
        input_file.close();
    }

    /*!
    *\brief Works from the tree's root.
    * Optimize the tree's structure to reduce the mean acces time.
    *\return void
    */
    void optimize()
    {
        optimize(_first,&_first);
    }

    /*!
    *\brief Works from the subtree pointed by the cursor.
    * Add a collection of OP to the subtree's structure if it s not already in.
    * Set the cursor to the matching node.
    *\param[in] ops const OP_COL& : the collection of OP
    *\return bool : true if the subtree's structure as been modified, false if the collection of OP was already in the subtree.
    */
    template<class OP_COL>
    bool add(const OP_COL& ops)
    {
        typename OP_COL::const_iterator it=ops.begin();
        if(!find(ops,it,true))
        {
            if(_first==nullptr) // add to first (empty tree)
            {
                _first=new Node();
                _cursor=_first;
            }
            else if(_cursor->_op<*it) //add to bigger
            {
                _cursor->_nb_bigger++;
                _cursor->_bigger=new Node();
                _cursor=_cursor->_bigger;
            }
            else if(_cursor->_op>*it) // add to smaller
            {
                _cursor->_nb_smaller++;
                _cursor->_smaller=new Node();
                _cursor=_cursor->_smaller;
            }
            else if(_cursor->_op==*it) // add to next
            {
                _cursor->_nb_next++;
                _cursor->_next=new Node();
                _cursor=_cursor->_next;
            }
            _cursor->_op=*it;
            it++;
            while(it!=ops.end())
            {
                _cursor->_nb_next++;
                _cursor->_next=new Node();
                _cursor=_cursor->_next;
                _cursor->_op=*it;
                it++;
            }
            return true;
        }
        return false;
    }

    /*!
    *\brief Set the STORED variable of the node pointed by the cursor.
    * Throw an std::runetime_error exception if the tree is empty.
    *\param[in] stored const STORED& : the STORED variable.
    *\return void
    */
    void set(const STORED& stored)
    {
        if(_cursor==nullptr)
            throw std::runtime_error("Use of TernaryTree.set(const STORED& stored) when the tree is empty.");
        _cursor->_stored=stored;
    }

    /*!
    *\brief Set the cursor to the root of the tree.
    *\return void
    */
    void resetCursor()
    {
        _cursor=_first;
    }

    /*!
    *\brief Works from the subtree pointed by the cursor.
    * Test if a collection of OP is in the subtree's structure.
    * Set the cursor to the best matching node.
    *\param[in] ops const OP_COL& : the collection of OP
    *\return bool : true if the collection of OP is in the subtree's structure, false if not.
    */
    template<class OP_COL>
    bool find(const OP_COL& ops)
    {
        typename OP_COL::const_iterator it=ops.begin();
        return find(ops,it,false);
    }

    /*!
    *\brief Works from the subtree pointed by the cursor.
    * Put the cursor to the bigger node if it exist.
    *\return bool : true if the cursor has been modify, false if not.
    */
    bool moveToBigger()
    {
        if(_cursor==nullptr || _cursor->_bigger==nullptr)
            return false;
        _cursor=_cursor->_bigger;
        return true;
    }

    /*!
    *\brief Works from the subtree pointed by the cursor.
    * Put the cursor to the smaller node if it exist.
    *\return bool : true if the cursor has been modify, false if not.
    */
    bool moveToSmaller()
    {
        if(_cursor==nullptr || _cursor->_smaller==nullptr)
            return false;
        _cursor=_cursor->_smaller;
        return true;
    }

    /*!
    *\brief Works from the subtree pointed by the cursor.
    * Put the cursor to the next node if it exist.
    *\return bool : true if the cursor has been modify, false if not.
    */
    bool moveToNext()
    {
        if(_cursor==nullptr || _cursor->_next==nullptr)
            return false;
        _cursor=_cursor->_next;
        return true;
    }

    /*!
    *\brief Test if the cursor point to a leaf.
    *\return bool : true if the cursor point to a leaf, false if not.
    */
    bool cursorIsOnALeaf()
    {
        if(_cursor!=nullptr)
            return _cursor->_bigger==nullptr && _cursor->_smaller==nullptr && _cursor->_next==nullptr;
        return false;
    }

    /*!
    *\brief Works from the root of the tree.
    * Test if the tree is empty.
    *\return bool : true if the tree is empty, false if not.
    */
    bool isEmpty()
    {
        return _first==nullptr;
    }

    /*!
    *\brief Works from the subtree pointed by the cursor.
    * Test is the bigger node exist.
    *\return bool : true if the bigger node exist, false if not.
    */
    bool biggerExist()
    {
        if(_cursor!=nullptr)
            return !_cursor->_bigger==nullptr;
        return false;
    }

    /*!
    *\brief Works from the subtree pointed by the cursor.
    * Test is the next node exist.
    *\return bool : true if the smaller node exist, false if not.
    */
    bool smallerExist()
    {
        if(_cursor!=nullptr)
            return !_cursor->_smaller==nullptr;
        return false;
    }

    /*!
    *\brief Works from the subtree pointed by the cursor.
    * Test is the bigger node exist.
    *\return bool : true if the next node exist, false if not.
    */
    bool nextExist()
    {
        if(_cursor!=nullptr)
            return !_cursor->_next==nullptr;
        return false;
    }

    /*!
    *\brief Return the STORED variable of the node pointed by the cursor.
    * Throw an std::runetime_error exception if the tree is empty.
    *\return STORED : the STORED variable.
    */
    STORED get()
    {
        if(_cursor!=nullptr)
            return _cursor->_stored;
        throw std::runtime_error("Use of TernaryTree.get() when the tree is empty.");
    }

    /*!
    *\brief Return the OP variable of the node pointed by the cursor.
    * Throw an std::runetime_error exception if the tree is empty.
    *\return OP : the OP variable.
    */
    OP getOp()
    {
        if(_cursor!=nullptr)
            return _cursor->_op;
        throw std::runtime_error("Use of TernaryTree.getOp() when the tree is empty.");
    }

    /*!
    *\brief Return the STORED variable of the node at the root.
    * Throw an std::runetime_error exception if the tree is empty.
    *\return STORED : the STORED variable.
    */
    STORED getFirst()
    {
        if(_first!=nullptr)
            return _first->_stored;
        throw std::runtime_error("Use of TernaryTree.getFirst() when the tree is empty.");
    }

    /*!
    *\brief Return the OP variable of the node at the root.
    * Throw an std::runetime_error exception if the tree is empty.
    *\return OP : the OP variable.
    */
    OP getFirstOp()
    {
        if(_first!=nullptr)
            return _first->_op;
        throw std::runtime_error("Use of TernaryTree.getFirstOp() when the tree is empty.");
    }

    /*!
    *\brief Return the STORED variable of the bigger node of the node pointed by the cursor.
    * Throw an std::runetime_error exception if the tree is empty or the node pointed by the cursor does not have a bigger node.
    *\return STORED : the STORED variable.
    */
    STORED getBigger()
    {
        if(_cursor!=nullptr)
        {
            if(_cursor->_bigger!=nullptr)
                return _cursor->_bigger->_stored;
            throw std::runtime_error("Use of TernaryTree.getBigger() when there is no bigger leaf.");
        }
        throw std::runtime_error("Use of TernaryTree.getBigger() when the tree is empty.");
    }

    /*!
    *\brief Return the OP variable of the bigger node of the node pointed by the cursor.
    * Throw an std::runetime_error exception if the tree is empty or the node pointed by the cursor does not have a bigger node.
    *\return OP : the OP variable.
    */
    OP getBiggerOp()
    {
        if(_cursor!=nullptr)
        {
            if(_cursor->_bigger!=nullptr)
                return _cursor->_bigger->_op;
            throw std::runtime_error("Use of TernaryTree.getBiggerOp() when there is no bigger leaf.");
        }
        throw std::runtime_error("Use of TernaryTree.getBiggerOp() when the tree is empty.");
    }

    /*!
    *\brief Return the STORED variable of the smaller node of the node pointed by the cursor.
    * Throw an std::runetime_error exception if the tree is empty or the node pointed by the cursor does not have a smaller node.
    *\return STORED : the STORED variable.
    */
    STORED getSmaller()
    {
        if(_cursor!=nullptr)
        {
            if(_cursor->_smaller!=nullptr)
                return _cursor->_smaller->_stored;
            throw std::runtime_error("Use of TernaryTree.getSmaller() when there is no smaller leaf.");
        }
        throw std::runtime_error("Use of TernaryTree.getSmaller() when the tree is empty.");
    }

    /*!
    *\brief Return the OP variable of the smaller node of the node pointed by the cursor.
    * Throw an std::runetime_error exception if the tree is empty or the node pointed by the cursor does not have a smaller node.
    *\return OP : the OP variable.
    */
    OP getSmallerOp()
    {
        if(_cursor!=nullptr)
        {
            if(_cursor->_smaller!=nullptr)
                return _cursor->_smaller->_op;
            throw std::runtime_error("Use of TernaryTree.getSmallerOp() when there is no smaller leaf.");
        }
        throw std::runtime_error("Use of TernaryTree.getSmallerOp() when the tree is empty.");
    }

    /*!
    *\brief Return the STORED variable of the next node of the node pointed by the cursor.
    * Throw an std::runetime_error exception if the tree is empty or the node pointed by the cursor does not have a next node.
    *\return STORED : the STORED variable.
    */
    STORED getNext()
    {
        if(_cursor!=nullptr)
        {
            if(_cursor->_next!=nullptr)
                return _cursor->_next->_stored;
            throw std::runtime_error("Use of TernaryTree.getNext() when there is no next leaf.");
        }
        throw std::runtime_error("Use of TernaryTree.getNext() when the tree is empty.");
    }

    /*!
    *\brief Return the OP variable of the next node of the node pointed by the cursor.
    * Throw an std::runetime_error exception if the tree is empty or the node pointed by the cursor does not have a next node.
    *\return OP : the OP variable.
    */
    OP getNextOp()
    {
        if(_cursor!=nullptr)
        {
            if(_cursor->_next!=nullptr)
                return _cursor->_next->_op;
            throw std::runtime_error("Use of TernaryTree.getNextOp() when there is no next leaf.");
        }
        throw std::runtime_error("Use of TernaryTree.getNextOp() when the tree is empty.");
    }
};

#endif // TERNARYTREE_HPP_INCLUDED
