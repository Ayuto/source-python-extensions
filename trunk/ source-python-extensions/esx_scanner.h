// CSigMngr
/* Credits to David "BAILOPAN" Anderson for writing this!
 * This piece of code is licensed under the GNU GPL V3.
 */

#ifndef _INCLUDE_CSIGMNGR_H
#define _INCLUDE_CSIGMNGR_H

struct signature_t
{
	void *allocBase;
	void *memInBase;
	int  memSize;
	void *offset;
	const char *sig;
	int siglen;
};

class CSigMngr
{
public:
	void *ResolveSig(void *memInBase, const char *pattern, int siglen);
	int ResolvePattern(void *memInBase, const char *pattern, int siglen, int number, ...);
private:
	bool ResolveAddress(signature_t *sigmem);
};

extern CSigMngr g_SigMngr;

#endif //_INCLUDE_CSIGMNGR_H