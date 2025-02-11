/*
 * Fresh Eye, a program for Russian writing style checking
 * Copyright (C) 1999  OnMind Systems
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: ui.koi8-r,v 1.2 2002/06/29 05:22:14 vadimp Exp $
 */

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "config.h"
#include "fe.h"
#include "ui.h"
#include "util.h"
#include "wrappers.h"
#include "cyrillic.h"

static void cleareol ( int ch )
{
	while ( ch != '\n' )
		if ( (ch = getchar ()) == EOF )
			break;
}

/*
 * General query function with some case folding.
 * Returns a character listed in keys or '\0'
 */
int whatkey ( const char* keys )
{
	int ch = 0;

	while ( 1 )
		switch ( ch = toupper ( getchar () ) ) {
		
			case 'N':
			case '�':
			case '�':
				if ( strchr ( keys, 'N' ) ) {
					ch = 'N';
					goto quit;
				}
				break;

			case 'Y':
			case '�':
			case '�':
				if ( strchr ( keys, 'Y' ) ) {
					ch = 'Y';
					goto quit;
				}
	  			break;
				
			default:
				if ( !strchr ( keys, ch ) )
					ch = 0;
				goto quit;
				
		}
quit:
	cleareol ( ch );
	return ch;
}

int ask ( const char *string, const char* keys )
{
	static int lastkey = 0;
	int key = 0;
	
	if ( string )
		printf (string);
	printf ("? (Yes/No/All/Stop/Context/Help) ");
	switch ( lastkey ) {
	case 'Y':
		printf ( "[Yes] " );
		break;
	case 'N':
		printf ( "[No] " );
		break;
	case 'C':
		printf ( "[Context] " );
		break;
	default:
		printf ( "[Help] " );
	}
	
	return lastkey = 
		((key = whatkey ( keys )) == '\n' ?  lastkey : key);
}

void usage ( void ) {

	printf ( 

	"Usage: "PACKAGE" [options] file...\n"
	"Check Russian writing style.\n\n"
	"-l n,    --context-size    Set size of context to n words\n"
		"                           (default = 15, min = 2)\n"
	"-s n,    --sensitivity     Set sensitivity threshold to n "
		"(default = 600)\n"
	"-c n,    --wordcount-use   Set coefficient of using wordcount "
		"information to n\n"
	"                           (0..100, 0 = off, default = 50)\n"
	"-a,      --silent          Output into log file without queries\n"
	"-d,      --dump-wordcount  Dump wordcount into log file\n"
	"-p,      --proper-names    Do not exclude proper names\n"
	"-r,      --resume          Resume processing, if possible\n"
	"-o path, --output          Use path as log file ('fresheye.log' "
		"is the default)\n"
	"-I cp,   --input-codepage  Set Cyrillic code page of input file to "
		"cp (check\n"
	"                           below for possible values of cp)\n"
#if	0 /* Not implemented yet */
	"-O cp,   --output-codepage Set Cyrillic code page of fe's interface "
		"and log\n"
	"                           file to cp (check below for "
		"possible values of cp)\n"
#endif
	"-h, -?,  --help            Display this help and exit\n"
	"-v,      --version         Display version information and exit\n\n"
	
	"cp parameter (used for code page definition) can be one of the "
		"following:\n\n"
	"koi8-r    -- KOI8-R (default on UNIX-compatible platforms)\n"
	"cp866     -- MS-DOS CP866 (aka 'alternative', default on Win32 "
		"platforms)\n"
	"cp1251    -- Windows CP1251\n"
        "mac       -- Cyrillic encoding used on Apple Macintosh\n"
	"iso8859-5 -- ISO 8859-5\n\n"
	
	"Send suggestions for improvements to Dmitry Kirsanov "
		"<dmitry@kirsanov.com>\n"
	"Report bugs to Vadim Penzin <penzin@attglobal.net>\n"
	"Please make sure there are words 'Fresh Eye' in the Subject: line\n"
	
	);
	
	exit ( 0 );
	
}

void version ( void ) {
	
	printf ( 
	
	PACKAGE" version "VERSION" ("PLATFORM" ["CYR_CP_NAME"])\n"
	"Copyright (C) 1999 OnMind Systems.\n"
	"Fresh Eye is distributed in the hope that it will be useful,\n"
	"but THERE IS ABSOLUTELY NO WARRANTY OF ANY KIND for this software.\n"
	"You may redistribute copies of Fresh Eye\n"
	"under the terms of the GNU General Public License.\n"
	"For more information, see the file named COPYING.\n" 
	
	);
		
	exit ( 0 );
}

int parse_command_line ( int argc, char* argv [] ) {

	static const char* options = "l:s:c:adpro:I:O:hv?";
	int option_index;
	static struct option long_options [] = {
		{ "context-size", 1, NULL, 'l' },
		{ "sensitivity", 1, NULL, 's' },
		{ "wordcount-use", 1, NULL, 'c' },
		{ "silent", 0, NULL, 'a' },
		{ "dump-wordcount", 0, NULL, 'd' },
		{ "proper-names", 0, NULL, 'p' },
		{ "resume", 0, NULL, 'r' },
		{ "output", 1, NULL, 'o' },
		{ "input-codepage", 1, NULL, 'I' },
		{ "output-codepage", 1, NULL, 'O' },
		{ "help", 0, NULL, 'h' },
		{ "version", 0, NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};
	int ch;

	while ( (ch = getopt_long ( argc, argv, options, long_options, 
			&option_index )) != -1 )
		switch ( ch ) {
			case 'l':
				context_size = atol ( optarg );
				break;
				
			case 's':
				sensitivity_threshold =	atol ( optarg );
				break;
				
			case 'c':
				wordcount_use_coefficient = atol ( optarg );
				break;
				
			case 'a':
				quiet_logging = 1;
				break;
			
			case 'd':
				dump_wordcount = 1;
				break;

			case 'p':
				exclude_proper_names = 0;
				break;
				
			case 'r':
				resume_processing = 1;
				break;
			
			case 'o':
				log_path = xstrdup ( optarg ); 
				break;

			case 'I':
				input_codepage = 
					cyr_codepage_by_name ( optarg );
				break;

			case 'O':
				output_codepage =
					cyr_codepage_by_name ( optarg );
				break;
				
			case '?':
			case 'h':
				usage ();
				
			case 'v':
				version ();

		}

	return optind;
}

