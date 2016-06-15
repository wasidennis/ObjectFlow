/**
 * config.cpp
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

#include "config.h"
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

namespace vpml_graphs
{

using namespace std;

///// ------
///// Config class implementation BEGINS
///// ------
Config::Config()
{
}

Config::Config(string file)
{
	addFromFile(file);
}

void
Config::addFromFile(string file)
{
    ifstream ipFile(file.c_str());
    if (!ipFile.is_open()) {
        cerr << "Config file: " << ipFile << " not found"
             << endl;
        throw "FileNotFound";
    }

		// jcorso 2010-02-09
		// changed slightly to handle config files that have multiple lines as well as multiple 
		//  entries on a single line
    string line;
    while(!ipFile.eof())
    {
				while (ipFile.peek() == '\n')
					ipFile.ignore();

        getline(ipFile, line);

				addFromString((char*)line.c_str());

      }

    ipFile.close();
}


void
Config :: addFromString(char* str)
{
	// read key,value
	char* key = strtok(str, " =,");
	while (key != NULL) 
	{
		char* val = strtok(NULL, " =,");
  	if (val != NULL) 
		{
      // save in map
      d_map[string(key)] = string(val);
    }

		// so that we continue :)
		key = strtok(NULL, " =,");
  }
}


string Config::get(string key) const
{
    if (d_map.find(key) != d_map.end())
        return (d_map.find(key))->second ;
    else return "";
}


ostream& operator<< (ostream& o, const Config& c)
{
    o << "Config: [" << endl;
    for (map<string, string>::const_iterator it = c.d_map.begin();
         it != c.d_map.end();
         it++)
    {
        o << it->first << "-->" << it->second << endl;
    }
    o << "]";

    return o;
}

///// ------
///// Config class implementation ENDS
///// ------


///// ------
///// Index class implementation BEGINS
///// ------
const int Index::operator[]( int idx ) const {
    if( idx < 0 || idx >= numberOfEntries )
        exit(-1);

    return table[idx];
}


void Index::parseString(std::string tempstr)
{
    size_t loc=0, subStrLoc=0;

    // if only 1 number is given
    if ((tempstr.size() == 1) && (tempstr.find('-') == string::npos) && (tempstr.find('/') == string::npos))
    {
        numberOfEntries=1;
        table = new int [1];
        table[0] = atoi((tempstr.substr(0, string::npos)).c_str());
    }
    // if numbers are separated by '/'
    else if ((loc=tempstr.find('/')) != string::npos)
    {
        // determine the number of symbols '/' that separate the
        // substrings of numbers
        int locStart = 0;
        string tempSubStr = "";

        // parse string and determine  the total number of entries
        while (loc != string::npos)
        {
            tempSubStr = tempstr.substr(locStart, loc-locStart);
            // if the substring uses the "n-m" format to specify a sequence of frames
            if ((subStrLoc = tempSubStr.find('-')) != string::npos)
            {
                int firstFrame = atoi((tempSubStr.substr(0, subStrLoc)).c_str());
                int lastFrame = atoi((tempSubStr.substr(subStrLoc+1, string::npos)).c_str());
                numberOfEntries += (lastFrame-firstFrame+1);
            }
            // if the substring contains only one frame number
            else
                numberOfEntries++;

            // update the locations for the next iteration
            locStart = loc+1;
            loc=tempstr.find('/', loc+1);
        }
        // parse the final substring behind the final '/'
        tempSubStr = tempstr.substr(locStart, loc-locStart);
        if ((subStrLoc = tempSubStr.find('-')) != string::npos)
        {
            int firstFrame = atoi((tempSubStr.substr(0, subStrLoc)).c_str());
            int lastFrame = atoi((tempSubStr.substr(subStrLoc+1, string::npos)).c_str());
            numberOfEntries += (lastFrame-firstFrame+1);
        }
        else
            numberOfEntries++;

        // build the frame index array
        table = new int [numberOfEntries];

        int i=0;
        locStart = 0;
        loc=tempstr.find('/');
        while (loc != string::npos)
        {
            tempSubStr = tempstr.substr(locStart, loc-locStart);
            // if the substring uses the "n-m" format to specify a sequence of frames
            if ((subStrLoc = tempSubStr.find('-')) != string::npos)
            {
                int firstFrame = atoi((tempSubStr.substr(0, subStrLoc)).c_str());
                int lastFrame = atoi((tempSubStr.substr(subStrLoc+1, string::npos)).c_str());
                for (int j=0; j<=(lastFrame-firstFrame); j++)
                    table[i++] = firstFrame+j;
            }
            // if the substring contains only one frame number
            else
                table[i++] = atoi((tempSubStr.substr(0,string::npos)).c_str());

            locStart = loc+1;
            loc=tempstr.find('/', loc+1);
        }
        // parse the final substring behind the final '/'
        tempSubStr = tempstr.substr(locStart, loc-locStart);
        if ((subStrLoc = tempSubStr.find('-')) != string::npos)
        {
            int firstFrame = atoi((tempSubStr.substr(0, subStrLoc)).c_str());
            int lastFrame = atoi((tempSubStr.substr(subStrLoc+1, string::npos)).c_str());
            for (int j=0; j<=(lastFrame-firstFrame); j++)
                table[i++] = firstFrame+j;
        }
        else
            table[i++] = atoi((tempSubStr.substr(0,string::npos)).c_str());
    }
    // if numbers are only specified as "n-m"
    else if ((loc=tempstr.find('-')) != string::npos)
    {
        int firstFrame = atoi((tempstr.substr(0, loc)).c_str());
        int lastFrame = atoi((tempstr.substr(loc+1, string::npos)).c_str());
        numberOfEntries = lastFrame-firstFrame+1;
        table = new int [numberOfEntries];
        for (int i=0; i<=numberOfEntries-1; i++)
            table[i] = firstFrame+i;
    }
    // if using wrong format
    else
    {
        exit(-1);
    }
}
///// ------
///// Index class implementation ENDS
///// ------



} //namespace vpml_graphs
