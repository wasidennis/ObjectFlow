/**
 * config.h
 *
 * RELEASED WITH THE SWA PART OF LIBSVX.
 * THIS IS A UTILITY FILE THAT IS USED IN THE SWA PROCESSING.  
 *
 * THIS CODE IS RELEASED UNDER THE MIT-LIKE LICENSE INCLUDED
 * WITH THE SOURCE CODE.
 *
 * CONTACT jcorso@buffalo.edu FOR ANY QUESTIONS.
 *
 */


#ifndef CONFIG_H
#define CONFIG_H
#include <iostream>
#include <string>
#include <map>

namespace vpml_graphs
{

class Config
{
    std::map<std::string, std::string> d_map;

public:
    friend std::ostream& operator<< (std::ostream& o, const Config& c);
    Config(std::string file);
    Config();

		void  addFromFile(std::string file);
		void  addFromString(char* str);
    std::string get(std::string key) const;


};


/** Index class is an array class with its entries corresponding to
 *  numbers parsed from the input string. The input string can be mixed with
 *  slashes ('/') and dashes ('-'). Dashes mean taking all the numbers in
 *  the interval (e.g. 3-5 == taking 3, 4, and 5), while slashes are used to
 *  separate different numbers or sequence of numbers,
 *  e.g. (1/4/9-10 ==> Index[0]==1, Index[1]==4, Index[2]==9, Index[3]=10 )
 *
 *  It can be used to specify the layers to be visualized, the frames of a
 *  video, or the slices of a 3D volume.
 *
 *  by A.Y.C.Chen 07/03/2009
 */
class Index
{
protected:
    int numberOfEntries;
    int* table;

public:
    Index() {numberOfEntries=0; table=NULL;};
    ~Index() {delete table;};
    const int operator[]( int idx ) const; //overloading the [] operator just for reading, not writing.
    int getSize() {return numberOfEntries;};
    void parseString(std::string tempstr);
};

} //namespace vpml_graphs
#endif
