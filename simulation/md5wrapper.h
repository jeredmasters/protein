/*
 *	This is my wrapper-class to create
 *	a HASHPLZ Hash from a string and a file.
 *
 *	This code is completly free, you 
 *	can copy it, modify it, or do 
 *	what ever you want with it.
 *
 *	Feb. 2005
 *	Benjamin Grüdelbach
 */

//include protection
#ifndef HASHPLZWRAPPER_H
#define HASHPLZWRAPPER_H

//basic includes
#include <string>
#include "md5.h"
//forwards


class md5wrapper
{
	private:
		HASHPLZ *md5;
	
		/*
		 * internal hash function, calling
		 * the basic methods from md5.h
		 */	
		std::string hashit(std::string text);

		/*
		 * converts the numeric giets to
		 * a valid std::string
		 */
		std::string convToString(unsigned char *bytes);
	public:
		//constructor
		md5wrapper();

		//destructor
		~md5wrapper();
		
		/*
		 * creates a HASHPLZ hash from
		 * "text" and returns it as
		 * string
		 */	
		std::string getHashFromString(std::string text);

		/*
		 * creates a HASHPLZ hash from
		 * a file specified in "filename" and 
		 * returns it as string
		 */	
		std::string getHashFromFile(std::string filename);
};


//include protection
#endif

/*
 * EOF
 */
