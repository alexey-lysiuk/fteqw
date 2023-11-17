#include "qcc.h"

#include <stdio.h>

static void *QCC_ReadFile(const char *fname, unsigned char *(*buf_get)(void *ctx, size_t len), void *buf_ctx, size_t *out_size, pbool issourcefile)
//unsigned char *PDECL QCC_ReadFile (const char *fname, void *buffer, int len, size_t *sz)
{
	size_t len;
	FILE *f;
	char *buffer;

	f = fopen(fname, "rb");
	if (!f)
	{
		if (out_size)
			*out_size = 0;
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (buf_get)
		buffer = buf_get(buf_ctx, len+1);
	else
		buffer = malloc(len+1);
	((char*)buffer)[len] = 0;
	if (len != fread(buffer, 1, len, f))
	{
		if (!buf_get)
			free(buffer);
		buffer = NULL;
	}
	fclose(f);

	if (out_size)
		*out_size = len;
	return buffer;
}

static pbool PDECL QCC_WriteFile (const char *name, void *data, int len)
{
	long    length;
	FILE *f;
	f = fopen(name, "wb");
	if (!f)
		return false;
	length = fwrite(data, 1, len, f);
	fclose(f);

	if (length != len)
		return false;

	return true;
}

int main (int argc, const char **argv)
{
	size_t size;
	void *progs = QCC_ReadFile(argv[1], NULL, NULL, &size, false);

	if (!progs)
		return 1;

	void DecompileProgsDat(char *name, void *buf, size_t bufsize);
	DecompileProgsDat("progs.dat", progs, size);

	extern vfile_t *qcc_vfiles;

	while(qcc_vfiles)
	{
		vfile_t *f = qcc_vfiles;
		qcc_vfiles = f->next;

		QCC_WriteFile(f->filename, f->file, f->size);

		free(f->file);
		free(f);
	}

	return 0;
}
