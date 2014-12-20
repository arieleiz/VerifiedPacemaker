/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

#include <stdio.h>

class CFile
{
public:
	CFile()
	{
		_file = NULL;
	}

	~CFile()
	{
		fclose(_file);
	}

	bool open(const char* name, const char* mode)
	{
		_file = fopen(name, mode);
		return (_file != NULL);
	}

	template<class T>
	bool read(T* t)
	{
		return fread(t, sizeof(T), 1, _file) == 1;
	}

	template<class T>
	bool write(T* t)
	{
		return fwrite(t, sizeof(T), 1, _file) == 1;
	}

	void seek(long offset, int mode)
	{
		fseek(_file, offset, mode);
	}

private:
	FILE* _file;
};

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		printf("usage: mbedcsum <bin>\n");
		return 1;
	}

	CFile file;
	if (!file.open(argv[1], "r+b"))
	{
		printf("Failed opening file: %s\n", argv[1]);
		return 1;
	}

	// Calculate exception vector checksum
	unsigned int i, n, checksum = 0;
	for (i = 0; i < 7; i++)
	{
		if (!file.read(&n))
		{
			printf("Failed reading file: %s\n", argv[1]);
			return 1;
		}

		checksum += n;
	}

	checksum = -checksum;
	printf("Checksum: 0x%08X\n", checksum);

	file.seek(7 * sizeof(int), SEEK_SET);
	file.write(&checksum);

	return 0;
}