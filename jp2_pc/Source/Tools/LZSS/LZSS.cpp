//
// Lempel, Ziv, Storer, and Szymanski compression/de-compression.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define N			4096	/* size of ring buffer */
#define F			18		/* upper limit for match_length */
#define THRESHOLD	2		/* encode string into position and length
							   if match_length is greater than this */
							   
#define NIL			N		/* index for root of binary search trees */

unsigned long 	textsize = 0,	/* text size counter */
				codesize = 0,	/* code size counter */
				printcount = 0; /* counter for reporting progress every 1K bytes */
				
/* ring buffer of size N, with extra F-1 bytes to facilitate string comparison */
unsigned char text_buf[N + F - 1];     

/* of longest match. These are set by the InsertNode() procedure. */
int	match_position, match_length;

/* left & right children & parents -- These constitute binary search trees. */
int lson[N + 1], rson[N + 257], dad[N + 1];


/* initialize trees */
void InitTree(void)
{
	register int i;

	/* For i = 0 to N - 1, rson[i] and lson[i] will be the right and
	   left children of node i.	 These nodes need not be initialized.
	   Also, dad[i] is the parent of node i.  These are initialized to
	   NIL (= N), which stands for 'not used.'
	   For i = 0 to 255, rson[N + i + 1] is the root of the tree
	   for strings that begin with character i.	 These are initialized
	   to NIL.	Note there are 256 trees. */

	for (i = N + 1; i <= N + 256; i++) rson[i] = NIL;
	for (i = 0; i < N; i++) dad[i] = NIL;
}


/* Inserts string of length F, text_buf[r..r+F-1], into one of the
   trees (text_buf[r]'th tree) and returns the longest-match position
   and length via the global variables match_position and match_length.
   If match_length = F, then removes the old node in favor of the new
   one, because the old one will be deleted sooner.
   Note r plays double role, as tree node and position in buffer. */
void InsertNode(int r)
{
	register int i, p, cmp;
	register unsigned char  *key;
	
	cmp = 1;  key = &text_buf[r];  p = N + 1 + key[0];
	rson[r] = lson[r] = NIL;  match_length = 0;
	for (;;)
	{
		if (cmp >= 0)
		{
			if (rson[p] != NIL)
			{
				p = rson[p];
			}
			else
			{
				rson[p] = r;  dad[r] = p;  return;
			}	
		}
		else
		{
			if (lson[p] != NIL)
			{
				p = lson[p];
			}
			else
			{	
				lson[p] = r;  dad[r] = p;  return;
			}	
		}
		
		for (i = 1; i < F; i++)
		{
			if ((cmp = key[i] - text_buf[p + i]) != 0)	
				break;
		}
			
		if (i > match_length)
		{
			match_position = p;
			if ((match_length = i) >= F)  
				break;
		}
	}
	
	dad[r] = dad[p];  lson[r] = lson[p];  rson[r] = rson[p];
	dad[lson[p]] = r;  dad[rson[p]] = r;
	
	if (rson[dad[p]] == p) 
		rson[dad[p]] = r;
	else
		lson[dad[p]] = r;
		
	dad[p] = NIL;  /* remove p */
}


/* deletes node p from tree */
void DeleteNode(int p)
{
	register int q;

	if (dad[p] == NIL) 
		return;	/* not in tree */
		
	if (rson[p] == NIL)
	{
		q = lson[p];
	}
	else if (lson[p] == NIL)
	{
		q = rson[p];
	}
	else
	{
		q = lson[p];
		
		if (rson[q] != NIL)
		{
			do
			{  
				q = rson[q];	
			}
			while (rson[q] != NIL);
			
			rson[dad[q]] = lson[q];	 dad[lson[q]] = dad[q];
			lson[q] = lson[p];	dad[lson[p]] = q;
		}

		rson[q] = rson[p];	dad[rson[p]] = q;
	}
		
	dad[q] = dad[p];
	
	if (rson[dad[p]] == p) 
		rson[dad[p]] = q;  
	else 
		lson[dad[p]] = q;
	
	dad[p] = NIL;
}


int Compress(FILE *infile, FILE *outfile)
{
	register int i, c, len, last_match_length, code_buf_ptr;
	int r, s;
	unsigned char code_buf[17], mask;

	textsize = 0;
	codesize = 0;
	printcount = 0;

	InitTree();	 /* initialize trees */
	
	code_buf[0] = 0;  
	/* code_buf[1..16] saves eight units of code, and
		code_buf[0] works as eight flags, "1" representing that the unit
		is an unencoded letter (1 byte), "0" a position-and-length pair
		(2 bytes).	Thus, eight units require at most 16 bytes of code. */
	
	code_buf_ptr = mask = 1;
	s = 0;	r = N - F;
	
	/* Clear the buffer with any character that will appear often. */
	for (i = s; i < r; i++) 
		text_buf[i] = '\0';	
							
	/* Read F bytes into the last F bytes of the buffer */
	for (len = 0; len < F && (c = getc(infile)) != EOF; len++)
		text_buf[r + len] = c;
						
	if ((textsize = len) == 0) 
		return(0);	/* text of size zero */
		
	 /* Insert the F strings,
		each of which begins with one or more 'space' characters.  Note
		the order in which these strings are inserted.	This way,
		degenerate trees will be less likely to occur. */		
	for (i = 1; i <= F; i++) 
		InsertNode(r - i);
		
	/* Finally, insert the whole string just read.	The
	   global variables match_length and match_position are set. */		
	InsertNode(r);
				
	do
	{
		/* match_length	may be spuriously long near the end of text. */
		if (match_length > len) 
			match_length = len;
		
		if (match_length <= THRESHOLD)
		{
			match_length = 1;		/* Not long enough match.  Send one byte. */
			code_buf[0] |= mask;  					/* 'send one byte' flag */
			code_buf[code_buf_ptr++] = text_buf[r];	/* Send uncoded. */
		}
		else
		{
			code_buf[code_buf_ptr++] = (unsigned char) match_position;
			
			/* Send position and length pair. Note match_length > THRESHOLD. */
			code_buf[code_buf_ptr++] = (unsigned char)
										(((match_position >> 4) & 0xf0)
						  				| (match_length - (THRESHOLD + 1)));	
		}
		
		if ((mask <<= 1) == 0)
		{
			/* Shift mask left one bit. */
			for (i = 0; i < code_buf_ptr; i++)	/* Send at most 8 units of */
				putc(code_buf[i], outfile);		/* code together */

			codesize += code_buf_ptr;
			code_buf[0] = 0;  code_buf_ptr = mask = 1;
		}
		
		last_match_length = match_length;
		for (i = 0; i < last_match_length && (c = getc(infile)) != EOF; i++)
		{
			DeleteNode(s);			/* Delete old strings and */
			text_buf[s] = c;		/* read new bytes */
			
			/* If the position is near the end of buffer, extend the 
				buffer to make string comparison easier. */
			if (s < F - 1) 
				text_buf[s + N] = c;
					
			/* Since this is a ring buffer, increment the position modulo N. */
			s = (s + 1) & (N - 1);	r = (r + 1) & (N - 1);
			
			InsertNode(r);	/* Register the string in text_buf[r..r+F-1] */
		}

		textsize += i;

#if (0)
		/* Reports progress each time the textsize exceeds multiples of 1024. */
		if (textsize > printcount)
		{
			printf("%12ld\r", textsize);  printcount += 1024*1024;
		}
#endif
	
		while (i++ < last_match_length)
		{
			/* After the end of text, */
			DeleteNode(s);						/* no need to read, but */
			s = (s + 1) & (N - 1);	
			r = (r + 1) & (N - 1);
			if (--len) InsertNode(r);			/* buffer may not be empty. */
		}
		
	}
	while (len > 0);		/* until length of string to be processed is zero */
	
	if (code_buf_ptr > 1)
	{	
		/* Send remaining code. */
		for (i = 0; i < code_buf_ptr; i++) putc(code_buf[i], outfile);
		codesize += code_buf_ptr;
	}

	printf("In : %ld bytes\n", textsize);		
	printf("Out: %ld bytes\n", codesize);
	printf("Out/In: %.3f\n", (double)codesize / textsize);

	return(codesize);	
}


/* Just the reverse of Compress(). */
void Expand(FILE *infile, FILE *outfile)
{
	register int i, k, r, c;
	int j;
	register unsigned int  flags;

	for (i = 0; i < N - F; i++) 
		text_buf[i] = '\0';
	
	r = N - F;	flags = 0;
	
	for (;;)
	{
		if (((flags >>= 1) & 256) == 0)
		{
			if ((c = getc(infile)) == EOF) 
				break;
			flags = c | 0xff00;			/* uses higher byte cleverly */
		}								/* to count eight */
		
		if (flags & 1)
		{
			if ((c = getc(infile)) == EOF) 
				break;
			putc(c, outfile);  text_buf[r++] = c;  r &= (N - 1);
		}
		else
		{
			if ((i = getc(infile)) == EOF) 
				break;
			if ((j = getc(infile)) == EOF) 
				break;
			
			i |= ((j & 0xf0) << 4);	 
			j = (j & 0x0f) + THRESHOLD;
					
			for (k = 0; k <= j; k++)
			{
				c = text_buf[(i + k) & (N - 1)];
				putc(c, outfile);  
				text_buf[r++] = c;  
				r &= (N - 1);
			}
		}
	}
}


#if (1)

#include <sys/types.h>
#include <sys/stat.h>


// De-compression only main.
int main(int argc, char *argv[])
{
	char  *sin, *sout;
	FILE *infile, *outfile;
	unsigned int u_size;
	struct _stat src_stat, dst_stat;

	if (argc != 3 && argc != 4)
	{
		printf("\nLZENCODE [-d] Source Destination\n\n");
		printf("  -d           Update compressed files only if out of date.\n");
		printf("  Source       Source file specification. No wildcards.\n");
		printf("  Destination  Destination file specification. No wildcards.\n");

		return EXIT_FAILURE;
	}
	
	sin = argv[1];
	sout = argv[2];

	if (*argv[1] == '-')
	{
		if (toupper(argv[1][1]) == 'D')
		{
			sin = argv[2];
			sout = argv[3];

			// Check file dates.
			if (_stat(sin, &src_stat) == 0 && _stat(sout, &dst_stat) == 0)
			{
				// Check last modified times.
				if (src_stat.st_mtime <= dst_stat.st_mtime)
				{
					return EXIT_SUCCESS;
				}
			}

		}
		else
		{
			printf("Unrecognized option -> %s\n", argv[1]);
			return EXIT_FAILURE;
		}
	}

	if ((infile  = fopen(sin, "rb")) == NULL)
	{
		printf("Failed to open -> %s\n", sin);
		return EXIT_FAILURE;
	}

	if ((outfile = fopen(sout, "wb")) == NULL)
	{
		printf("Failed to open -> %s\n", sout);
		fclose(infile);
		return EXIT_FAILURE;
	}

	// Get size of file.
	fseek(infile, 0, SEEK_END);
	u_size = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	// Write un-compressed size.
	fwrite(&u_size, sizeof(u_size), 1, outfile);

	Compress(infile, outfile);
		
	fclose(infile);	 
	fclose(outfile);
	
	return EXIT_SUCCESS;
}

#else

// main application.
int main(int argc, char *argv[])
{
	char  *s;
	FILE *infile, *outfile;
	unsigned int u_size;

	if (argc != 4)
	{
		printf("'lzss e file1 file2' encodes file1 into file2.\n"
			   "'lzss d file2 file1' decodes file2 into file1.\n");

		return EXIT_FAILURE;
	}
	
	if ((s = argv[1], s[1] || strpbrk(s, "DEde") == NULL)
	 	|| (s = argv[2], (infile  = fopen(s, "rb")) == NULL)
	 	|| (s = argv[3], (outfile = fopen(s, "wb")) == NULL))
	{
			printf("??? %s\n", s);	return EXIT_FAILURE;
	}
	
	if (toupper(*argv[1]) == 'E') 
	{
		// Get size of file.
		fseek(infile, 0, SEEK_END);
		u_size = ftell(infile);
		fseek(infile, 0, SEEK_SET);

		// Write un-compressed size.
		fwrite(&u_size, sizeof(u_size), 1, outfile);

		Compress(infile, outfile);
	}
	else 
	{
		// Read un-compressed size.
		fread(&u_size, sizeof(u_size), 1, infile);

		Expand(infile, outfile);
	}
		
	fclose(infile);	 
	fclose(outfile);
	
	return EXIT_SUCCESS;
}

#endif
