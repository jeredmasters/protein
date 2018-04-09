#include <string>

typedef unsigned char *POINTER;

typedef struct 
{
	unsigned long int state[4];   	      /* state (ABCD) */
	unsigned long int count[2]; 	      /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	      /* input buffer */
} HASHPLZ_CTX;

class HASHPLZ
{

	private:

		void HASHPLZTransform (unsigned long int state[4], unsigned char block[64]);
		void Encode (unsigned char*, unsigned long int*, unsigned int);
		void Decode (unsigned long int*, unsigned char*, unsigned int);
		void HASHPLZ_memcpy (POINTER, POINTER, unsigned int);
		void HASHPLZ_memset (POINTER, int, unsigned int);

	public:
	
		void HASHPLZInit (HASHPLZ_CTX*);
		void HASHPLZUpdate (HASHPLZ_CTX*, unsigned char*, unsigned int);
		void HASHPLZFinal (unsigned char [16], HASHPLZ_CTX*);

	HASHPLZ(){};
};

