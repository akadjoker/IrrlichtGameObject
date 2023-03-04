/*$T Lists.h GC 1.138 11/09/03 16:39:55 */

/*$I0
  ***************************************************************
  * 6DX 2.0                                                    *
  * Copyright (c) 2002,2003 Aztica.Com LLC                      *
  * Portions Copyright (c) 1999, 2000,2001 JRR McEntire         *
  ***************************************************************
  *                                                             *
  * Permission is hereby granted, free of charge, to            *
  * any person obtaining a copy of this software and            *
  * associated documentation files (the "Software"),            *
  * to deal in the Software without restriction,                *
  * including without limitation the rights to use,             *
  * copy, modify, merge, publish, distribute, sublicense,       *
  * and/or sell copies of the Software, and to permit           *
  * persons to whom the Software is furnished to do so,         *
  * subject to the following conditions:                        *
  *                                                             *
  * The above copyright notice and this permission notice       *
  * shall be included in all copies or substantial portions     *
  * of the Software.                                            *
  *                                                             *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY   *
  * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO      *
  * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR *
  * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS  *
  * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR    *
  * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT     *
  * OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH    *
  * THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  *
  *************************************************************** */

#pragma once
#pragma warning(disable : 4530)                 // Warning us that strings want to use try/catch.
#pragma warning(disable : 4786)                 // We know basic_string generates long names :-)
#include <algorithm>
#include <list>
#include <string>
using namespace std;
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <typeinfo>
template<class Type> class TList
  {
    unsigned int  maxitems;
  public:
    unsigned int  frame;
    unsigned int  count;
    Type          **items;
    TList(void);
    ~TList(void);
    unsigned int  Add(Type *element);
    void          Expand(unsigned int count);

    // deletes the element from the list.
    // Danger! This will reset the index numbers, so
    // don't use this when you are using the index!!!
    void          Delete(Type *element);
  };
template<class Type> TList<Type>::TList (void)
  {
    maxitems = 0;
    frame = -1;
    items = NULL;                               // (Type **)malloc(maxitems*sizeof(Type *));
    count = 0;
  }
template<class Type> TList<Type>::~TList (void)
  {
    if(items) free(items);
  }
template<class Type>

unsigned int TList<Type>::Add(Type *element)
  {
    if(count > 500000)
      {
      return (0);
      }

    if(count >= maxitems)
      {
      if(maxitems < 1)
        Expand(64);
      else
        Expand(maxitems + (maxitems >> 1));
      }

    items[count++] = element;
    return (count - 1);
  }


template<class Type> void TList<Type>::Delete(Type *element)
  {
    Type  *temp[8192];                          // max 8192 items.
    int   tcount = 0;
    for(unsigned int i = 0; i < count; i++)
      {
      if(items[i] != element)
        {
        // items[i] = NULL;
        temp[tcount] = items[i];
        tcount++;
        }
      }

    count = tcount;
    for(int i = 0; i < count; i++)
      {
      items[i] = temp[i];
      }
  }


template<class Type> void TList<Type>::Expand(unsigned int newsize)
  {
    Type  **newitems = (Type **)malloc(newsize * sizeof(Type *));
    if(items)
      {
      for(unsigned int i = 0; i < count; i++) newitems[i] = items[i];
      free(items);
      }

    // reassign variables.
    items = newitems;
    maxitems = newsize;
  }

class TIntList
  {
    unsigned int  maxitems;
  public:
    unsigned int  count;
    unsigned int  *items;
    TIntList(void);
    ~TIntList(void);
    unsigned int  Add(unsigned int element);
    unsigned int  Append(unsigned int element);
    unsigned int  AddShorts(short p1, short p2);
    void          GetShorts(unsigned int index, short &p1, short &p2);
    void          Expand(unsigned int count);
    void          Fill(unsigned int fillnum);
  };
template<class Type> class TOList
  {
    unsigned int  maxitems;
  public:
    unsigned int  count;
    Type          *items;
    TOList(void);
    TOList(unsigned int in_maxitems);
    ~TOList(void);
    Type  *GetNext(void);
    void  Expand(unsigned int count);
  };

// objectlist keeps an array of pointers.
// it allocates a new one with AppendReuse().
// you can add your own to it with the Add() function.
// you can delete one manually, but set the pointer to NULL
//     so it doesn't get freed again!
// on cleanup, it deletes all the objects.
template<class Type> class TObjectList
  {
    string        Name;
    unsigned int  maxitems;
  public:
    unsigned int  count;
    int           lastadd;
    Type          **items;
    TObjectList(void);
    TObjectList(string &name);
    ~TObjectList(void);
    Type  *Append(void);
    Type  *Add(Type *);
    Type  *AddReuse(Type *);
    Type  *AppendReuse(void);
    void  DeleteItem(Type *);
    void  Expand(unsigned int count);
    void  Clear(void);                          // frees all the members. Resets the count to 0.
  };
template<class Type> TObjectList<Type>::TObjectList(string & name)
  {
    Name = name;
    maxitems = 4;
    items = (Type **)malloc(maxitems * sizeof(Type *));
    count = 0;
  };
template<class Type> TObjectList<Type>::TObjectList (void)
  {
    Name = "Unknown Object list";
    maxitems = 4;
    items = (Type **)malloc(maxitems * sizeof(Type *));
    count = 0;
  };
template<class Type>
TObjectList<Type>::~TObjectList(void)
  {
    Clear();
    if(items) free(items);
  }

template<class Type>
Type *TObjectList<Type>::Append(void)
  {
    if(count >= maxitems) Expand(maxitems + (maxitems >> 1));

    Type  *element = new Type;
    lastadd = count;
    items[count++] = element;
    return (element);
  }


template<class Type> Type *TObjectList<Type>::AppendReuse(void)
  {
    Type  *element = new Type;
    for(unsigned int i = 0; i < count; i++)
      {
      if(!items[i])
        {
        lastadd = i;
        items[i] = element;
        return (element);
        }
      }

    return (Add(element));
  }


template<class Type> Type *TObjectList<Type>::Add(Type *t)
  {
    if(count >= maxitems) Expand(maxitems + (maxitems >> 1));
    lastadd = count;
    items[count++] = t;
    return (t);
  }


template<class Type> void TObjectList<Type>::DeleteItem(Type *t)
  {
    /* find item in list */
    for(unsigned int i = 0; i < count; i++)
      {
      if(items[i] == t)
        {
        delete t;
        items[i] = NULL;
        return;
        }
      }
  }


template<class Type> Type *TObjectList<Type>::AddReuse(Type *element)
  {
    for(unsigned int i = 0; i < count; i++)
      {
      if(!items[i])
        {
        lastadd = i;
        items[i] = element;
        return (element);
        }
      }

    return (Add(element));
  }


template<class Type> void TObjectList<Type>::Clear(void)
  {
    for(unsigned int i = 0; i < count; i++)
      {
      if(items[i])
        {
        delete items[i];
        items[i] = NULL;
        }
      }

    count = 0;
  }


template<class Type> void TObjectList<Type>::Expand(unsigned int newsize)
  {
    Type  **newitems = (Type **)malloc(newsize * sizeof(Type *));

    // copy the data to the new memory block.
    for(unsigned int i = 0; i < count; i++) newitems[i] = items[i];
    free(items);

    // reassign variables.
    items = newitems;
    maxitems = newsize;
  }

template<class Type> TOList<Type>::TOList (void)
  {
    maxitems = 4;
    items = (Type *)malloc(maxitems * sizeof(Type));
    count = 0;
  }
template<class Type> TOList<Type>::TOList (unsigned int in_maxitem)
  {
    maxitems = in_maxitem;
    items = (Type *)malloc(maxitems * sizeof(Type));
    count = 0;
  }
template<class Type> TOList<Type>::~TOList (void)
  {
    if(items) free(items);
  }

template<class Type>
Type *TOList<Type>::GetNext(void)
  {
    if(count >= maxitems) Expand(maxitems * 2);
    return (items + count++);
  }


template<class Type> void TOList<Type>::Expand(unsigned int newsize)
  {
    if(newsize > maxitems)
      {
      items = (Type *)realloc(items, sizeof(Type) * newsize);
      maxitems = newsize;
      }
  }

class stringlist
  {
  private:
    unsigned int maxitems;                      // starts out at 64. Then increases by 64 each time it goes over.
  public:
    unsigned int count;                         // current count of items.
    char **items;                               // items in stringlist.
    stringlist(void);
    ~stringlist(void);
    void          Clear(void);
    unsigned int  Add(char *string);            // only adds if new. otherwise returns ID of previous added string.
    unsigned int  Find(char *string);           // returns -1 if not found.
    void          Expand(unsigned int newsize); // increases the maximum items. Done automatically on add, but can be done in advance
    unsigned int  Append(char *string);         // add to the end of the list.
  };
