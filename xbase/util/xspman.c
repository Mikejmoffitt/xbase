#include "xbase/util/xspman.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

// =============================================================================
// Data structures and static data
// =============================================================================

// Definition for "XSB" bundle format from png2xsp.
typedef struct XSBHeader
{
	unsigned short type;  // XSP(0) or SP(1)
	unsigned short ref_count;  // ref bytes / 8;  0 for sp
	unsigned short frm_bytes;  // frm bytes; 0 for sp
	unsigned short pcg_count;  // valid for both xsp and sp
	unsigned short pal[16];
	unsigned int ref_offs;
	unsigned int frm_offs;
	unsigned int pcg_offs;
} XSBHeader;

enum
{
	XSPMAN_FILE_XSP,
	XSPMAN_FILE_SP,
	XSPMAN_FILE_BUNDLE,
};

typedef struct XSPManFileNode XSPManFileNode;
struct XSPManFileNode
{
	char fname_base[256];
	XSPManFileNode *next;
	unsigned short ref_bytes;
	unsigned short frm_bytes;
	unsigned int pcg_bytes;
	short filetype;
};

static struct
{
	bool loaded;  // If true, registration is complete.

	char *frmdat;  // Internal, referenced by ref/objdat.
	char *refdat;  // REF data.
	char *pcgdat;  // PCG pattern data.
	char *pcg_alt;  // Scratchpad for PCG data.
	short pcg_alt_bytes;  // Size of scratchpad.

	// File lists.
	XSPManFileNode *file_list;

	// Buffer sizes.
	unsigned short ref_bytes;
	unsigned short frm_bytes;
	unsigned int pcg_bytes;

	// Load indices.
	int ref_load_offs;
	int frm_load_offs;
	int pcg_load_offs;
} s_xspman;

// =============================================================================
// Init
// =============================================================================

void xspman_init(void)
{
	memset(&s_xspman, 0, sizeof(s_xspman));
}

// =============================================================================
// File loaders
// =============================================================================

FILE *file_open_sub(const char *fname_base, const char *ext, int filetype)
{
	if (filetype == XSPMAN_FILE_BUNDLE) return fopen(fname_base, "rb");
	char fname_buffer[256];
	snprintf(fname_buffer, sizeof(fname_buffer), "%s.%s", fname_base, ext);
	return fopen(fname_buffer, "rb");
}

int add_ref_data(const char *fname_base, int bytes, int filetype)
{
	// Load the REF data into the REF buffer.
	FILE *f = file_open_sub(fname_base, "ref", filetype);
	if (!f) return 0;
	char *ref = &s_xspman.refdat[s_xspman.ref_load_offs];
	if (filetype == XSPMAN_FILE_BUNDLE)
	{
		fseek(f, offsetof(XSBHeader, ref_offs), SEEK_SET);
		unsigned int offs;
		fread(&offs, sizeof(unsigned int), 1, f);
		printf("@$%X;", offs);
		fseek(f, offs, SEEK_SET);
	}
	bytes = fread(ref, 1, bytes, f);
	fclose(f);

	// The REF data as it comes contains offsets from the start of the FRM data
	// block. We have to rebase it against the start of the FRM block in RAM.
	const unsigned int frm_base_addr =
	    (unsigned int)&s_xspman.frmdat[s_xspman.frm_load_offs];
	for (short i = 0; i < bytes / 8; i++)
	{
		// Starting from the second byte is the FRM data offset.
		const short ref_offs = (i * 8) + 2;
		// In order to add the FRM_DAT base address, we will access it directly
		// by casting as an unsigned int.
		unsigned int *frm_ptr = (unsigned int *)(&ref[ref_offs]);
		*frm_ptr += frm_base_addr;
	}

	return bytes;
}

int add_frm_data(const char *fname_base, int bytes, int filetype)
{
	// Load the FRM data into the FRM buffer.
	FILE *f = file_open_sub(fname_base, "frm", filetype);
	if (!f) return 0;
	char *frm = &s_xspman.frmdat[s_xspman.frm_load_offs];
	if (filetype == XSPMAN_FILE_BUNDLE)
	{
		fseek(f, offsetof(XSBHeader, frm_offs), SEEK_SET);
		unsigned int offs;
		fread(&offs, sizeof(unsigned int), 1, f);
		printf("@$%X;", offs);
		fseek(f, offs, SEEK_SET);
	}
	bytes = fread(frm, 1, bytes, f);
	fclose(f);

	// The FRM data as it comes contains pattern numbers within the PCG data
	// block. We must increment the pattern code to match the loaded number.
	for (short i = 0; i < bytes / 8; i++)
	{
		const short frm_offs = (i * 8) + 4;
		short *pt = (short *)&frm[frm_offs];
		*pt += (s_xspman.pcg_load_offs / 128);
	}

	return bytes;
}

int add_pcg_data(const char *fname_base, int bytes, int filetype)
{
	// Load the PCG data into the PCG buffer.
	FILE *f = file_open_sub(fname_base,
	                        filetype == XSPMAN_FILE_XSP ? "xsp" : "sp",
	                        filetype);
	if (!f) return 0;
	if (filetype == XSPMAN_FILE_BUNDLE)
	{
		fseek(f, offsetof(XSBHeader, pcg_offs), SEEK_SET);
		unsigned int offs;
		fread(&offs, sizeof(unsigned int), 1, f);
		printf("@$%X;", offs);
		fseek(f, offs, SEEK_SET);
	}
	bytes = fread(&s_xspman.pcgdat[s_xspman.pcg_load_offs], 1, bytes, f);
	fclose(f);

	return bytes;
}

// =============================================================================
// File list creation
// =============================================================================

static void free_file_list(XSPManFileNode *current)
{
	XSPManFileNode *next = NULL;
	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}
}

static void add_file(const char *fname_base,
                     int ref_bytes, int frm_bytes, int pcg_bytes, int filetype)
{
	XSPManFileNode *newnode = NULL;
	if (s_xspman.file_list == NULL)
	{
		s_xspman.file_list = malloc(sizeof(XSPManFileNode));
		newnode = s_xspman.file_list;
	}
	else
	{
		XSPManFileNode *current = s_xspman.file_list;
		while (current->next != NULL)
		{
			current = current->next;
		}
		current->next = malloc(sizeof(XSPManFileNode));
		newnode = current->next;
	}

	strncpy(newnode->fname_base, fname_base, sizeof(newnode->fname_base)-1);
	newnode->next = NULL;
	newnode->filetype = filetype;
	newnode->ref_bytes = ref_bytes;
	newnode->frm_bytes = frm_bytes;
	newnode->pcg_bytes = pcg_bytes;
	printf("%s:\t REF $%06X\tFRM $%06X\tPCG $%06X\n", newnode->fname_base,
	       newnode->ref_bytes, newnode->frm_bytes, newnode->pcg_bytes);
}

// =============================================================================
// Registration
// =============================================================================

static int get_file_bytes(const char *fname_base, const char *ext)
{
	char fname_buffer[256];
	snprintf(fname_buffer, sizeof(fname_buffer), "%s.%s", fname_base, ext);
	FILE *f_ref = fopen(fname_buffer, "rb");
	if (!f_ref)
	{
		printf("Couldn't open %s\n", fname_buffer);
		return -1;
	}

	fseek(f_ref, 0L, SEEK_END);
	const int fsize = (int)ftell(f_ref);
	fclose(f_ref);
	return fsize;
}

short xspman_reg_bundle(const char *fname, unsigned short *pal)
{
	if (s_xspman.loaded) return -1;
	printf("[xspman_reg_bundle] %s\n", fname);
	FILE *f = fopen(fname, "rb");
	if (!f)
	{
		printf("Error opening file.\n");
		return -1;
	}
	XSBHeader header;
	fread(&header, 1, sizeof(header), f);
	fclose(f);

	if (pal)
	{
		memcpy(pal, header.pal, sizeof(header.pal));
	}

	short ret = -1;

	if (header.type == 0)  // XSP
	{
		ret = s_xspman.ref_bytes / 8;  // XOBJ no.
	}
	else if (header.type == 1)  // sp
	{
		ret = s_xspman.pcg_bytes / 128;  // PT no.
	}
	else
	{
		printf("Error: type %d\n", header.type);
		return ret;
	}
	s_xspman.ref_bytes += (header.ref_count * 8);
	s_xspman.frm_bytes += header.frm_bytes;
	s_xspman.pcg_bytes += header.pcg_count * 128;
	add_file(fname, header.ref_count * 8, header.frm_bytes,
	         header.pcg_count * 128, XSPMAN_FILE_BUNDLE);
	return ret;
}

short xspman_reg_xsp(const char *fname_base)
{
	if (s_xspman.loaded) return -1;

	const int ref_bytes = get_file_bytes(fname_base, "ref");
	if (ref_bytes < 0) return -1;
	if (ref_bytes % 8 != 0)
	{
		printf("xspman_reg_xsp: Unusual REF file size for %s\n", fname_base);
		return -1;
	}
	const int ret = s_xspman.ref_bytes / 8;  // This gives us the XOBJ no.
	s_xspman.ref_bytes += ref_bytes;

	const int frm_bytes = get_file_bytes(fname_base, "frm");
	if (frm_bytes < 0) return -1;
	if (frm_bytes % 8 != 0)
	{
		printf("xspman_reg_xsp: Unusual FRM file size for %s\n", fname_base);
		return -1;
	}
	s_xspman.frm_bytes += frm_bytes;

	const int pcg_bytes = get_file_bytes(fname_base, "xsp");
	if (pcg_bytes < 0) return -1;
	if (pcg_bytes % 128 != 0)
	{
		printf("xspman_reg_xsp: Unusual PCG file size for %s\n", fname_base);
		return -1;
	}
	s_xspman.pcg_bytes += pcg_bytes;

	add_file(fname_base, ref_bytes, frm_bytes, pcg_bytes, XSPMAN_FILE_XSP);

	return ret;
}

short xspman_reg_sp(const char *fname_base)
{
	if (s_xspman.loaded) return -1;

	const int pcg_bytes = get_file_bytes(fname_base, "sp");
	if (pcg_bytes < 0) return -1;
	if (pcg_bytes % 128 != 0)
	{
		printf("xspman_reg_sp: Unusual PCG file size for %s\n", fname_base);
		return -1;
	}

	const short ret = s_xspman.pcg_bytes / 128;

	s_xspman.pcg_bytes += pcg_bytes;

	add_file(fname_base, 0, 0, pcg_bytes, XSPMAN_FILE_SP);
	return ret;
}

bool xspman_load(void)
{
	if (s_xspman.loaded) return false;

	printf("[xspman_load] Buffer summary:\n");
	printf("  FRM: %d bytes\n", s_xspman.frm_bytes);
	printf("  REF: %d bytes\n", s_xspman.ref_bytes);
	printf("  PCG: %d bytes\n", s_xspman.pcg_bytes);

	// Create buffers based on surveyed data from registration.
	s_xspman.frmdat = malloc(s_xspman.frm_bytes);
	if (!s_xspman.frmdat) return false;
	s_xspman.refdat = malloc(s_xspman.ref_bytes);
	if (!s_xspman.refdat)
	{
		free(s_xspman.frmdat);
		return false;
	}
	s_xspman.pcgdat = malloc(s_xspman.pcg_bytes);
	if (!s_xspman.pcgdat)
	{
		free(s_xspman.frmdat);
		free(s_xspman.refdat);
		return false;
	}
	s_xspman.pcg_alt_bytes = (s_xspman.pcg_bytes / 128) + 1;
	s_xspman.pcg_alt = malloc(s_xspman.pcg_alt_bytes);
	if (!s_xspman.pcgdat)
	{
		free(s_xspman.frmdat);
		free(s_xspman.refdat);
		free(s_xspman.pcgdat);
		return false;
	}

	// Load the data into the buffers.
	XSPManFileNode *current = s_xspman.file_list;
	while (current != NULL)
	{
		printf("[xspman] %s:\n", current->fname_base);
		if (current->filetype != XSPMAN_FILE_SP)
		{
			// Loading an XOBJ file requires multiple steps. The complication,
			// at least when compared to a plain PCG tile, stems from the fact
			// that the data structures must be rebased.
			//
			// REF_DAT: Composite sprite reference data structure.
			//   * Dictates number of composite frame structs (FRM_DAT count)
			//   * Has a pointer to start of FRM_DAT (rebased against FRM_DAT)
			//
			// FRM_DAT: Composite sprite frame data structure (describes a tile)
			//   * Has coordinate data and draw metadata (reflection, color)
			//   * Has a PCG pattern no. (rebased against PCG number base)

			// First, load the REF data, and rebase it against frm_base, which is
			// frmdat[frm_load_offs].
			printf(" REF");
			s_xspman.ref_load_offs += add_ref_data(current->fname_base,
			                                       current->ref_bytes,
			                                       current->filetype);
			// Next, load the FRM data into frm_base, add to frm_load_offs the
			// size of the loaded data, and add pcg_load_offs/128 to the pattern
			// field of the FRM data.
			printf(" FRM");
			s_xspman.frm_load_offs += add_frm_data(current->fname_base,
			                                       current->frm_bytes,
			                                       current->filetype);
		}
		// Finally, load the PCG data, and increment pcg_load_offs by the number
		// of bytes loaded.
		printf(" PCG");
		s_xspman.pcg_load_offs += add_pcg_data(current->fname_base,
		                                       current->pcg_bytes,
		                                       current->filetype);
		printf(" OK!\n");
		// Visit the next.
		current = current->next;
	}

	free_file_list(s_xspman.file_list);

	s_xspman.loaded = true;
	return true;
}

const void *xspman_get_pcgdat_ptr(void)
{
	return s_xspman.pcgdat;
}

char *xspman_get_pcg_alt_ptr(void)
{
	return s_xspman.pcg_alt;
}

short xspman_get_pcg_alt_bytes(void)
{
	return s_xspman.pcg_alt_bytes;
}

const void *xspman_get_objdat_ptr(void)
{
	return s_xspman.refdat;
}

void xspman_shutdown(void)
{
	if (!s_xspman.loaded) return;

	free(s_xspman.frmdat);
	free(s_xspman.refdat);
	free(s_xspman.pcgdat);
	free(s_xspman.pcg_alt);
	xspman_init();
}

