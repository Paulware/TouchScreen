#include "PSTRStrings.h"
#include <EEPROM.h>


bool PSTRStrings::checkMatch (int which)
{  
  bool match = false;
  CommandStringType * str;
  str = &strings [which]; 
  
  if (str->matched)
  {
    match = true;
    str->matched = 0;
  }  
  return match;
}

void PSTRStrings::clearMatches ()
{
  CommandStringType * str;
  for (int i=0;i<numStrings; i++)
  { 
    str = &strings [i]; 
    str->matched = 0;    
  }  
}

// Print a string from program memory
void PSTRStrings::printPSTR ( PGM_P s) 
{
  char c;
  while ((c=pgm_read_byte(s++)) != 0)
    Serial.print (c);
} 


// This also clears the match flags
void PSTRStrings::showMatches ( )
{
  int count = 0;
  CommandStringType * str;
  for (int i=0;i<numStrings; i++)
  {
    str = &strings[i];
    if (str->matched)
    {
      if (count++ == 0)
        printPSTR ( PSTR ( "Matches:\n" ) );
     
      Serial.print ( count);
      printPSTR ( PSTR ( ")" ) );      
      printString (i);
      printPSTR( PSTR ( "\n" ) );
    }
  }
  if (count==0)
    printPSTR ( PSTR ( "No matches detected\n" ) );

}

// s = pointer to PSTR string
void PSTRStrings::addString ( const prog_char * s)
{ 
  CommandStringType * str;
  prog_char * ptr;
  int index;
  int mallocSize = sizeof(CommandStringType) * numberOfStrings;
  
  if (numStrings == 0) // This is the first one
  {
    strings = (CommandStringType *) malloc ( mallocSize);
	//printPSTR ( PSTR ( "Allocation of " ) );
    //Serial.println ( mallocSize );
  }
  
  if (numStrings == numberOfStrings)
    printPSTR ( PSTR ( "ERR..PSTRStrings list full\n" ) );
  else if (numStrings < numberOfStrings) // There is memory allocated for the string
  { 
    str = &strings [numStrings++];  
  
    // str->len = progLen (str->ptr);
    str->index = 0;
    str->len = 0;
    str->matched = 0;
    str->ptr = (prog_char *)s;
    ptr = (prog_char *)s;
    while (pgm_read_byte (ptr++))
      str->len = str->len + 1;
  } 
}

PSTRStrings::PSTRStrings(int _numberOfStrings )
{
  strings = 0;
  numStrings = 0;
  numberOfStrings = _numberOfStrings;
}

int PSTRStrings::stringLen( int which)
{
  int len = 0;
  CommandStringType * str;
  if ((which < numStrings) && (which > -1))
  {
    str = &strings[which];
    len = str->len;
  }  
  return len;    
}

void PSTRStrings::printString ( int which )
{
  CommandStringType * str = &strings[which];
  char c;
  int len = stringLen ( which );
  
    
  for (int i=0; i<len; i++)
    Serial.print ( charAt ( which, i));
}

char PSTRStrings::charAt ( int i, int j)
{
  CommandStringType * str = &strings[i];
  int len = str->len;
  char ch = 0;
  char * ptr; 
  bool debugThis = false;

  /* 
    if (i>=numStrings)
      printPSTR ( PSTR ( "charAt i>=numStrings\n" ) );
    else if (i < 0) 
      printPSTR ( PSTR ( "charAt i<0\n" ) );
    else if (j >= len)
    {
      printPSTR ( PSTR ( "charAt j:" ));
      Serial.print (j);
      printPSTR ( PSTR ( " >= len:" ));
      Serial.print ( len);
      printPSTR ( PSTR ( " for " ) );
   	  Serial.print (i); 
	  printPSTR ( PSTR ( "): " ) );
      printString ( i );
      printPSTR ( PSTR ( "\n" ) );    	
    }   
    else if (j <0)
      printPSTR ( PSTR ( "charAt j<0 \n" ) );
    else
  */
  str = &strings [i];  
  ptr = str->ptr + j;      
  ch = pgm_read_byte (ptr);
  return ch;
}

void PSTRStrings::show ( int startValue, int stopValue ) 
{ 
  if (startValue < 0)
    startValue = 0;
  if (stopValue >= numStrings)
    stopValue = numStrings;
  
  for (int i=startValue; i<stopValue; i++)
  {
    Serial.print ( i );
	printPSTR ( PSTR ( ":" ) );
	Serial.print ( stringLen (i));	
    printPSTR ( PSTR ( ")" ) );
    printString(i);
    printPSTR ( PSTR ( ".\n" ) );
  }
}

void PSTRStrings::showAll ( )
{
  show (0, numStrings );
}


int PSTRStrings::matchCharPointer ( char * ch ) 
{
  int matched = -1;
  /*
  int len;
  CommandStringType * str;  

  for (int i=0; i<numStrings; i++)  
  {
    str = &strings[i];
    len = str->len;
    for (int j=0; j<len; j++)
    {
      if (*(ch+j) != charAt (i,j))
      {      
        //Serial.print ( c );
        //printPSTR ( PSTR ( " != " ) );
        //Serial.println ( (char) *(ch+j));
        break;
      }  

      if ((j+1) == len)
      {
        str->matched = 1;
        matched = i;
        //printPSTR ( PSTR  ( "Got a match on word ") );
        //Serial.println ( i );
        break;
      }
    }  
    if (matched > -1)
      break;
  }
  */
  return matched;    
}


void PSTRStrings::clearIndexes()
{
  CommandStringType * str;
  for (int i=0; i<numStrings; i++)  
  {
    str = &strings [i]; 
    str->index = 0;
  }  
}    

int PSTRStrings::intersect ( PSTRStrings * targetList, int index )
{
  int found = -1;
  int len;
  char ch;
  char c;
  int count;
  
  // Check all the current strings
  for (int i=0; i<numStrings; i++)
  {    
    len = stringLen(i);
    if (!len)
      printPSTR ( PSTR ( "No match because len== 0\n" ) );
    else if (len <= targetList->stringLen(index))
    {     
      count = 0;
     
      for (int j=0; j<len; j++)
      { 
        ch = charAt (i,j);
        c = targetList->charAt (index,j);
        if (ch == c) 
          count++;
        else
          break;
        if (count == len)
          break;        
      }
       
      if (count == len)
      {
        //printPSTR ( PSTR ( "Got an intersect on " ) );
        //Serial.println ( i );
        found = i;
        break;    
      }
    } 
  }  

  return found;
}

// Check all strings for matches using the supplied ch.
int PSTRStrings::matchString ( char ch, bool doDebug ) 
{
  int matched = -1;
  char c;  
  CommandStringType * str;
  int watching = 1;
    

  for (int i=0; i<numStrings; i++)  
  {
    str = &strings [i]; 
    c = charAt (i,str->index);
    if (ch == c)
    {
      if (doDebug && (i==watching))
      {
        Serial.print ( ch );
        printPSTR ( PSTR  ( " == " ));
        Serial.println ( c );
      }  
      str->index = str->index + 1;
    }  
    else
    {
      if (doDebug && (i==watching))
      {
        if ((int) ch < 33) 
          Serial.print ( (int) ch );
        else
          Serial.print ( ch );
        printPSTR ( PSTR  ( " != ") );
        if ((int)c < 33) 
          Serial.println ( (int) c );
        else          
          Serial.println (c);
      }  
      str->index = 0;
    }  
    if (str->index == str->len)
    {
      matched = i;
      clearIndexes();
      str->matched = 1;
      if (doDebug)
      {
        printPSTR ( PSTR  ( "Matched ") );
        Serial.println ( i );
      }
      break;
    }
  } 
  return matched;    
}

