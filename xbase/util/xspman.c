#include "xbase/util/xspman.h"

#include <string.h>
#include <stdio.h>

typedef struct XSPManFileNode XSPManFileNode;
struct XSPManFileNode
{
	const char *fname_base;
	XSPManFileNode *next;
};

static struct
{
	bool loaded;  // If true, registration is complete.

	char *frmdat;  // Internal, referenced by ref/objdat.
	char *refdat;  // REF data.
	char *pcgdat;  // PCG pattern data.
	char *pcg_alt;  // Scratchpad for PCG data.
	short pcg_alt_size;  // Size of scratchpad.

	// File lists.
	XSPManFileNode *xobj_list;
	XSPManFileNode *sp_list;

	// Buffer sizes.
	short ref_bytes;
	short frm_bytes;
	short pcg_bytes;

	// Load indices.
	int ref_load_offs;
	int frm_load_offs;
	short pcg_load_offs;
} s_xspman;

int add_ref_data(const char *fname_base)
{
	// Load the REF data into the REF buffer.
	char fname_buffer[256];
	snprintf(fname_buffer, sizeof(fname_buffer), "%s.ref", fname_base);
	FILE *f = fopen(fname_buffer, "rb");
	uint8_t *ref = &s_xspman.refdat[s_xspman.ref_load_offs];
	const int bytes = fread(ref, 1, 8, f);
	fclose(f);

	// Rebase pointer against the FRM data.
	for (short i = 0; i < bytes / 8; i++)
	{
		uint32_t *frm_ptr = (uint32_t *)&ref[(i * 8) + 2];
		*frm_ptr += &s_xspman.frmdat[s_xspman.frm_load_offs];
	}

	return bytes;
}

int add_frm_data(const char *fname_base)
{
	// Load the FRM data into the FRM buffer.
	char fname_buffer[256];
	snprintf(fname_buffer, sizeof(fname_buffer), "%s.frm", fname_base);
	FILE *f = fopen(fname_buffer, "rb");
	fseek(f, 0, SEEK_END);
	const int bytes = ftell(f);
	fseek(f, 0, SEEK_SET);
	fread(&s_xspman.frmdat[s_xspman.frm_load_offs], 1, bytes, f);
	fclose(f);

	// Rebase pattern indices by the current tile count.
	for (short i = 0; i < bytes / 8; i++)
	{
		uint16_t *frm16 = (uint16_t *)&frm[(i * 8) + 4];
		*frm16 += (s_xspman.pcg_load_offs / 128);
	}

	return bytes;
}

int add_pcg_data(const char *fname_base)
{
	// Load the PCG data into the PCG buffer.
	char fname_buffer[256];
	snprintf(fname_buffer, sizeof(fname_buffer), "%s.xsp", fname_base);
	FILE *f = fopen(fname_buffer, "rb");
	fseek(f, 0, SEEK_END);
	const int bytes = ftell(f);
	fseek(f, 0, SEEK_SET);
	fread(&s_xspman.pcgdat[s_xspman.pcg_load_offs], 1, bytes, f);
	fclose(f);

	return bytes;
}

static void free_file_list(XSPManFileNode *current)
{
	XSPManFileNode *next = NULL;
	while (current != NULL)
	{
		if (current->next != null) next = current->next;
		free(current);
		current = next;
	}
}

static void add_xsp_file(const char *fname_base)
{
	if (s_xspman.xobj_list == NULL)
	{
		s_xspman.xobj_list = malloc(sizeof(XSPManFileNode));
		s_xspman.xobj_list->next = NULL;
	}
	else
	{
		XSPManFileNode *current = s_xspman.xobj_list;
		while (current->next != NULL) current = current->next;
		if (current == NULL) return;
		current->next = malloc(sizeof(XSPManFileNode));
		current->fname_base = fname_base;
	}
}

static void add_sp_file(const char *fname_base)
{
	// TODO
}

void xspman_init(void)
{
	memset(s_xspman, 0, sizeof(s_xspman));
}

// Registration
static int get_file_size(const char *fname_base, const char *ext)
{
	char fname_buffer[256];
	snprintf(fname_buffer, sizeof(fname_buffer), "%s.%s", fname_base, ext);
	FILE *f_ref = fopen(fname_buffer, "rb");
	if (!f_ref)
	{
		printf("Couldn't open %s\n", f_ref);
		return -1;
	}

	fseek(f_ref, 0L, SEEK_END);
	const int fsize = (int)ftell(f_ref);
	fclose(f_ref);
	return fsize;
}

short xspman_reg_xsp(const char *fname_base)
{
	if (s_xspman.loaded) return -1;

	const int ref_size = get_file_size(fname_base, "ref");
	if (ref_size < 0) return -1;
	if (ref_size % 8 != 0)
	{
		printf("xspman_reg_xsp: Unusual REF file size for %s\n", fname_base);
		return -1;
	}
	const int ret = s_xspman.ref_bytes / 8;
	s_xspman.ref_bytes += ref_size;

	const int frm_size = get_file_size(fname_base, "frm");
	if (frm_size < 0) return -1;
	if (frm_size % 8 != 0)
	{
		printf("xspman_reg_xsp: Unusual FRM file size for %s\n", fname_base);
		return -1;
	}
	s_xspman.frm_bytes += frm_size;

	const int pcg_size = get_file_size(fname_base, "pcg");
	if (pcg_size < 0) return -1;
	if (pcg_size % 128 != 0)
	{
		printf("xspman_reg_xsp: Unusual PCG file size for %s\n", fname_base);
		return -1;
	}

	s_xspman.pcg_bytes += pcg_size;

	add_xsp_file(fname_base);

	return ret;
}

short xspman_reg_sp(const char *fname_base)
{
	if (s_xspman.loaded) return -1;
	// TODO
	return -1;
}

bool xspman_load(void)
{
	if (s_xspman.loaded) return false;

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
	s_xspman.pcg_alt_size = (s_xspman.pcg_bytes / 128) + 1;
	s_xspman.pcg_alt = malloc(s_xspman.pcg_alt_size);
	if (!s_xspman.pcgdat)
	{
		free(s_xspman.frmdat);
		free(s_xspman.refdat);
		free(s_xspman.pcgdat);
		return false;
	}

	// Load the data into the buffers.
	XSPManFileNode *current = s_xspman.xobj_list;
	while (current != NULL)
	{
		current = current->next;
		const int pcg_offs = s_xspman.pcg_bytes / 128;  // 128 bytes per tile.
		const char *frm_base = &s_xspman.frmdat[frm_load_offs];

		// First, load the REF data, and rebase it against frm_base, which is
		// frmdat[frm_load_offs].
		s_xspman.ref_load_offs += add_ref_data(current->fname_base);
		// Next, load the FRM data into frm_base, add to frm_load_offs the
		// size of the loaded data, and add pcg_load_offs to the pattern field
		// of the FRM data.
		s_xspman.frm_load_offs += add_frm_data(current->fname_base);
		// Finally, load the PCG data, and increment pcg_load_offs by the tile
		// count (size of loaded data / 128).
		s_xspman.pcg_load_offs += add_pcg_data(current->fname_base);
	}

	free_file_list(s_xspman.xobj_list);

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

short xspman_get_pcg_alt_size(void)
{
	return s_xspman.pcg_alt_size;
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

