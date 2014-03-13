/**
\file Callstack.h
\author Shane Lillie
\brief Callstack debugging module header.

\verbatim
Copyright 2002-2003 Energon Software

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
\endverbatim
*/


#if !defined CALLSTACK_H
#define CALLSTACK_H


#include <string>
#include <iostream>
#include <fstream>


/**
\namespace Callstack
\brief Callstack related constructs.

Constructs contained in this namespace may be used for source debugging.
They provide a means to track function entry and count.
*/
namespace Callstack
{
    /**
    \class Stack
    \brief A stack datastructure for tracking function calls.
    */
    class Stack
    {
    private:
        /**
        \class Node
        \brief A node in the stack.
        */
        class Node
        {
        public:
            /**
            \brief Constructs a node on the stack.
            @param next The node that will come after this one in the stack.
            @param function_name The name of the function called.
            @param filename The value of __FILE__.
            @param line The value of __LINE__.
            */
            Node(Node* const next, const std::string& function_name, const std::string& filename, long line) throw()
                : m_next(next), m_function(function_name), m_file(filename), m_line(line) { }

        public:
            /**
            @return The next node in the stack.
            */
            Node* const next() const throw() { return m_next; }

        public:
            /**
            \brief Prints the values of the node.
            @param lhs The output stream to print to.
            @param rhs The node to print.
            */
            friend std::ostream& operator<<(std::ostream& lhs, const Node& rhs) throw()
            {
                lhs << rhs.m_function << "() [" << rhs.m_file << "]: " << rhs.m_line;
                return lhs;
            }

        private:
            Node* m_next;
            std::string m_function;
            std::string m_file;
            long m_line;
        };

    public:
        /**
        \brief Constructs an empty stack.
        */
        Stack() throw() : m_head(NULL), m_size(0) { }

        /**
        \brief Frees the nodes in the stack.
        */
        ~Stack() throw() { while(size() > 0) pop(); }

    public:
        /**
        \brief Returns the size (number of nodes) of the stack.
        */
        size_t size() const throw() { return m_size; }

        /**
        \brief Pushes a function call onto the stack.
        @param function_name The name of the function called.
        @param filename The value of __FILE__.
        @param line The value of __LINE__.
        */
        void push(const std::string& function_name, const std::string& filename, long line) throw()
        {
            if(!m_head) m_head = new Node(NULL, function_name, filename, line);
            else m_head = new Node(m_head, function_name, filename, line);
            ++m_size;
        }

        /**
        \brief Pops the top function call from the stack.
        */
        void pop() throw()
        {
            if(!size()) return;
            Node* t = m_head;
            m_head = m_head->next();
            delete t;
            --m_size;
        }

    public:
        /**
        \brief Prints all of the nodes in the stack, one to a line.
        @param lhs The output stream to print to.
        @param rhs The stack to print.
        */
        friend std::ostream& operator<<(std::ostream& lhs, const Stack& rhs) throw()
        {
            Node* current = rhs.m_head;
            while(current) {
                lhs << *current << std::endl;
                current = current->next();
            }
            return lhs;
        }

    private:
        Node* m_head;
        size_t m_size;
    };

    /**
    \class EnterFunction
    \brief A class to facilitate pushing and popping function calls.

    Create an instance of the class with the function name, __FILE__, __LINE__
    and the destructor will take care of popping the function call.
    */
    class EnterFunction
    {
    public:
        /**
        \brief Dumps to the contents of the current call stack as well as the total number of functions called.
        @param out The output stream to dump to.
        @note This doesn't remove any elements from the stack.
        */
        static void dump(std::ostream& out) throw();

    private:
        static Stack callstack;
        static long long calls;

    public:
        /**
        \brief Pushes a function call onto the call stack.
        @param function_name The name of the function called.
        @param filename The value of __FILE__.
        @param line The value of __LINE__.
        */
        EnterFunction(const std::string& function_name, const std::string& filename, long line) throw()
        {
            callstack.push(function_name, filename, line);
            ++calls;
        }

        /**
        \brief Pops the head of the call stack.
        */
        ~EnterFunction() throw() { callstack.pop(); }
    };

/* Call the next two functions on an error */

    /**
    \brief Dumps the call stack to an output stream.
    @param out The output stream to dump to.
    */
    void dump(std::ostream& out) throw();

    /**
    \brief Dumps the call stack to a file.
    @param filename The name of the file to dump to.
    @note This overwrites the contents of the file.
    */
    void dump(const std::string& filename) throw();
};


/*
Use these macros to push a function call
The EnterFUnction class destructor will pop for you.
*/
#if defined DEBUG
    /**
    \def ENTER_FUNCTION
    @param name The name of the function called.
    @note This makes use of the variable name enter_function.
    */
    #define ENTER_FUNCTION(name) Callstack::EnterFunction enter_function(#name, __FILE__, __LINE__)
#else
    /**
    \def ENTER_FUNCTION
    \brief Placeholder for non-debug builds.
    */
    #define ENTER_FUNCTION(name)
#endif

#endif
