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
*\version 1.2.1
*\date 27/08/2020
*/


#ifndef TERNARYTREE_HPP_INCLUDED
#define TERNARYTREE_HPP_INCLUDED

#include <stdexcept>
#include <fstream>
#include <deque>

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
		// template variables
		OP _op;
		STORED* _stored;

		// structure variables
		Node* _greater;
		Node* _smaller;
		Node* _next;

		// additional variables
		#ifndef TERNARYTREE_LIGHT
		unsigned int _nb_greater;
		unsigned int _nb_smaller;
		unsigned int _nb_next;
		unsigned int* _father_nb;
		bool _optimize_in_process;
		Node* _father;
		Node** _father_link;
		#endif // TERNARYTREE_LIGHT

		/*!
		*\brief Constructor of the Node class
		*/
		Node() : _stored(nullptr), _greater(nullptr),_smaller(nullptr),_next(nullptr)
		{
			// additional variables
			#ifndef TERNARYTREE_LIGHT
			_nb_greater=0;
			_nb_smaller=0;
			_nb_next=0;
			_father_nb=nullptr;
			_optimize_in_process=false;
			_father=nullptr;
			_father_link=nullptr;
			#endif // TERNARYTREE_LIGHT
		}

		/*!
		*\brief Destrutor of the Node class
		*/
		~Node()
		{
			if(_stored!=nullptr)
				delete _stored;
			if(_next!=nullptr)
				delete _next;
			if(_greater!=nullptr)
				delete _greater;
			if(_smaller!=nullptr)
				delete _smaller;
		}
	};

	Node* _first=nullptr;
	Node* _cursor=nullptr;

	unsigned int _nb_node=0;
	unsigned int _nb_stored=0;

	#ifndef TERNARYTREE_LIGHT
	bool find_to_add;
	#endif // TERNARYTREE_LIGHT


	/*!
	*\brief Private terminal recursive function.
	* Works from the subtree pointed by the cursor.
	* Test if a collection of OP is in the subtree's structure.
	* Set the cursor to the best matching node.
	*\param[in] ops const OP_COL& : a collection of OP.
	*\param[in] it OP_COL::const_iterator& : a const_iterator for the collection of OP.
	*\return bool : true if the collection of OP is in the subtree's structure, false if not.
	*/
	template<class OP_COL>
	bool find(const OP_COL& ops,typename OP_COL::const_iterator& it)
	{
		if(_cursor==nullptr)
			return false;

		if(_cursor->_op==*it)
		{
			if(it+1==ops.end())
				return true;

			if(_cursor->_next==nullptr)
				return false;

			#ifndef TERNARYTREE_LIGHT
			if(find_to_add)
				_cursor->_nb_next++;
			#endif // TERNARYTREE_LIGHT

			_cursor=_cursor->_next;
			return find(ops,++it);
		}

		if(_cursor->_op<*it)
		{
			if(_cursor->_greater==nullptr)
				return false;

			#ifndef TERNARYTREE_LIGHT
			if(find_to_add)
				_cursor->_nb_greater++;
			#endif // TERNARYTREE_LIGHT

			_cursor=_cursor->_greater;
			return find(ops,it);
		}

		if(_cursor->_op>*it)
		{
			if(_cursor->_smaller==nullptr)
				return false;

			#ifndef TERNARYTREE_LIGHT
			if(find_to_add)
				_cursor->_nb_smaller++;
			#endif // TERNARYTREE_LIGHT

			_cursor=_cursor->_smaller;
			return find(ops,it);
		}

		return false;
	}

	#ifndef TERNARYTREE_LIGHT
	/*!
	*\brief Private terminal recursive function.
	* Propagate a modification of the optimization's variable to all the father nodes.
	*\param[in] current Node* : the current node.
	*\param[in] delta int : the modification.
	*\return void
	*/
	void propagateForOptimization(Node* current,int delta)
	{
		if(current!=nullptr && current->_father_nb!=nullptr)
		{
			current->_father_nb+=delta;
			propagateForOptimization(current->_father,delta);
		}
	}

	/*!
	*\brief Private terminal recursive function.
	* Return the smallest leaf of a given subtree.
	*\param[in] current Node* : the current node.
	*\param[in] nb unsigned int : a paramater to update optimization variables. Default value is 0.
	*\return Node* : the smallest leaf of the given tree.
	*/
	Node*  getSmallestLeaf(Node* current,unsigned int nb=0)
	{
		current->_nb_smaller+=nb;
		if(current->_smaller!=nullptr)
			return getSmallestLeaf(current->_smaller);
		return current;
	}

	/*!
	*\brief Private terminal recursive function.
	* Return the greatest leaf of a given subtree.
	*\param[in] current Node* : the current node.
	*\param[in] nb unsigned int : a paramater to update optimization variables. Default value is 0.
	*\return Node* : the greatest leaf of the given tree.
	*/
	Node*  getGreatestLeaf(Node* current,unsigned int nb=0)
	{
		current->_nb_greater+=nb;
		if(current->_greater!=nullptr)
			return getGreatestLeaf(current->_greater);
		return current;
	}

	/*!
	*\brief Private recursive function.
	* Optimize the tree's structure to reduce the mean acces time.
	*\param[in] current Node* : the current node.
	*\param[in] father_link Node** : a pointer to the father's link of the current node.
	*\return void
	*/
	void optimize(Node* current)
	{
		if(current!=nullptr)
		{
			current->_optimize_in_process=true;
			if(current->_nb_next+current->_nb_smaller+1<current->_nb_greater) // too much in greater
			{
				Node* new_root=current->_greater;
				Node* new_father;
				int self_nb=0;

				if(current->_stored!=nullptr)
					self_nb=1;

				current->_nb_greater=0;
				current->_greater=nullptr;

				(*current->_father_link)=new_root;
				new_father=getSmallestLeaf(new_root,current->_nb_next+current->_nb_smaller+self_nb);
				new_father->_smaller=current;

				new_root->_father=current->_father;
				new_root->_father_link=current->_father_link;
				current->_father=new_father;
				current->_father_link=(&new_father->_smaller);

				if(!new_root->_optimize_in_process)
					optimize(new_root);
			}
			else
			{
				if(current->_greater!=nullptr && !current->_greater->_optimize_in_process)
					optimize(current->_greater);
			}
			if(current->_nb_next+current->_nb_greater+1<current->_nb_smaller) // too much in smaller
			{
				Node* new_root=current->_smaller;
				Node* new_father;
				int self_nb=0;

				if(current->_stored!=nullptr)
					self_nb=1;

				current->_nb_smaller=0;
				current->_smaller=nullptr;

				(*current->_father_link)=new_root;
				new_father=getGreatestLeaf(new_root,current->_nb_next+current->_nb_greater+self_nb);
				new_father->_greater=current;

				new_root->_father=current->_father;
				new_root->_father_link=current->_father_link;
				current->_father=new_father;
				current->_father_link=(&new_father->_greater);

				if(!new_root->_optimize_in_process)
					optimize(new_root);
			}
			else
			{
				if(current->_smaller!=nullptr && !current->_smaller->_optimize_in_process)
					optimize(current->_smaller);
			}
			optimize(current->_next);
			current->_optimize_in_process=false;
		}
	}
	#endif // TERNARYTREE_LIGHT

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
			if(current->_stored!=nullptr)
				output_file <<  STORED_TO_STRING((*current->_stored));
			output_file << "\n<#> end\n\n";
			saveToFile(current->_smaller,n,current_n,1,output_file,OP_TO_STRING,STORED_TO_STRING);
			saveToFile(current->_greater,n,current_n,2,output_file,OP_TO_STRING,STORED_TO_STRING);
			saveToFile(current->_next,n,current_n,3,output_file,OP_TO_STRING,STORED_TO_STRING);
		}
	}

	/*!
	*\brief Private function.
	* Load a tree from a file.
	*\param[in,out] nodes std::deque<Node*> : a deque of Node* used to create the links between nodes.
	*\param[in,out] input_file std::ifstream& : the file where the tree is saved.
	*\param[in] STRING_TO_OP OP(std::string) : a function to cast an std::string  into an OP variable.
	*\param[in] STRING_TO_STORED STORED(std::string) : a function to cast an std::string  into a STORED variable.
	*\return void
	*/
	void loadFromFile(std::deque<Node*>& nodes,std::ifstream& input_file,OP STRING_TO_OP(std::string),STORED STRING_TO_STORED(std::string))
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
						_first=new Node();
						nodes.push_back(_first);
						break;

					case 1: //smaller
						nodes[father_id]->_smaller=new Node();
						nodes.push_back(nodes[father_id]->_smaller);

						#ifndef TERNARYTREE_LIGHT
						nodes.back()->_father_link=&nodes[father_id]->_smaller;
						nodes.back()->_father_nb=(&nodes[father_id]->_nb_smaller);
						#endif // TERNARYTREE_LIGHT

						break;

					case 2: //greater
						nodes[father_id]->_greater=new Node();
						nodes.push_back(nodes[father_id]->_greater);

						#ifndef TERNARYTREE_LIGHT
						nodes.back()->_father_link=&nodes[father_id]->_greater;
						nodes.back()->_father_nb=(&nodes[father_id]->_nb_greater);
						#endif // TERNARYTREE_LIGHT

						break;

					case 3: //next
						nodes[father_id]->_next=new Node();
						nodes.push_back(nodes[father_id]->_next);

						#ifndef TERNARYTREE_LIGHT
						nodes.back()->_father_link=&nodes[father_id]->_next;
						nodes.back()->_father_nb=(&nodes[father_id]->_nb_next);
						#endif // TERNARYTREE_LIGHT

						break;

					default:
						//not supposed to go here
						break;
					}

					#ifndef TERNARYTREE_LIGHT
					nodes.back()->_father=nodes[father_id];
					#endif // TERNARYTREE_LIGHT

					nodes.back()->_op=STRING_TO_OP(str_op);

					if(!str_stored.empty())
					{
						nodes.back()->_stored=new STORED;
						(*nodes.back()->_stored)=STRING_TO_STORED(str_stored);

						#ifndef TERNARYTREE_LIGHT
						propagateForOptimization(nodes.back(),1);
						#endif // TERNARYTREE_LIGHT
					}

					node_id=0;
					father_id=0;
					link_type=0;
					str_op.clear();
					str_stored.clear();
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
	*\param[in] reset_cursor bool : set by default to true. If true the fonction will place the cursor to the root of the tree before starting.
	*\return void
	*/
	void saveToFile(const std::string& path,std::string OP_TO_STRING(OP),std::string STORED_TO_STRING(STORED),bool reset_cursor=true)
	{
		if(reset_cursor)
			resetCursor();
		std::ofstream output_file(path.c_str());
		unsigned int n=0;
		if(output_file)
		{
			saveToFile(_cursor,n,0,0,output_file,OP_TO_STRING,STORED_TO_STRING);
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
		std::deque<Node*> nodes;
		std::ifstream input_file(path.c_str());
		if(input_file)
		{
			loadFromFile(nodes,input_file,STRING_TO_OP,STRING_TO_STORED);
		}
		input_file.close();
	}

	#ifndef TERNARYTREE_LIGHT
	/*!
	*\brief Works from the tree's root.
	* Optimize the tree's structure to reduce the mean acces time.
	*\return void
	*/
	void optimize()
	{
		optimize(_first);
	}

	/*!
	*\brief Remove the node pointed by the cursor.
	* Set the cursor to the root of the tree.
	*\param[in] ops const OP_COL& : the collection of OP
	*\param[in] reset_cursor bool : set by default to true. If true the fonction will place the cursor to the root of the tree before starting.
	*\return bool : true if the tree's structure as been modified, false if the collection of OP was not in the tree's structure.
	*/
	void remove()
	{
		if(_cursor!=nullptr)
		{
			if(_cursor->_next!=nullptr)
			{
				if(_cursor->_stored!=nullptr)
				{
					delete _cursor->_stored;
					_nb_stored--;
					propagateForOptimization(_cursor,-1);
				}
			}
			else
			{
				if(_cursor->_smaller!=nullptr && _cursor->_greater==nullptr)
				{
					(*_cursor->_father_link)=_cursor->_smaller;
				}
				else if(_cursor->_smaller==nullptr && _cursor->_greater!=nullptr)
				{
					(*_cursor->_father_link)=_cursor->_greater;
				}
				else if(_cursor->_smaller!=nullptr && _cursor->_greater!=nullptr)
				{
					Node* new_father_of_smaller;

					(*_cursor->_father_link)=_cursor->_greater;
					new_father_of_smaller=getSmallestLeaf(_cursor->_greater,_cursor->_nb_smaller);
					new_father_of_smaller->_smaller=_cursor->_smaller;

					_cursor->_greater->_father=_cursor->_father;
					_cursor->_greater->_father_link=_cursor->_father_link;
					_cursor->_smaller->_father=new_father_of_smaller;
					_cursor->_smaller->_father_link=(&new_father_of_smaller->_smaller);
				}
				else //leaf
				{
					(*_cursor->_father_link)=nullptr;
					Node* cursor_save=_cursor;
					_cursor=_cursor->_father;
					while(cursorIsOnALeaf() && _cursor->_stored==nullptr)
					{
						(*_cursor->_father_link)=nullptr;
						_nb_node--;
						_cursor=_cursor->_father;
					}
					_cursor=cursor_save;
				}
				_cursor->_greater=nullptr;
				_cursor->_smaller=nullptr;
				if(_cursor->_stored!=nullptr)
				{
					_nb_stored--;
					propagateForOptimization(_cursor,-1);
				}
				_nb_node--;
				delete _cursor;
			}
		}
		resetCursor();
	}

	/*!
	*\brief Remove a collection of OP to the tree's structure if it s in.
	* Set the cursor to the root of the tree.
	*\param[in] ops const OP_COL& : the collection of OP
	*\param[in] reset_cursor bool : set by default to true. If true the fonction will place the cursor to the root of the tree before starting.
	*\return bool : true if the tree's structure as been modified, false if the collection of OP was not in the tree's structure.
	*/
	template<class OP_COL>
	bool remove(const OP_COL& ops, bool reset_cursor=true)
	{
		if(reset_cursor)
			resetCursor();
		typename OP_COL::const_iterator it=ops.begin();
		if(find(ops,it))
		{
			remove();
			return true;
		}
		return false;
	}
	#endif // TERNARYTREE_OPTIMIZE_UNABLE

	/*!
	*\brief Delete all the tree's node.
	*\return void
	*/
	void clear()
	{
		if(_first!=nullptr)
			delete _first;
		_first=nullptr;
		_cursor=nullptr;
		_nb_node=0;
		_nb_stored=0;
	}

	/*!
	*\brief Add a collection of OP to the tree's structure if it s not already in.
	* Set the cursor to the matching node.
	*\param[in] ops const OP_COL& : the collection of OP
	*\param[in] reset_cursor bool : set by default to true. If true the fonction will place the cursor to the root of the tree before starting.
	*\return bool : true if the tree's structure as been modified, false if the collection of OP was already in the tree's structure.
	*/
	template<class OP_COL>
	bool add(const OP_COL& ops, bool reset_cursor=true)
	{
		if(reset_cursor)
			resetCursor();

		#ifndef TERNARYTREE_LIGHT
		find_to_add=true;
		#endif // TERNARYTREE_LIGHT

		typename OP_COL::const_iterator it=ops.begin();
		if(!find(ops,it))
		{
			if(_first==nullptr) // add to first (empty tree)
			{
				_first=new Node();
				_nb_node++;

				_cursor=_first;
				_cursor->_op=*it;

				#ifndef TERNARYTREE_LIGHT
				_cursor->_father_link=(&_first);
				#endif // TERNARYTREE_LIGHT
			}
			else if(_cursor->_op<*it) //add to greater
			{
				_cursor->_greater=new Node();
				_nb_node++;

				#ifndef TERNARYTREE_LIGHT
				_cursor->_nb_greater++;
				_cursor->_greater->_father_nb=(&_cursor->_nb_greater);
				_cursor->_greater->_father=_cursor;
				_cursor->_greater->_father_link=(&_cursor->_greater);
				#endif // TERNARYTREE_LIGHT

				_cursor=_cursor->_greater;
				_cursor->_op=*it;
			}
			else if(_cursor->_op>*it) // add to smaller
			{
				_cursor->_smaller=new Node();
				_nb_node++;

				#ifndef TERNARYTREE_LIGHT
				_cursor->_nb_smaller++;
				_cursor->_smaller->_father_nb=(&_cursor->_nb_smaller);
				_cursor->_smaller->_father=_cursor;
				_cursor->_smaller->_father_link=(&_cursor->_smaller);
				#endif // TERNARYTREE_LIGHT

				_cursor=_cursor->_smaller;
				_cursor->_op=*it;
			}
			while(it+1!=ops.end())
			{
				_cursor->_next=new Node();
				_nb_node++;

				#ifndef TERNARYTREE_LIGHT
				_cursor->_nb_next++;
				_cursor->_next->_father_nb=(&_cursor->_nb_next);
				_cursor->_next->_father=_cursor;
				_cursor->_next->_father_link=(&_cursor->_next);
				#endif // TERNARYTREE_LIGHT

				it++;
				_cursor=_cursor->_next;
				_cursor->_op=*it;
			}

			#ifndef TERNARYTREE_LIGHT
			find_to_add=false;
			#endif // TERNARYTREE_LIGHT

			return true;
		}

		#ifndef TERNARYTREE_LIGHT
		find_to_add=false;
		#endif // TERNARYTREE_LIGHT

		return false;
	}

	/*!
	*\brief Set the STORED variable of the node pointed by the cursor.
	* Throw an std::runetime_error exception if the tree is empty.
	*\param[in] stored STORED : the STORED variable.
	*\return void
	*/
	void set(STORED stored)
	{
		if(_cursor==nullptr)
			throw std::runtime_error("Use of TernaryTree.set(const STORED& stored) when the tree is empty.");
		if(_cursor->_stored==nullptr)
		{
			_cursor->_stored=new STORED;
			_nb_stored++;
		}
		(*_cursor->_stored)=stored;
	}

	/*!
	*\brief Add a collection of OP to the tree's structure if it s not already in.
	* Set the cursor to the matching node.
	* Set the STORED variable of the node pointed by the cursor
	*\param[in] ops const OP_COL& : the collection of OP
	*\param[in] stored STORED : the STORED variable
	*\param[in] reset_cursor bool : set by default to true. If true the fonction will place the cursor to the root of the tree before starting.
	*\return bool : true if the tree's structure as been modified, false if the collection of OP was already in the tree's structure.
	*/
	template<class OP_COL>
	bool add(const OP_COL& ops,STORED stored, bool reset_cursor=true)
	{
		bool r_bool=add(ops,reset_cursor);
		set(stored);
		return r_bool;
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
	*\brief Test if a collection of OP is in the tree's structure.
	* Set the cursor to the best matching node.
	*\param[in] ops const OP_COL& : the collection of OP
	*\param[in] reset_cursor bool : set by default to true. If true the fonction will place the cursor to the root of the tree before starting.
	*\return bool : true if the collection of OP is in the tree's structure, false if not.
	*/
	template<class OP_COL>
	bool find(const OP_COL& ops, bool reset_cursor=true)
	{
		if(reset_cursor)
			resetCursor();
		typename OP_COL::const_iterator it=ops.begin();
		return find(ops,it);
	}

	/*!
	*\brief Works from the subtree pointed by the cursor.
	* Put the cursor to the greater node if it exist.
	*\return bool : true if the cursor has been modify, false if not.
	*/
	bool moveToGreater()
	{
		if(_cursor==nullptr || _cursor->_greater==nullptr)
			return false;
		_cursor=_cursor->_greater;
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
	*\brief Works from the subtree pointed by the cursor.
	* Test if the node has a STORED variable.
	*\return bool : true if the greater node exist, false if not.
	*/
	bool hasAStored()
	{
		return !(_cursor->_stored==nullptr);
	}

	/*!
	*\brief Test if the cursor point to a leaf.
	*\return bool : true if the cursor point to a leaf, false if not.
	*/
	bool cursorIsOnALeaf()
	{
		if(_cursor!=nullptr)
			return _cursor->_greater==nullptr && _cursor->_smaller==nullptr && _cursor->_next==nullptr;
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
	* Test if the greater node exist.
	*\return bool : true if the greater node exist, false if not.
	*/
	bool greaterExist()
	{
		return !_cursor->_greater==nullptr;
	}

	/*!
	*\brief Works from the subtree pointed by the cursor.
	* Test if the next node exist.
	*\return bool : true if the smaller node exist, false if not.
	*/
	bool smallerExist()
	{
		return !_cursor->_smaller==nullptr;
	}

	/*!
	*\brief Works from the subtree pointed by the cursor.
	* Test if the greater node exist.
	*\return bool : true if the next node exist, false if not.
	*/
	bool nextExist()
	{
		return !_cursor->_next==nullptr;
	}

	/*!
	*\brief Return the tree's number of node.
	*\return unsigned int : the number of node.
	*/
	unsigned int getNbNode()
	{
		return _nb_node;
	}

	/*!
	*\brief Return the number of STORED variable in the tree.
	*\return unsigned int : the number of STORED variable.
	*/
	unsigned int getNbStored()
	{
		return _nb_stored;
	}

	/*!
	*\brief Return the STORED variable of the node pointed by the cursor.
	* Throw an std::runetime_error exception if the STORED variable does not exist.
	*\return STORED : the STORED variable.
	*/
	STORED get()
	{
		if(_cursor!=nullptr)
		{
			if(_cursor->_stored!=nullptr)
				return (*_cursor->_stored);
			throw std::runtime_error("Use of TernaryTree.get() when the node does not have a STORED variable.");
		}
		throw std::runtime_error("Use of TernaryTree.get() when the tree is empty.");
	}

	/*!
	*\brief Return the OP variable of the node pointed by the cursor.
	* Throw an std::runetime_error exception if the OP variable does not exist.
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
	* Throw an std::runetime_error exception if the STORED variable does not exist.
	*\return STORED : the STORED variable.
	*/
	STORED getFirst()
	{
		if(_first!=nullptr)
		{
			if(_first->_stored!=nullptr)
				return (*_first->_stored);
			throw std::runtime_error("Use of TernaryTree.getFirst() when the root does not have a STORED variable.");
		}
		throw std::runtime_error("Use of TernaryTree.getFirst() when the tree is empty.");
	}

	/*!
	*\brief Return the OP variable of the node at the root.
	* Throw an std::runetime_error exception if the OP variable does not exist.
	*\return OP : the OP variable.
	*/
	OP getFirstOp()
	{
		if(_first!=nullptr)
			return _first->_op;
		throw std::runtime_error("Use of TernaryTree.getFirstOp() when the tree is empty.");
	}

	/*!
	*\brief Return the STORED variable of the greater node of the node pointed by the cursor.
	* Throw an std::runetime_error exception if the STORED variable does not exist.
	*\return STORED : the STORED variable.
	*/
	STORED getGreater()
	{
		if(_cursor!=nullptr)
		{
			if(_cursor->_greater!=nullptr)
			{
				if(_cursor->_greater->_stored!=nullptr)
					return (*_cursor->_greater->_stored);
				throw std::runtime_error("Use of TernaryTree.getGreater() when the greater node does not have a STORED variable.");
			}
			throw std::runtime_error("Use of TernaryTree.getGreater() when there is no greater node.");
		}
		throw std::runtime_error("Use of TernaryTree.getGreater() when the tree is empty.");
	}

	/*!
	*\brief Return the OP variable of the greater node of the node pointed by the cursor.
	* Throw an std::runetime_error exception if the OP variable does not exist.
	*\return OP : the OP variable.
	*/
	OP getGreaterOp()
	{
		if(_cursor!=nullptr)
		{
			if(_cursor->_greater!=nullptr)
				return _cursor->_greater->_op;
			throw std::runtime_error("Use of TernaryTree.getGreaterOp() when there is no greater node.");
		}
		throw std::runtime_error("Use of TernaryTree.getGreaterOp() when the tree is empty.");
	}

	/*!
	*\brief Return the STORED variable of the smaller node of the node pointed by the cursor.
	* Throw an std::runetime_error exception if the STORED variable does not exist.
	*\return STORED : the STORED variable.
	*/
	STORED getSmaller()
	{
		if(_cursor!=nullptr)
		{
			if(_cursor->_smaller!=nullptr)
			{
				if(_cursor->_smaller->_stored!=nullptr)
					return (*_cursor->_smaller->_stored);
				throw std::runtime_error("Use of TernaryTree.getSmaller() when the smaller node does not have a STORED variable.");
			}
			throw std::runtime_error("Use of TernaryTree.getSmaller() when there is no smaller node.");
		}
		throw std::runtime_error("Use of TernaryTree.getSmaller() when the tree is empty.");
	}

	/*!
	*\brief Return the OP variable of the smaller node of the node pointed by the cursor.
	* Throw an std::runetime_error exception if the OP variable does not exist.
	*\return OP : the OP variable.
	*/
	OP getSmallerOp()
	{
		if(_cursor!=nullptr)
		{
			if(_cursor->_smaller!=nullptr)
				return _cursor->_smaller->_op;
			throw std::runtime_error("Use of TernaryTree.getSmallerOp() when there is no smaller node.");
		}
		throw std::runtime_error("Use of TernaryTree.getSmallerOp() when the tree is empty.");
	}

	/*!
	*\brief Return the STORED variable of the next node of the node pointed by the cursor.
	* Throw an std::runetime_error exception if the STORED variable does not exist.
	*\return STORED : the STORED variable.
	*/
	STORED getNext()
	{
		if(_cursor!=nullptr)
		{
			if(_cursor->_next!=nullptr)
			{
				if(_cursor->_next->_stored!=nullptr)
					return (*_cursor->_next->_stored);
				throw std::runtime_error("Use of TernaryTree.getNext() when the next node does not have a STORED variable.");
			}
			throw std::runtime_error("Use of TernaryTree.getNext() when there is no next node.");
		}
		throw std::runtime_error("Use of TernaryTree.getNext() when the tree is empty.");
	}

	/*!
	*\brief Return the OP variable of the next node of the node pointed by the cursor.
	* Throw an std::runetime_error exception if the OP variable does not exist.
	*\return OP : the OP variable.
	*/
	OP getNextOp()
	{
		if(_cursor!=nullptr)
		{
			if(_cursor->_next!=nullptr)
				return _cursor->_next->_op;
			throw std::runtime_error("Use of TernaryTree.getNextOp() when there is no next node.");
		}
		throw std::runtime_error("Use of TernaryTree.getNextOp() when the tree is empty.");
	}
};

#endif // TERNARYTREE_HPP_INCLUDED
