/***************************************************************************************************
 * File:         $URL: https://svn.bolitho.us/JHU/Research/StreamingSurfaceReconstruction/Trunk/Reconstructor/Src/Allocator.hpp $
 * Author:       $Author: OBTUSE\matthew $
 * Revision:     $Rev: 719 $
 * Last Updated: $Date: 2008-04-06 09:28:09 -0700 (Sun, 06 Apr 2008) $
 * 
 * 
 * Copyright (c) 2006-2007, Matthew G Bolitho;  Michael Kazhdan
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer. Redistributions in binary form must reproduce
 * the above copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the distribution. 
 * 
 * Neither the name of the Johns Hopkins University nor the names of its contributors
 * may be used to endorse or promote products derived from this software without specific
 * prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 ***************************************************************************************************/

#pragma once

#include "Memory/PoolAllocator.hpp"
using Bolitho::PoolAllocator;

#include "System/Threading/CriticalSection.hpp"
using Bolitho::System::CriticalSection;
#include "System/Threading/ScopedLock.hpp"
using Bolitho::System::Lock;

#include <vector>

class AllocatorState
{
public:
  SIZE_T index,remains;
};
/** This templated class assists in memory allocation and is well suited for instances
* when it is known that the sequence of memory allocations is performed in a stack-based
* manner, so that memory allocated last is released first. It also preallocates memory
* in chunks so that multiple requests for small chunks of memory do not require separate
* system calls to the memory manager.
* The allocator is templated off of the class of objects that we would like it to allocate,
* ensuring that appropriate constructors and destructors are called as necessary.
*/

template<class T>
class Allocator
{
  SIZE_T blockSize;
  SIZE_T index,remains;
public:
  std::vector<T*> memory;
  Allocator()
  {
    blockSize = index = remains = 0;
  }

  Allocator(CONST Allocator& A)
  {
    memory = A.memory;
    blockSize = A.blockSize;
    index = A.index;
    remains = A.remains;
  }

public:
  ~Allocator()
  {
    reset();
  }

  /** This method returns the number of blocks allocated at a time */
  INT BlockSize() const {return blockSize;}


  /** This method is the allocators destructor. It frees up any of the memory that
  * it has allocated. */
  void reset(){
    for(size_t i=0;i<memory.size();i++){delete[] memory[i];}
    memory.clear();
    blockSize=index=remains=0;
  }
  /** This method returns the memory state of the allocator. */
  AllocatorState getState() const{
    AllocatorState s;
    s.index=index;
    s.remains=remains;
    return s;
  }

  /** This method rolls back the allocator so that it makes all of the memory previously
  * allocated available for re-allocation. Note that it does it not call the constructor
  * again, so after this method has been called, assumptions about the state of the values
  * in memory are no longer valid. */
  void DeleteAll()
  {
    if(memory.size()){
      for(SIZE_T i=0;i<memory.size();i++)
        for(SIZE_T j=0;j<blockSize;j++)
        {
          memory[i][j].~T();
          new(&memory[i][j]) T();
        }
        index=0;
        remains=blockSize;
    }
  }
  /** This method rolls back the allocator to the previous memory state and makes all of the memory previously
  * allocated available for re-allocation. Note that it does it not call the constructor
  * again, so after this method has been called, assumptions about the state of the values
  * in memory are no longer valid. */
  void rollBack(const AllocatorState& state){
    if(state.index<index || (state.index==index && state.remains<remains)){
      if(state.index<index){
        for(INT j=state.remains;j<blockSize;j++){
          memory[state.index][j].~T();
          new(&memory[state.index][j]) T();
        }
        for(INT i=state.index+1;i<index-1;i++){
          for(INT j=0;j<blockSize;j++){
            memory[i][j].~T();
            new(&memory[i][j]) T();
          }
        }
        for(INT j=0;j<remains;j++){
          memory[index][j].~T();
          new(&memory[index][j]) T();
        }
        index=state.index;
        remains=state.remains;
      }
      else{
        for(INT j=0;j<state.remains;j<remains){
          memory[index][j].~T();
          new(&memory[index][j]) T();
        }
        remains=state.remains;
      }
    }
  }

  /** This method initiallizes the constructor and the blockSize variable specifies the
  * the number of objects that should be pre-allocated at a time. */
  void SetBlockSize(SIZE_T blockSize)
  {
    reset();
    this->blockSize=blockSize;
    index=-1;
    remains=0;
  }

  /** This method returns a pointer to an array of elements objects. If there is left over pre-allocated
  * memory, this method simply returns a pointer to the next free piece of memory, otherwise it pre-allocates
  * more memory. Note that if the number of objects requested is larger than the value blockSize with which
  * the allocator was initialized, the request for memory will fail.
  */
  //CriticalSection m_Lock;

  T* New(SIZE_T elements)
  {
    //Lock<CriticalSection> L(m_Lock);

    T* mem;
    if(!elements){return NULL;}

    Assert(elements <= blockSize);

    if(remains<elements)
    {
      if(index==memory.size()-1)
      {
        mem=new T[blockSize];
        memory.push_back(mem);
      }
      index++;
      remains=blockSize;
    }
    mem=&(memory[index][blockSize-remains]);
    remains-=elements;
    
    return mem;
  }
  /** These methods are incorporated to allow the allocator to behave like a vector. In order for this to work,
  * it is assumed that elements are allocated in such a manner that all blockSize elements are used.
  */
  void push_back(const T& t)
  {
    if(!remains){
      if(index==memory.size()-1){
        T* mem=new T[blockSize];
        memory.push_back(mem);
      }
      index++;
      remains=blockSize;
    }
    memory[index][blockSize-remains]=t;
    remains--;
  }

  void pop_back()
  {
    if(remains==blockSize)
    {
      index--;
      remains=0;
    }
    if(index>=0)
    {
      remains++;
      memory[index][blockSize-remains].~T();
      new(&memory[index][blockSize-remains]) T();
    }
  }


  SIZE_T size() const
  {
    SIZE_T count=0;
    if (index >= 0)
      count=index*blockSize+blockSize-remains;
    return count;
  }

  T& Get(SIZE_T index)
  {
    return memory[index/blockSize][index%blockSize];
  }

};

