/***************************************************************************************
 *
 *                                Function Buffer
 *
 *   File Name : FunctionBuffer.hpp
 *
 *   Author : Hussam Wael
 *
 *   Description : This file contains an alternative implementation for the std::function that 
 *                 uses a local buffer to store the callable object. 
 *                 
 *           
 * 
 *  Created on : August 14th, 2024
 *
 * **************************************************************************************/
#ifndef __FUNCTIONBUFFER_H__
#define __FUNCTIONBUFFER_H__

#include <type_traits>
#include <cstddef>
#include <utility>
#include <algorithm>
#include <stdexcept>
 /************************************************************************
 * 
 *                           Configurations 
 *                             
 ************************************************************************/

/*This parameter specifies the maximum size of the local buffer: */
constexpr size_t MAX_BUFFER_SIZE = 40; //in bytes

/************************************************************************/

template<typename T>
class FunctionBuffer;

template<typename R, typename ...Args>
class FunctionBuffer<R(Args...)>
{
    /************************************************************************
     * 
     *                        The Function Concept 
     *                             
     ************************************************************************/
    struct FunctionConcept
    {   
        //The function call operator 
        virtual R operator()(Args && ...) = 0;

        //Don't forget to make the destructor virtual
        virtual ~FunctionConcept(){}
    };

    /************************************************************************
     * 
     *                        The Function Model 
     *                             
     ************************************************************************/
    template<typename F>
    struct FunctionModel : FunctionConcept
    {
        private:
        //The callable object
            F m_functor;

        public:

            //Template constructor
            template<typename G>
            FunctionModel(G&& func) : m_functor{std::forward<G>(func)}
            {
            }
        
            //Function call operator
            R operator()(Args && ...args) override
            {
                return m_functor(std::forward<Args>(args)...);
            }

    };

    /************************************************************************
     * 
     *                        The Local Buffer 
     *                             
     ************************************************************************/
    alignas(std::max_align_t) char m_buffer[MAX_BUFFER_SIZE];

    void * const buffer_address = reinterpret_cast<void*>(m_buffer);
    /************************************************************************/

    //Pointer to the callable object
    FunctionConcept *m_func;

public:

    //Templated Constructor:
    template<typename T>
    FunctionBuffer(T && func): m_func{ new(buffer_address) FunctionModel<T>(std::forward<T>(func) )}
    {
        static_assert(sizeof(FunctionModel<T>) <= MAX_BUFFER_SIZE , "The object won't fit in this buffer!");
    }

    //Templated Assignment Operator:
    template<typename T>
    FunctionBuffer& operator=(T && func)
    {   
        if(m_func != nullptr){
            m_func->~FunctionConcept();
        }

        m_func = new(buffer_address) FunctionModel<T>(std::forward<T>(func));
        return *this;
    }

    //Function call operator
    R operator()(Args && ...args)
    {   
        if(!m_func)
            throw std::runtime_error("The function buffer is empty!");

        return (*m_func)(std::forward<Args>(args)...);
    }

    //Destructor
    ~FunctionBuffer()
    {
        if(m_func != nullptr){
            m_func->~FunctionConcept();
        }
    }

    //Copy Constructor:
    FunctionBuffer(const FunctionBuffer& other) : m_buffer{other.m_buffer}
    {

        m_func = reinterpret_cast<FunctionConcept *>(buffer_address);
    }

    //Copy Assignment Operator:
    FunctionBuffer& operator=(const FunctionBuffer& other)
    {   

        if(m_func != nullptr){
            m_func->~FunctionConcept();
        }

        /*Copying the buffer content: */
        std::copy(other.m_buffer , other.m_buffer+MAX_BUFFER_SIZE , m_buffer);

        m_func = reinterpret_cast<FunctionConcept *>(buffer_address);

        return *this;
    }

    //Move Constructor:
    FunctionBuffer(FunctionBuffer&& other) 
    {
        std::copy(other.m_buffer , other.m_buffer+MAX_BUFFER_SIZE , m_buffer);
        m_func = reinterpret_cast<FunctionConcept *>(buffer_address);
        other.m_func = nullptr;
    }

    //Move Assignment Operator:
    FunctionBuffer& operator=(FunctionBuffer&& other)
    {
        if(m_func != nullptr){
            m_func->~FunctionConcept();
        }
        std::copy(other.m_buffer , other.m_buffer+MAX_BUFFER_SIZE , m_buffer);
        m_func = reinterpret_cast<FunctionConcept *>(buffer_address);
        other.m_func = nullptr;
        return *this;
    }

    //Conversion to bool operator to check if the function buffer holds a callable object or not.
    operator bool() const{
        return m_func != nullptr;
    }
};



#endif // __FUNCTIONBUFFER_H__
