#include "Lists.h"
//*******************************************************************
//*******************************************************************
// Stringlists
//*******************************************************************
//*******************************************************************

stringlist::stringlist(void)
  {
  maxitems = 64;
  count = 0;
  items = (char **)calloc(maxitems, sizeof(char *));
  }

// Function
stringlist::~stringlist(void)
  {
  Clear();
  }

// Function
void stringlist::Clear(void)
  {
  for(unsigned int i = 0; i < count; i++)
    {
    if(items[i]) free(items[i]);
    items[i] = NULL;
    }

  count = 0;
  }

// Function
void stringlist::Expand(unsigned int newsize)
  {
  char  **newitems = (char **)calloc(newsize, sizeof(char *));

  // copy the data to the new memory block.
  for(unsigned int i = 0; i < count; i++) newitems[i] = items[i];
  free(items);

  // reassign variables.
  items = newitems;
  maxitems = newsize;
  }

// Function
unsigned int stringlist::Find(char *string)
  {
  for(unsigned int i = 0; i < count; i++)
    {
    if(!strcmp(string, items[i])) return (i);
    }

  return (-1);
  }

// Function
unsigned int stringlist::Add(char *string)
  {
  if(!string) return (-1);

  int result = Find(string);
  if(result > -1) return (result);
  return (Append(string));
  }

// Function
unsigned int stringlist::Append(char *string)
  {
  if(count >= maxitems)
    {
    // expand by 50%
    Expand(maxitems + (maxitems >> 1));
    }

  int   slen = (int)strlen(string);
  char  *str = (char *)malloc(slen + 1);
  strcpy(str, string);
  items[count++] = str;
  return (count - 1);
  }

// Function
TIntList::TIntList(void)
  {
  maxitems = 4;
  items = (unsigned int *)malloc(maxitems * sizeof(int));
  count = 0;
  }

// Function
TIntList::~TIntList(void)
  {
  if(items) free(items);
  }

// Function
void TIntList::Fill(unsigned int fillnum)
  {
  for(unsigned int i = 0; i < count; i++)
    {
    items[i] = fillnum;
    }
  }

// Function
unsigned int TIntList::Add(unsigned int element)
  {
  for(unsigned int i = 0; i < count; i++)
    {
    if(items[i] == element) return (i);
    }

  if(count >= maxitems) Expand(maxitems + (maxitems >> 1));
  items[count++] = element;
  return (count - 1);
  }

// Function
unsigned int TIntList::Append(unsigned int element)
  {
  if(count >= maxitems) Expand(maxitems + (maxitems >> 1));
  items[count++] = element;
  return (count - 1);
  }

// Function
unsigned int TIntList::AddShorts(short p1, short p2)
  {
  int elem = p1 | (p2 << 16);
  return (Add(elem));
  }

// Function
void TIntList::GetShorts(unsigned int index, short &p1, short &p2)
  {
  int elem = items[index];
  p1 = elem & 0xffff;
  p2 = elem >> 16;
  }

// Function
void TIntList::Expand(unsigned int newsize)
  {
  items = (unsigned int *)realloc(items, sizeof(unsigned int) * newsize);
  maxitems = newsize;
  }
