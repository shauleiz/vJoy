// CreateVersion.cpp : Defines the entry point for the console application.
// Based on: https://github.com/mnunberg/git-describe-parse by M. Nunberg
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

static void find_delim(char *s, char **res, int *ix)
{
	for (; *ix && s[*ix] != '-'; *ix -= 1);
	if (*ix) {
		s[*ix] = '\0';
		*res = s + *ix + 1;
	}
	else {
		*res = s;
	}
}

int main(int argc, char **argv)
{
	char vbuf[4096] = { 0 };
	char *version, *ncommits, *sha;
	FILE *po;
	int buflen;

	fprintf(stderr, "Starting CreateVersion\n");
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <Outputfile>\n", argv[0]);
		exit(-1);
	}

	/** Get the output */
	po = popen("git describe --long --match  v*", "r");
	if (!po) {
		perror("git describe");
		exit(-2);
	}

	if (!fgets(vbuf, sizeof(vbuf), po)) {
		fprintf(stderr, "git describe closed stream\n");
		exit(-3);
	}
	pclose(po);
	buflen = (int)strlen(vbuf);
	vbuf[buflen - 2] = '\0';
	buflen -= 1;

	// SHA1	 (Remove the leading 'g')
	find_delim(vbuf, &sha, &buflen);
	sha++;

	// Commit #	 - Will be used as build number
	find_delim(vbuf, &ncommits, &buflen);
	
	// Version
	version = vbuf;

	// Break the version number into its components
	unsigned int major=0, mid=0, minor=0;
	int nFields = sscanf_s(version, "v%u.%u.%u",&major, &mid, &minor) ;
	if (nFields != 3) {
		fprintf(stderr, "Version string corrupt\n");
		exit(-4);
	}

	// Open output file
	char * filename = argv[1];
	if (!filename) {
		fprintf(stderr, "Bad output file name\n");
		exit(-5);
	}

	FILE * f;
	errno_t err = fopen_s(&f, filename, "w+");
	if (err || !f)
	{
		fprintf(stderr, "Could not oupen output file name (errno=%d)\n", err);
		exit(-6);
	}

	// Put data into file
	fprintf(f, "#pragma once\n");
	fprintf(f, "#define VER_H_ %u\n", major);
	fprintf(f, "#define VER_M_ %u\n", mid);
	fprintf(f, "#define VER_L_ %u\n", minor);
	fprintf(f, "#define BUILD %s\n", ncommits);
 	fprintf(f, "\n");
	fprintf(f, "#define FILEVER\t\t\t\t\"%u, %u, %u, %s\"\n", major, mid, minor, ncommits);
	fprintf(f, "#define PRODVER_TAG\t\t\t\"%s\"\n", version);
	fprintf(f, "#define PRODVER_SHA1\t\t\"%s\"\n", sha);
 	fprintf(f, "\n");


	return 0;
}

//#include "stdafx.h"
//
//
//int main()
//{
//    return 0;
//}
//
