#include "NiftiImage.h"

const string &NiftiImage::dtypeName() const { return _datatype.name; }
/*
* Map for string representations of NIfTI unit codes.
*
*\sa NIFTI1_UNITS group in nifti1.h
*/
const string &NiftiImage::spaceUnits() const
{
    static const StringMap Units
    {
        { NIFTI_UNITS_METER,  "m" },
        { NIFTI_UNITS_MM,     "mm" },
        { NIFTI_UNITS_MICRON, "um" }
    };

    static string unknown("Unknown space units code");
    StringMap::const_iterator it = Units.find(xyz_units);
    if (it == Units.end())
        return unknown;
    else
        return it->second;
}
const string &NiftiImage::timeUnits() const
{
    static const StringMap Units
    {
        { NIFTI_UNITS_SEC,    "s" },
        { NIFTI_UNITS_MSEC,   "ms" },
        { NIFTI_UNITS_USEC,   "us" },
        { NIFTI_UNITS_HZ,     "Hz" },
        { NIFTI_UNITS_PPM,    "ppm" },
        { NIFTI_UNITS_RADS,   "rad/s" }
    };
    static string unknown("Unknown time units code");
    StringMap::const_iterator it = Units.find(time_units);
    if (it == Units.end())
        return unknown;
    else
        return it->second;
}

/*
* Map for string representations of NIfTI intent types.
*
*\sa NIFTI1_INTENT_CODES group in nifti1.h
*/
const string &NiftiImage::intentName() const
{
    const StringMap Intents
    {
        { NIFTI_INTENT_CORREL,     "Correlation statistic" },
        { NIFTI_INTENT_TTEST,      "T-statistic" },
        { NIFTI_INTENT_FTEST,      "F-statistic" },
        { NIFTI_INTENT_ZSCORE,     "Z-score" },
        { NIFTI_INTENT_CHISQ,      "Chi-squared distribution" },
        { NIFTI_INTENT_BETA,       "Beta distribution" },
        { NIFTI_INTENT_BINOM,      "Binomial distribution" },
        { NIFTI_INTENT_GAMMA,      "Gamma distribution" },
        { NIFTI_INTENT_POISSON,    "Poisson distribution" },
        { NIFTI_INTENT_NORMAL,     "Normal distribution" },
        { NIFTI_INTENT_FTEST_NONC, "F-statistic noncentral" },
        { NIFTI_INTENT_CHISQ_NONC, "Chi-squared noncentral" },
        { NIFTI_INTENT_LOGISTIC,   "Logistic distribution" },
        { NIFTI_INTENT_LAPLACE,    "Laplace distribution" },
        { NIFTI_INTENT_UNIFORM,    "Uniform distribition" },
        { NIFTI_INTENT_TTEST_NONC, "T-statistic noncentral" },
        { NIFTI_INTENT_WEIBULL,    "Weibull distribution" },
        { NIFTI_INTENT_CHI,        "Chi distribution" },
        { NIFTI_INTENT_INVGAUSS,   "Inverse Gaussian distribution" },
        { NIFTI_INTENT_EXTVAL,     "Extreme Value distribution" },
        { NIFTI_INTENT_PVAL,       "P-value" },

        { NIFTI_INTENT_LOGPVAL,    "Log P-value" },
        { NIFTI_INTENT_LOG10PVAL,  "Log10 P-value" },

        { NIFTI_INTENT_ESTIMATE,   "Estimate" },
        { NIFTI_INTENT_LABEL,      "Label index" },
        { NIFTI_INTENT_NEURONAME,  "NeuroNames index" },
        { NIFTI_INTENT_GENMATRIX,  "General matrix" },
        { NIFTI_INTENT_SYMMATRIX,  "Symmetric matrix" },
        { NIFTI_INTENT_DISPVECT,   "Displacement vector" },
        { NIFTI_INTENT_VECTOR,     "Vector" },
        { NIFTI_INTENT_POINTSET,   "Pointset" },
        { NIFTI_INTENT_TRIANGLE,   "Triangle" },
        { NIFTI_INTENT_QUATERNION, "Quaternion" },

        { NIFTI_INTENT_DIMLESS,    "Dimensionless number" }
    };
    static string unknown("Unknown intent code");
    StringMap::const_iterator it = Intents.find(intent_code);
    if (it == Intents.end())
        return unknown;
    else
        return it->second;
}

/*
* Map for string representations of NIfTI transform codes.
*
*\sa NIFTI1_XFORM_CODES group in nifti1.h
*/
const string &NiftiImage::transformName() const
{
    static const StringMap Transforms
    {
        { NIFTI_XFORM_SCANNER_ANAT, "Scanner Anat" },
        { NIFTI_XFORM_ALIGNED_ANAT, "Aligned Anat" },
        { NIFTI_XFORM_TALAIRACH,    "Talairach" },
        { NIFTI_XFORM_MNI_152,      "MNI_152" }
    };
    static string unknown("Unknown transform code");
    StringMap::const_iterator it = Transforms.find(sform_code);
    if (it == Transforms.end())
        return unknown;
    else
        return it->second;
}

/*
* Map for string representations of NIfTI slice_codes
*
*\sa NIFTI1_SLICE_ORDER group in nifti1.h
*/
const string &NiftiImage::sliceName() const
{
    static const StringMap SliceOrders
    {
        { NIFTI_SLICE_SEQ_INC,  "sequential_increasing" },
        { NIFTI_SLICE_SEQ_DEC,  "sequential_decreasing" },
        { NIFTI_SLICE_ALT_INC,  "alternating_increasing" },
        { NIFTI_SLICE_ALT_DEC,  "alternating_decreasing" },
        { NIFTI_SLICE_ALT_INC2, "alternating_increasing_2" },
        { NIFTI_SLICE_ALT_DEC2, "alternating_decreasing_2" }
    };
    static string unknown("Unknown slice order code");
    StringMap::const_iterator it = SliceOrders.find(slice_code);
    if (it == SliceOrders.end())
        return unknown;
    else
        return it->second;
}

/*---------------------------------------------------------------------------*/
/* prototypes for internal functions - not part of exported library          */

/* extension routines */
/*static int  nifti_read_extensions( nifti_image *nim, znzFile fp, int remain );
static int  nifti_read_next_extension( nifti1_extension * nex, nifti_image *nim,                                       int remain, znzFile fp );
static int  nifti_check_extension(nifti_image *nim, int size,int code, int rem);
static int  nifti_add_exten_to_list(nifti1_extension *  new_ext,
nifti1_extension ** list, long new_length);
static int  nifti_fill_extension(nifti1_extension * ext, const char * data,
int len, int ecode);*/

/* for nifti_read_collapsed_image: */
/*static int  rci_read_data(nifti_image *nim, int *pivots, int *prods, int nprods,
const int dims[], char *data, znzFile fp, size_t base_offset);
static int  rci_alloc_mem(void ** data, int prods[8], int nprods, int nbyper );
static int  make_pivot_list(nifti_image * nim, const int dims[], int pivots[],
int prods[], int * nprods );*/
/*----------------------------------------------------------------------*/

/*! Swap siz bytes at a time from the given array of n sets of bytes
*
*  Declared void * so that the fields from the headers can be passed through
*  without casting.
*/
void NiftiImage::SwapBytes(size_t n, int size, void *bytes)
{
    size_t i;
    char *cp0 = (char *)bytes, *cp1, *cp2;
    char swap;

    for (i = 0; i < n; i++) {
        cp1 = cp0;
        cp2 = cp0 + (size - 1);
        while (cp2 > cp1)
        {
            swap = *cp1; *cp1 = *cp2; *cp2 = swap;
            cp1++; cp2--;
        }
        cp0 += size;
    }
}

/*
*  Byte swap the individual fields of a NIFTI-1 header struct.
*/
void NiftiImage::SwapNiftiHeader(struct nifti_1_header *h)
{
    SwapBytes(1, 4, &h->sizeof_hdr);
    SwapBytes(1, 4, &h->extents);
    SwapBytes(1, 2, &h->session_error);

    SwapBytes(8, 2, h->dim);
    SwapBytes(1, 4, &h->intent_p1);
    SwapBytes(1, 4, &h->intent_p2);
    SwapBytes(1, 4, &h->intent_p3);

    SwapBytes(1, 2, &h->intent_code);
    SwapBytes(1, 2, &h->datatype);
    SwapBytes(1, 2, &h->bitpix);
    SwapBytes(1, 2, &h->slice_start);

    SwapBytes(8, 4, h->pixdim);

    SwapBytes(1, 4, &h->vox_offset);
    SwapBytes(1, 4, &h->scl_slope);
    SwapBytes(1, 4, &h->scl_inter);
    SwapBytes(1, 2, &h->slice_end);

    SwapBytes(1, 4, &h->cal_max);
    SwapBytes(1, 4, &h->cal_min);
    SwapBytes(1, 4, &h->slice_duration);
    SwapBytes(1, 4, &h->toffset);
    SwapBytes(1, 4, &h->glmax);
    SwapBytes(1, 4, &h->glmin);

    SwapBytes(1, 2, &h->qform_code);
    SwapBytes(1, 2, &h->sform_code);

    SwapBytes(1, 4, &h->quatern_b);
    SwapBytes(1, 4, &h->quatern_c);
    SwapBytes(1, 4, &h->quatern_d);
    SwapBytes(1, 4, &h->qoffset_x);
    SwapBytes(1, 4, &h->qoffset_y);
    SwapBytes(1, 4, &h->qoffset_z);

    SwapBytes(4, 4, h->srow_x);
    SwapBytes(4, 4, h->srow_y);
    SwapBytes(4, 4, h->srow_z);

    return;
}

/*
*! Byte swap as an ANALYZE 7.5 header
*/
void NiftiImage::SwapAnalyzeHeader(nifti_analyze75 * h)
{
    SwapBytes(1, 4, &h->sizeof_hdr);
    SwapBytes(1, 4, &h->extents);
    SwapBytes(1, 2, &h->session_error);

    SwapBytes(8, 2, h->dim);
    SwapBytes(1, 2, &h->unused8);
    SwapBytes(1, 2, &h->unused9);
    SwapBytes(1, 2, &h->unused10);
    SwapBytes(1, 2, &h->unused11);
    SwapBytes(1, 2, &h->unused12);
    SwapBytes(1, 2, &h->unused13);
    SwapBytes(1, 2, &h->unused14);

    SwapBytes(1, 2, &h->datatype);
    SwapBytes(1, 2, &h->bitpix);
    SwapBytes(1, 2, &h->dim_un0);

    SwapBytes(8, 4, h->pixdim);

    SwapBytes(1, 4, &h->vox_offset);
    SwapBytes(1, 4, &h->funused1);
    SwapBytes(1, 4, &h->funused2);
    SwapBytes(1, 4, &h->funused3);

    SwapBytes(1, 4, &h->cal_max);
    SwapBytes(1, 4, &h->cal_min);
    SwapBytes(1, 4, &h->compressed);
    SwapBytes(1, 4, &h->verified);

    SwapBytes(1, 4, &h->glmax);
    SwapBytes(1, 4, &h->glmin);

    SwapBytes(1, 4, &h->views);
    SwapBytes(1, 4, &h->vols_added);
    SwapBytes(1, 4, &h->start_field);
    SwapBytes(1, 4, &h->field_skip);

    SwapBytes(1, 4, &h->omax);
    SwapBytes(1, 4, &h->omin);
    SwapBytes(1, 4, &h->smax);
    SwapBytes(1, 4, &h->smin);
}

/*----------------------------------------------------------------------*/
/*! display the contents of the nifti_1_header (send to stdout)

\param info if non-NULL, print this character string
\param hp   pointer to nifti_1_header
*//*--------------------------------------------------------------------*/
int disp_nifti_1_header(const char * info, const nifti_1_header * hp)
{
    int c;

    fputs("-------------------------------------------------------\n", stdout);
    if (info)  fputs(info, stdout);
    if (!hp) { fputs(" ** no nifti_1_header to display!\n", stdout); return 1; }

    fprintf(stdout, " nifti_1_header :\n"
        "    sizeof_hdr     = %d\n"
        "    data_type[10]  = %s\n"
        "    db_name[18]    = %s\n",
        hp->sizeof_hdr, hp->data_type, hp->db_name);
    fprintf(stdout, "    extents        = %d\n"
        "    session_error  = %d\n"
        "    regular        = 0x%x\n"
        "    dim_info       = 0x%x\n",
        hp->extents, hp->session_error, hp->regular, hp->dim_info);
    fprintf(stdout, "    dim[8]         =");
    for (c = 0; c < 8; c++) fprintf(stdout, " %d", hp->dim[c]);
    fprintf(stdout, "\n"
        "    intent_p1      = %f\n"
        "    intent_p2      = %f\n"
        "    intent_p3      = %f\n"
        "    intent_code    = %d\n"
        "    datatype       = %d\n"
        "    bitpix         = %d\n"
        "    slice_start    = %d\n"
        "    pixdim[8]      =",
        hp->intent_p1, hp->intent_p2, hp->intent_p3, hp->intent_code,
        hp->datatype, hp->bitpix, hp->slice_start);
    /* break pixdim over 2 lines */
    for (c = 0; c < 4; c++) fprintf(stdout, " %f", hp->pixdim[c]);
    fprintf(stdout, "\n                    ");
    for (c = 4; c < 8; c++) fprintf(stdout, " %f", hp->pixdim[c]);
    fprintf(stdout, "\n"
        "    vox_offset     = %f\n"
        "    scl_slope      = %f\n"
        "    scl_inter      = %f\n"
        "    slice_end      = %d\n"
        "    slice_code     = %d\n"
        "    xyzt_units     = 0x%x\n"
        "    cal_max        = %f\n"
        "    cal_min        = %f\n"
        "    slice_duration = %f\n"
        "    toffset        = %f\n"
        "    glmax          = %d\n"
        "    glmin          = %d\n",
        hp->vox_offset, hp->scl_slope, hp->scl_inter, hp->slice_end,
        hp->slice_code, hp->xyzt_units, hp->cal_max, hp->cal_min,
        hp->slice_duration, hp->toffset, hp->glmax, hp->glmin);
    fprintf(stdout,
        "    descrip        = '%.80s'\n"
        "    aux_file       = '%.24s'\n"
        "    qform_code     = %d\n"
        "    sform_code     = %d\n"
        "    quatern_b      = %f\n"
        "    quatern_c      = %f\n"
        "    quatern_d      = %f\n"
        "    qoffset_x      = %f\n"
        "    qoffset_y      = %f\n"
        "    qoffset_z      = %f\n"
        "    srow_x[4]      = %f, %f, %f, %f\n"
        "    srow_y[4]      = %f, %f, %f, %f\n"
        "    srow_z[4]      = %f, %f, %f, %f\n"
        "    intent_name    = '%-.16s'\n"
        "    magic          = '%-.4s'\n",
        hp->descrip, hp->aux_file, hp->qform_code, hp->sform_code,
        hp->quatern_b, hp->quatern_c, hp->quatern_d,
        hp->qoffset_x, hp->qoffset_y, hp->qoffset_z,
        hp->srow_x[0], hp->srow_x[1], hp->srow_x[2], hp->srow_x[3],
        hp->srow_y[0], hp->srow_y[1], hp->srow_y[2], hp->srow_y[3],
        hp->srow_z[0], hp->srow_z[1], hp->srow_z[2], hp->srow_z[3],
        hp->intent_name, hp->magic);
    fputs("-------------------------------------------------------\n", stdout);
    fflush(stdout);

    return 0;
}
/*----------------------------------------------------------------------
* Read the extensions into the nifti_image struct   08 Dec 2004 [rickr]
*
* This function is called just after the header struct is read in, and
* it is assumed the file pointer has not moved.  The value in remain
* is assumed to be accurate, reflecting the bytes of space for potential
* extensions.
*
* return the number of extensions read in, or < 0 on error
*----------------------------------------------------------------------*/
//static int nifti_read_extensions( nifti_image *nim, znzFile fp, int remain )
//{
//	nifti1_extender    extdr;      /* defines extension existence  */
//	nifti1_extension   extn;       /* single extension to process  */
//	nifti1_extension * Elist;      /* list of processed extensions */
//	long               posn, count;
//
//	if( !nim || znz_isnull(fp) ) {
//		if( g_opts.debug > 0 )
//			fprintf(stderr,"** nifti_read_extensions: bad inputs (%p,%p)\n",
//					(void *)nim, (void *)fp);
//		return -1;
//	}
//
//	posn = znztell(fp);
//
//	if( (posn != sizeof(nifti_1_header)) &&
//       (nim->nifti_type != NIFTI_FTYPE_ASCII) )
//		fprintf(stderr,"** WARNING: posn not header size (%ld, %d)\n",
//				posn, (int)sizeof(nifti_1_header));
//
//	if( g_opts.debug > 2 )
//		fprintf(stderr,"-d nre: posn = %ld, offset = %d, type = %d, remain = %d\n",
//				posn, nim->iname_offset, nim->nifti_type, remain);
//
//	if( remain < 16 ){
//		if( g_opts.debug > 2 ){
//			if( g_opts.skip_blank_ext )
//				fprintf(stderr,"-d no extender in '%s' is okay, as "
//						"skip_blank_ext is set\n",nim->fname);
//			else
//				fprintf(stderr,"-d remain=%d, no space for extensions\n",remain);
//		}
//		return 0;
//	}
//
//	count = (int)znzread( extdr.extension, 1, 4, fp ); /* get extender */
//
//	if( count < 4 ){
//		if( g_opts.debug > 1 )
//			fprintf(stderr,"-d file '%s' is too short for an extender\n",
//					nim->fname);
//		return 0;
//	}
//
//	if( extdr.extension[0] != 1 ){
//		if( g_opts.debug > 2 )
//			fprintf(stderr,"-d extender[0] (%d) shows no extensions for '%s'\n",
//					extdr.extension[0], nim->fname);
//		return 0;
//	}
//
//	remain -= 4;
//	if( g_opts.debug > 2 )
//		fprintf(stderr,"-d found valid 4-byte extender, remain = %d\n", remain);
//
//	/* so we expect extensions, but have no idea of how many there may be */
//
//	count = 0;
//	Elist = NULL;
//	while (nifti_read_next_extension(&extn, nim, remain, fp) > 0)
//	{
//		if( nifti_add_exten_to_list(&extn, &Elist, count+1) < 0 ){
//			if( g_opts.debug > 0 )
//				fprintf(stderr,"** failed adding ext %ld to list\n", count);
//			return -1;
//		}
//
//		/* we have a new extension */
//		if( g_opts.debug > 1 ){
//			fprintf(stderr,"+d found extension #%ld, code = 0x%x, size = %d\n",
//					count, extn.ecode, extn.esize);
//			if( extn.ecode == NIFTI_ECODE_AFNI && g_opts.debug > 2 ) /* ~XML */
//				fprintf(stderr,"   AFNI extension: %.*s\n",
//						extn.esize-8,extn.edata);
//			else if( extn.ecode == NIFTI_ECODE_COMMENT && g_opts.debug > 2 )
//				fprintf(stderr,"   COMMENT extension: %.*s\n",        /* TEXT */
//						extn.esize-8,extn.edata);
//		}
//		remain -= extn.esize;
//		count++;
//	}
//
//	if( g_opts.debug > 2 ) fprintf(stderr,"+d found %ld extension(s)\n", count);
//
//	nim->num_ext = (int)count;
//	nim->ext_list = Elist;
//
//	return (int)count;
//}
//
//
///*----------------------------------------------------------------------*/
///*! nifti_add_extension - add an extension, with a copy of the data
//
// Add an extension to the nim->ext_list array.
// Fill this extension with a copy of the data, noting the
// length and extension code.
//
// \param nim    - nifti_image to add extension to
// \param data   - raw extension data
// \param length - length of raw extension data
// \param ecode  - extension code
//
// \sa extension codes NIFTI_ECODE_* in nifti1_io.h
// \sa nifti_free_extensions, valid_nifti_extensions, nifti_copy_extensions
//
// \return 0 on success, -1 on error (and free the entire list)
// *//*--------------------------------------------------------------------*/
//int nifti_add_extension(nifti_image *nim, const char * data, int len, int ecode)
//{
//	nifti1_extension ext;
//
//	/* error are printed in functions */
//	if( nifti_fill_extension(&ext, data, len, ecode) )                 return -1;
//	if( nifti_add_exten_to_list(&ext, &nim->ext_list, nim->num_ext+1)) return -1;
//
//	nim->num_ext++;  /* success, so increment */
//
//	return 0;
//}
//
//
///*----------------------------------------------------------------------*/
///* nifti_add_exten_to_list     - add a new nifti1_extension to the list
//
// We will append via "malloc, copy and free", because on an error,
// the list will revert to the previous one (sorry realloc(), only
// quality dolphins get to become part of St@rk!st brand tunafish).
//
// return 0 on success, -1 on error (and free the entire list)
// *//*--------------------------------------------------------------------*/
//static int nifti_add_exten_to_list( nifti1_extension *  new_ext,
//								   nifti1_extension ** list, long new_length )
//{
//	nifti1_extension * tmplist;
//
//	tmplist = *list;
//	*list = (nifti1_extension *)malloc(new_length * sizeof(nifti1_extension));
//
//	/* check for failure first */
//	if( ! *list ){
//		fprintf(stderr,"** failed to alloc %ld extension structs (%ld bytes)\n",
//				new_length, new_length*(int)sizeof(nifti1_extension));
//		if( !tmplist ) return -1;  /* no old list to lose */
//
//		*list = tmplist;  /* reset list to old one */
//		return -1;
//	}
//
//	/* if an old list exists, copy the pointers and free the list */
//	if( tmplist ){
//		memcpy(*list, tmplist, (new_length-1)*sizeof(nifti1_extension));
//		free(tmplist);
//	}
//
//	/* for some reason, I just don't like struct copy... */
//	(*list)[new_length-1].esize = new_ext->esize;
//	(*list)[new_length-1].ecode = new_ext->ecode;
//	(*list)[new_length-1].edata = new_ext->edata;
//
//	if( g_opts.debug > 2 )
//		fprintf(stderr,"+d allocated and appended extension #%ld to list\n",
//				new_length);
//
//	return 0;
//}
//
//
///*----------------------------------------------------------------------*/
///* nifti_fill_extension  - given data and length, fill an extension struct
//
// Allocate memory for data, copy data, set the size and code.
//
// return 0 on success, -1 on error (and free the entire list)
// *//*--------------------------------------------------------------------*/
//static int nifti_fill_extension( nifti1_extension *ext, const char * data,
//                                int len, int ecode)
//{
//	int esize;
//
//	if( !ext || !data || len < 0 ){
//		fprintf(stderr,"** fill_ext: bad params (%p,%p,%d)\n",
//				(void *)ext, data, len);
//		return -1;
//	} else if( ! nifti_is_valid_ecode(ecode) ){
//		fprintf(stderr,"** fill_ext: invalid ecode %d\n", ecode);
//		return -1;
//	}
//
//	/* compute esize, first : len+8, and take ceiling up to a mult of 16 */
//	esize = len+8;
//	if( esize & 0xf ) esize = (esize + 0xf) & ~0xf;
//	ext->esize = esize;
//
//	/* allocate esize-8 (maybe more than len), using calloc for fill */
//	ext->edata = (char *)calloc(esize-8, sizeof(char));
//	if( !ext->edata ){
//		fprintf(stderr,"** NFE: failed to alloc %d bytes for extension\n",len);
//		return -1;
//	}
//
//	memcpy(ext->edata, data, len);  /* copy the data, using len */
//	ext->ecode = ecode;             /* set the ecode */
//
//	if( g_opts.debug > 2 )
//		fprintf(stderr,"+d alloc %d bytes for ext len %d, ecode %d, esize %d\n",
//				esize-8, len, ecode, esize);
//
//	return 0;
//}
//
//
///*----------------------------------------------------------------------
// * nifti_read_next_extension  - read a single extension from the file
// *
// * return (>= 0 is okay):
// *
// *     success      : esize
// *     no extension : 0
// *     error        : -1
// *----------------------------------------------------------------------*/
//static int nifti_read_next_extension( nifti1_extension * nex, nifti_image *nim,
//									 int remain, znzFile fp )
//{
//	int swap = nim->byteorder != nifti_short_order();
//	int count, size, code;
//
//	/* first clear nex */
//	nex->esize = nex->ecode = 0;
//	nex->edata = NULL;
//
//	if( remain < 16 ){
//		if( g_opts.debug > 2 )
//			fprintf(stderr,"-d only %d bytes remain, so no extension\n", remain);
//		return 0;
//	}
//
//	/* must start with 4-byte size and code */
//	count = (int)znzread( &size, 4, 1, fp );
//	if( count == 1 ) count += (int)znzread( &code, 4, 1, fp );
//
//	if( count != 2 ){
//		if( g_opts.debug > 2 )
//			fprintf(stderr,"-d current extension read failed\n");
//		znzseek(fp, -4*count, SEEK_CUR); /* back up past any read */
//		return 0;                        /* no extension, no error condition */
//	}
//
//	if( swap ){
//		if( g_opts.debug > 2 )
//			fprintf(stderr,"-d pre-swap exts: code %d, size %d\n", code, size);
//
//		nifti_swap_4bytes(1, &size);
//		nifti_swap_4bytes(1, &code);
//	}
//
//	if( g_opts.debug > 2 )
//		fprintf(stderr,"-d potential extension: code %d, size %d\n", code, size);
//
//	if( !nifti_check_extension(nim, size, code, remain) ){
//		if( znzseek(fp, -8, SEEK_CUR) < 0 ){      /* back up past any read */
//			fprintf(stderr,"** failure to back out of extension read!\n");
//			return -1;
//		}
//		return 0;
//	}
//
//	/* now get the actual data */
//	nex->esize = size;
//	nex->ecode = code;
//
//	size -= 8;  /* subtract space for size and code in extension */
//	nex->edata = (char *)malloc(size * sizeof(char));
//	if( !nex->edata ){
//		fprintf(stderr,"** failed to allocate %d bytes for extension\n",size);
//		return -1;
//	}
//
//	count = (int)znzread(nex->edata, 1, size, fp);
//	if( count < size ){
//		if( g_opts.debug > 0 )
//			fprintf(stderr,"-d read only %d (of %d) bytes for extension\n",
//					count, size);
//		free(nex->edata);
//		nex->edata = NULL;
//		return -1;
//	}
//
//	/* success! */
//	if( g_opts.debug > 2 )
//		fprintf(stderr,"+d successfully read extension, code %d, size %d\n",
//				nex->ecode, nex->esize);
//
//	return nex->esize;
//}
//
//
///*----------------------------------------------------------------------*/
///*! for each extension, check code, size and data pointer
// *//*--------------------------------------------------------------------*/
//int valid_nifti_extensions(const nifti_image * nim)
//{
//	nifti1_extension * ext;
//	int                c, errs;
//
//	if( nim->num_ext <= 0 || nim->ext_list == NULL ){
//		if( g_opts.debug > 2 ) fprintf(stderr,"-d empty extension list\n");
//		return 0;
//	}
//
//	/* for each extension, check code, size and data pointer */
//	ext = nim->ext_list;
//	errs = 0;
//	for ( c = 0; c < nim->num_ext; c++ ){
//		if( ! nifti_is_valid_ecode(ext->ecode) ) {
//			if( g_opts.debug > 1 )
//				fprintf(stderr,"-d ext %d, invalid code %d\n", c, ext->ecode);
//			errs++;
//		}
//
//		if( ext->esize <= 0 ){
//			if( g_opts.debug > 1 )
//				fprintf(stderr,"-d ext %d, bad size = %d\n", c, ext->esize);
//			errs++;
//		} else if( ext->esize & 0xf ){
//			if( g_opts.debug > 1 )
//				fprintf(stderr,"-d ext %d, size %d not multiple of 16\n",
//						c, ext->esize);
//			errs++;
//		}
//
//		if( ext->edata == NULL ){
//			if( g_opts.debug > 1 ) fprintf(stderr,"-d ext %d, missing data\n", c);
//			errs++;
//		}
//
//		ext++;
//	}
//
//	if( errs > 0 ){
//		if( g_opts.debug > 0 )
//			fprintf(stderr,"-d had %d extension errors, none will be written\n",
//					errs);
//		return 0;
//	}
//
//	/* if we're here, we're good */
//	return 1;
//}
//
//
///*----------------------------------------------------------------------*/
///*! check whether the extension code is valid
//
// \return 1 if valid, 0 otherwise
// *//*--------------------------------------------------------------------*/
//int nifti_is_valid_ecode( int ecode )
//{
//	if( ecode < NIFTI_ECODE_IGNORE  ||   /* minimum code number (0) */
//       ecode > NIFTI_MAX_ECODE     ||   /* maximum code number     */
//       ecode & 1 )                      /* cannot be odd           */
//		return 0;
//
//	return 1;
//}
//
//
///*----------------------------------------------------------------------
// * check for valid size and code, as well as can be done
// *----------------------------------------------------------------------*/
//static int nifti_check_extension(nifti_image *nim, int size, int code, int rem)
//{
//	/* check for bad code before bad size */
//	if( ! nifti_is_valid_ecode(code) ) {
//		if( g_opts.debug > 2 )
//			fprintf(stderr,"-d invalid extension code %d\n",code);
//		return 0;
//	}
//
//	if( size < 16 ){
//		if( g_opts.debug > 2 )
//			fprintf(stderr,"-d ext size %d, no extension\n",size);
//		return 0;
//	}
//
//	if( size > rem ){
//		if( g_opts.debug > 2 )
//			fprintf(stderr,"-d ext size %d, space %d, no extension\n", size, rem);
//		return 0;
//	}
//
//	if( size & 0xf ){
//		if( g_opts.debug > 2 )
//			fprintf(stderr,"-d nifti extension size %d not multiple of 16\n",size);
//		return 0;
//	}
//
//	if( nim->nifti_type == NIFTI_FTYPE_ASCII && size > LNI_MAX_NIA_EXT_LEN ){
//		if( g_opts.debug > 2 )
//			fprintf(stderr,"-d NVE, bad nifti_type 3 size %d\n", size);
//		return 0;
//	}
//
//	return 1;
//}
//
///*--------------------------------------------------------------------------*/
///*! free the nifti extensions
//
// - If any edata pointer is set in the extension list, free() it.
// - Free ext_list, if it is set.
// - Clear num_ext and ext_list from nim.
//
// \return 0 on success, -1 on error
//
// \sa nifti_add_extension, nifti_copy_extensions
// *//*------------------------------------------------------------------------*/
//int nifti_free_extensions( nifti_image *nim )
//{
//	int c ;
//	if( nim == NULL ) return -1;
//	if( nim->num_ext > 0 && nim->ext_list ){
//		for( c = 0; c < nim->num_ext; c++ )
//			if ( nim->ext_list[c].edata ) free(nim->ext_list[c].edata);
//		free(nim->ext_list);
//	}
//	/* or if it is inconsistent, warn the user (if we are not in quiet mode) */
//	else if ( (nim->num_ext > 0 || nim->ext_list != NULL) && (g_opts.debug > 0) )
//		fprintf(stderr,"** warning: nifti extension num/ptr mismatch (%d,%p)\n",
//				nim->num_ext, (void *)nim->ext_list);
//
//	if( g_opts.debug > 2 )
//		fprintf(stderr,"+d free'd %d extension(s)\n", nim->num_ext);
//
//	nim->num_ext = 0;
//	nim->ext_list = NULL;
//
//	return 0;
//}
///* return number of extensions written, or -1 on error */
//static int nifti_write_extensions(znzFile fp, nifti_image *nim)
//{
//	nifti1_extension * list;
//	char               extdr[4] = { 0, 0, 0, 0 };
//	int                c, size, ok = 1;
//
//	if( znz_isnull(fp) || !nim || nim->num_ext < 0 ){
//		if( g_opts.debug > 0 )
//			fprintf(stderr,"** nifti_write_extensions, bad params\n");
//		return -1;
//	}
//
//	/* if no extensions and user requests it, skip extender */
//	if( g_opts.skip_blank_ext && (nim->num_ext == 0 || ! nim->ext_list ) ){
//		if( g_opts.debug > 1 )
//			fprintf(stderr,"-d no exts and skip_blank_ext set, "
//					"so skipping 4-byte extender\n");
//		return 0;
//	}
//
//	/* if invalid extension list, clear num_ext */
//	if( ! valid_nifti_extensions(nim) ) nim->num_ext = 0;
//
//	/* write out extender block */
//	if( nim->num_ext > 0 ) extdr[0] = 1;
//	if( nifti_write_buffer(fp, extdr, 4) != 4 ){
//		fprintf(stderr,"** failed to write extender\n");
//		return -1;
//	}
//
//	list = nim->ext_list;
//	for ( c = 0; c < nim->num_ext; c++ ){
//		size = (int)nifti_write_buffer(fp, &list->esize, sizeof(int));
//		ok = (size == (int)sizeof(int));
//		if( ok ){
//			size = (int)nifti_write_buffer(fp, &list->ecode, sizeof(int));
//			ok = (size == (int)sizeof(int));
//		}
//		if( ok ){
//			size = (int)nifti_write_buffer(fp, list->edata, list->esize - 8);
//			ok = (size == list->esize - 8);
//		}
//
//		if( !ok ){
//			fprintf(stderr,"** failed while writing extension #%d\n",c);
//			return -1;
//		} else if ( g_opts.debug > 2 )
//			fprintf(stderr,"+d wrote extension %d of %d bytes\n", c, size);
//
//		list++;
//	}
//
//	if( g_opts.debug > 1 )
//		fprintf(stderr,"+d wrote out %d extension(s)\n", nim->num_ext);
//
//	return nim->num_ext;
//}
///*----------------------------------------------------------------------*/
///*! \fn int nifti_copy_extensions(nifti_image * nim_dest, nifti_image * nim_src)
// \brief copy the nifti1_extension list from src to dest
//
// Duplicate the list of nifti1_extensions.  The dest structure must
// be clear of extensions.
// \return 0 on success, -1 on failure
//
// \sa nifti_add_extension, nifti_free_extensions
// */
//int nifti_copy_extensions(nifti_image * nim_dest, const nifti_image * nim_src)
//{
//	char   * data;
//	size_t   bytes;
//	int      c, size, old_size;
//
//	if( nim_dest->num_ext > 0 || nim_dest->ext_list != NULL ){
//		fprintf(stderr,"** will not copy extensions over existing ones\n");
//		return -1;
//	}
//
//	if( g_opts.debug > 1 )
//		fprintf(stderr,"+d duplicating %d extension(s)\n", nim_src->num_ext);
//
//	if( nim_src->num_ext <= 0 ) return 0;
//
//	bytes = nim_src->num_ext * sizeof(nifti1_extension);  /* I'm lazy */
//	nim_dest->ext_list = (nifti1_extension *)malloc(bytes);
//	if( !nim_dest->ext_list ){
//		fprintf(stderr,"** failed to allocate %d nifti1_extension structs\n",
//				nim_src->num_ext);
//		return -1;
//	}
//
//	/* copy the extension data */
//	nim_dest->num_ext = 0;
//	for( c = 0; c < nim_src->num_ext; c++ ){
//		size = old_size = nim_src->ext_list[c].esize;
//		if( size & 0xf ) size = (size + 0xf) & ~0xf; /* make multiple of 16 */
//		if( g_opts.debug > 2 )
//			fprintf(stderr,"+d dup'ing ext #%d of size %d (from size %d)\n",
//					c, size, old_size);
//		/* data length is size-8, as esize includes space for esize and ecode */
//		data = (char *)calloc(size-8,sizeof(char));      /* maybe size > old */
//		if( !data ){
//			fprintf(stderr,"** failed to alloc %d bytes for extention\n", size);
//			if( c == 0 ) { free(nim_dest->ext_list); nim_dest->ext_list = NULL; }
//			/* otherwise, keep what we have (a.o.t. deleting them all) */
//			return -1;
//		}
//		/* finally, fill the new structure */
//		nim_dest->ext_list[c].esize = size;
//		nim_dest->ext_list[c].ecode = nim_src->ext_list[c].ecode;
//		nim_dest->ext_list[c].edata = data;
//		memcpy(data, nim_src->ext_list[c].edata, old_size-8);
//
//		nim_dest->num_ext++;
//	}
//
//	return 0;
//}
//
//
///*----------------------------------------------------------------------*/
///*! compute the total size of all extensions
//
// \return the total of all esize fields
//
// Note that each esize includes 4 bytes for ecode, 4 bytes for esize,
// and the bytes used for the data.  Each esize also needs to be a
// multiple of 16, so it may be greater than the sum of its 3 parts.
// *//*--------------------------------------------------------------------*/
//int nifti_extension_size(nifti_image *nim)
//{
//	int c, size = 0;
//
//	if( !nim || nim->num_ext <= 0 ) return 0;
//
//	if( g_opts.debug > 2 ) fprintf(stderr,"-d ext sizes:");
//
//	for ( c = 0; c < nim->num_ext; c++ ){
//		size += nim->ext_list[c].esize;
//		if( g_opts.debug > 2 ) fprintf(stderr,"  %d",nim->ext_list[c].esize);
//	}
//
//	if( g_opts.debug > 2 ) fprintf(stderr," (total = %d)\n",size);
//
//	return size;
//}

NiftiImage::~NiftiImage()
{
    if (_mode != NIFTI_CLOSED)
        close();
}

NiftiImage::NiftiImage() :
    _dim{ 0, 0, 0, 0, 0, 0, 0, 0 },
    _voxdim{ 0., 1., 1., 1., 0., 0., 0., 0. },
    _mode(NIFTI_CLOSED),
    _gz(false),
    _swap(false)
{
    _qform.setIdentity(); _sform.setIdentity();
    setDatatype(NIFTI_TYPE_FLOAT32);
}

NiftiImage::NiftiImage(const string &filename, const char &mode) :
    _dim(),
    _voxdim(),
    _mode(NIFTI_CLOSED),
    _gz(false),
    _swap(false)
{
    _qform.setIdentity(); _sform.setIdentity();
    setDatatype(NIFTI_TYPE_FLOAT32);
    open(filename, mode);
}

NiftiImage::NiftiImage(const int nx, const int ny, const int nz, const int nt,
    const float dx, const float dy, const float dz, const float dt,
    const int datatype) :
    _mode(NIFTI_CLOSED),
    _gz(false),
    _swap(false)
{
    setDatatype(datatype);
    _qform.setIdentity(); _sform.setIdentity();
    _dim[1] = nx; _dim[2] = ny; _dim[3] = nz; _dim[4] = nt;
    _dim[0] = (nt > 1) ? 4 : 3;
    _voxdim[1] = dx; _voxdim[2] = dy; _voxdim[3] = dz; _voxdim[4] = dt;
}

NiftiImage::NiftiImage(const NiftiImage &clone) :
    _qform(clone._qform), _sform(clone._sform),
    _voxoffset(0),
    _datatype(clone._datatype), _mode(NIFTI_CLOSED), _gz(false),
    scaling_slope(clone.scaling_slope), scaling_inter(clone.scaling_inter),
    calibration_min(clone.calibration_min), calibration_max(clone.calibration_max),
    qform_code(clone.qform_code), sform_code(clone.sform_code),
    freq_dim(clone.freq_dim), phase_dim(clone.phase_dim),
    slice_dim(clone.slice_dim), slice_code(clone.slice_code),
    slice_start(clone.slice_start), slice_end(clone.slice_end),
    slice_duration(clone.slice_duration),
    toffset(clone.toffset),
    xyz_units(clone.xyz_units), time_units(clone.time_units),
    intent_code(clone.intent_code),
    intent_p1(clone.intent_p1), intent_p2(clone.intent_p2), intent_p3(clone.intent_p3),
    intent_name(clone.intent_name),
    description(clone.description),
    aux_file(clone.aux_file)
{
    for (int i = 0; i < 8; i++)
    {
        _dim[i] = clone._dim[i];
        _voxdim[i] = clone._voxdim[i];
    }
    //int                num_ext ;  /*!< number of extensions in ext_list       */
    //nifti1_extension * ext_list ; /*!< array of extension structs (with data) */
    //analyze_75_orient_code analyze75_orient; /*!< for old analyze files, orient */
}

NiftiImage &NiftiImage::operator=(const NiftiImage &other)
{
    if (this == &other)
        return *this;
    else if (_mode != NIFTI_CLOSED)
        close();

    _qform = other._qform;
    _sform = other._sform;
    _mode = NIFTI_CLOSED;
    _gz = false;
    _voxoffset = 0;
    scaling_slope = other.scaling_slope;
    scaling_inter = other.scaling_inter;
    calibration_min = other.calibration_min;
    calibration_max = other.calibration_max;
    qform_code = other.qform_code;
    sform_code = other.sform_code;
    freq_dim = other.freq_dim;
    phase_dim = other.phase_dim;
    slice_dim = other.slice_dim;
    slice_code = other.slice_code;
    slice_start = other.slice_start;
    slice_end = other.slice_end;
    slice_duration = other.slice_duration;
    toffset = other.toffset;
    xyz_units = other.xyz_units;
    time_units = other.time_units;
    intent_code = other.intent_code;
    intent_p1 = other.intent_p1;
    intent_p2 = other.intent_p2;
    intent_p3 = other.intent_p3;
    intent_name = other.intent_name;
    description = other.description;
    aux_file = other.aux_file;
    setDatatype(other.datatype());
    for (int i = 0; i < 8; i++)
    {
        _dim[i] = other._dim[i];
        _voxdim[i] = other._voxdim[i];
    }
    return *this;
}

bool isGZippedFile(const string &fname)
{
    if (fname.find_last_of(".") != string::npos)
    {
        string ext = fname.substr(fname.find_last_of(".") + 1);
        transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == "gz")
            return true;
    }
    return false;
}

void NiftiImage::setFilenames(const string &fname)
{
	_ext = fname.substr(fname.find_last_of(".") + 1);
    _basename = fname.substr(0, fname.find_last_of("."));
    _gz = false;
	if (_ext == "gz") {
        _gz = true;
		_ext = _basename.substr(_basename.find_last_of(".") + 1);
        _basename = _basename.substr(0, _basename.find_last_of("."));
    }
	if (_ext == "hdr" || _ext == "img") {
        _imgname = _basename + ".img";
        _hdrname = _basename + ".hdr";
    }
	else if (_ext == "nii") {
        _imgname = _hdrname = _basename + ".nii";
    }
    else {
		cerr << "NiftiImage: Extension " << _ext << " is not a valid NIfTI extension." << endl;
        exit(EXIT_FAILURE);
    }
    if (_gz) {
        _imgname += ".gz";
        _hdrname += ".gz";
		_ext += ".gz";
    }
}
const string &NiftiImage::basename() { return _basename; }
const string &NiftiImage::extname() { return _ext; }

size_t NiftiImage::read(void *buff, size_t size, size_t nmemb)
{
    if (_gz)
    {
        unsigned long remain = size*nmemb;
        char *cbuf = (char *)buff;
        unsigned long n2read;
        int nread;
        while (remain > 0) {
            n2read = (remain < MaxZippedBytes) ? remain : MaxZippedBytes;
            nread = gzread(_file.zipped, (void *)cbuf, (unsigned int)n2read);
            if (nread < 0) return nread; /* returns -1 on error */

            remain -= nread;
            cbuf += nread;

            /* require reading n2read bytes, so we don't get stuck */
            if (nread < (int)n2read) break;  /* return will be short */
        }

        /* warn of a short read that will seem complete */
        if (remain > 0 && remain < size)
            cerr << "NiftiImage: Zipped read short by " << remain << " bytes." << endl;
        return nmemb - remain / size;   /* return number of members processed */
    }
    else
        return fread(buff, size, nmemb, _file.unzipped);
}

size_t NiftiImage::write(const void *buff, size_t size, size_t nmemb)
{
    if (_gz) {
        unsigned long remain = size*nmemb;
        char     * cbuf = (char *)buff;
        unsigned long n2write;
        int        nwritten;
        while (remain > 0) {
            n2write = (remain < MaxZippedBytes) ? remain : MaxZippedBytes;
            nwritten = gzwrite(_file.zipped, (void *)cbuf, (unsigned int)n2write);

            /* gzread returns 0 on error, but in case that ever changes... */
            if (nwritten < 0) return nwritten;

            remain -= nwritten;
            cbuf += nwritten;

            /* require writing n2write bytes, so we don't get stuck */
            if (nwritten < (int)n2write) break;
        }

        /* warn of a short write that will seem complete */
        if (remain > 0 && remain < size)
            cerr << "NiftiImage: Zipped write short by " << remain << " bytes." << endl;

        return nmemb - remain / size;   /* return number of members processed */
    }
    return fwrite(buff, size, nmemb, _file.unzipped);
}

/*  Seeks into a NIfTI file. Does some checking to ensure we are not seeking
*  into the header, or seeking backwards in GZipped file that we are writing
*  to, as Zlib doesn't support this but doesn't fail when you attempt it.
*/
long NiftiImage::seek(long offset, int whence)
{
    if (_mode == NIFTI_CLOSED)	// Seek is private, so this should not happen
        NIFTI_FAIL("Cannot seek in a closed file.");
    long newpos, currpos, error;
    if (_gz)
        currpos = gztell(_file.zipped);
    else
        currpos = ftell(_file.unzipped);

    // Work out where we are going to seek to.
    if (whence == SEEK_SET)
        newpos = offset;
    else if (whence == SEEK_CUR)
        newpos = currpos + offset;
    else {
        if (_gz) {
            gzseek(_file.zipped, 0, whence);
            newpos = gztell(_file.zipped) - offset;
        }
        else {
            fseek(_file.unzipped, 0, whence);
            newpos = ftell(_file.unzipped) - offset;
        }
    }

    if (newpos == currpos)
        return 0;	// No need to seek
    else if (newpos < _voxoffset)
        NIFTI_FAIL("Attempted to seek into the header of file " + _imgname);
    else if (_gz && (_mode == NIFTI_WRITE) && (newpos < currpos))
        NIFTI_FAIL("Cannot seek backwards while writing a file with libz.");

    if (_gz)
        error = gzseek(_file.zipped, offset, whence);
    else
        error = fseek(_file.unzipped, offset, whence);
    return error;
}

inline float NiftiImage::fixFloat(const float f)
{
    if (isfinite(f))
        return f;
    else
        return 0.;
}

void NiftiImage::readHeader(string path)
{
    struct nifti_1_header nhdr;

    if (_gz)
        _file.zipped = gzopen(path.c_str(), "rb");
    else
        _file.unzipped = fopen(path.c_str(), "rb");
    if (!_file.zipped)
        NIFTI_FAIL("Failed to open header from " + path);

    size_t obj_read;
    if (_gz) {
        size_t bytes_read = gzread(_file.zipped, &nhdr, sizeof(nhdr));
        obj_read = bytes_read / sizeof(nhdr);
    }
    else
        obj_read = fread(&nhdr, sizeof(nhdr), 1, _file.unzipped);
    if (obj_read < 1)
        NIFTI_FAIL("Could not read header structure from " + _hdrname);

    // Check if disk and CPU byte order match.
    // The sizeof_hdr field should always be 352, as this is the size of a
    // NIfTI-1 header
    if (nhdr.sizeof_hdr != sizeof(nhdr)) {
        SwapBytes(1, 4, &nhdr.sizeof_hdr);
        if (nhdr.sizeof_hdr != sizeof(nhdr))
            NIFTI_FAIL("Could not determine byte order of header " + _hdrname);
        // If we didn't fail, then we need to swap the header (first swap sizeof back)
        _swap = true;
        SwapBytes(1, 4, &nhdr.sizeof_hdr);
    }

    // Check the magic string is set to one of the possible NIfTI values,
    // otherwise process as an ANALYZE file
    int is_nifti = ((nhdr.magic[0] == 'n' && nhdr.magic[3] == '\0') &&
        (nhdr.magic[1] == 'i' || nhdr.magic[1] == '+') &&
        (nhdr.magic[2] >= '1' && nhdr.magic[2] <= '9')) ? true : false;

    if (_swap && is_nifti)
        SwapNiftiHeader(&nhdr);
    else if (_swap)
        SwapAnalyzeHeader((nifti_analyze75 *)&nhdr);

    if (nhdr.datatype == DT_BINARY || nhdr.datatype == DT_UNKNOWN)
        cerr << "Bad datatype in header " << _hdrname << endl;
    if (nhdr.dim[1] <= 0)
        cerr << "Bad first dimension in header " << _hdrname << endl;

    /* fix bad dim[] values in the defined dimension range */
    for (int i = 2; i <= nhdr.dim[0]; i++)
        if (nhdr.dim[i] <= 0) nhdr.dim[i] = 1;

    /* fix any remaining bad dim[] values, so garbage does not propagate */
    /* (only values 0 or 1 seem rational, otherwise set to arbirary 1)   */
    for (int i = nhdr.dim[0] + 1; i < 8; i++)
        if (nhdr.dim[i] != 1 && nhdr.dim[i] != 0) nhdr.dim[i] = 1;

    /**- Set dimensions arrays */
    for (int i = 0; i < 8; i++)
    {
        _dim[i] = nhdr.dim[i];
        _voxdim[i] = nhdr.pixdim[i];
    }

    /**- set the type of data in voxels and how many bytes per voxel */
    setDatatype(nhdr.datatype);

    /**- compute qto_xyz transformation from pixel indexes (i,j,k) to (x,y,z) */
    Affine3d S;
    S = Scaling((double)_voxdim[1], (double)_voxdim[2], (double)_voxdim[3]);    //    S = Scaling<double>(_voxdim[1], _voxdim[2], _voxdim[3]);
    if (!is_nifti || nhdr.qform_code <= 0) {
        /**- if not nifti or qform_code <= 0, use grid spacing for qto_xyz */
        _qform = S.matrix();
        qform_code = NIFTI_XFORM_UNKNOWN;
    }
    else {
        /**- else NIFTI: use the quaternion-specified transformation */
        double b = fixFloat(nhdr.quatern_b);
        double c = fixFloat(nhdr.quatern_c);
        double d = fixFloat(nhdr.quatern_d);

        double x = fixFloat(nhdr.qoffset_x);
        double y = fixFloat(nhdr.qoffset_y);
        double z = fixFloat(nhdr.qoffset_z);
        double qfac = (nhdr.pixdim[0] < 0.0) ? -1.0 : 1.0;  /* left-handedness? */
        double a = sqrt(1 - (b*b + c*c + d*d));
        Quaterniond Q(a, b, c, d);
        Affine3d T; T = Translation3d(x, y, z);
        _qform = T*Q*S;
        if (qfac < 0.)
            _qform.matrix().block(0, 2, 3, 1) *= -1.;
        qform_code = nhdr.qform_code;
    }
    /**- load sto_xyz affine transformation, if present */
    if (!is_nifti || nhdr.sform_code <= 0)
    {	/**- if not nifti or sform_code <= 0, then no sto transformation */
        sform_code = NIFTI_XFORM_UNKNOWN;
    }
    else {
        /**- else set the sto transformation from srow_*[] */
        _sform.setIdentity();
        for (int i = 0; i < 4; i++) {
            _sform(0, i) = nhdr.srow_x[i];
            _sform(1, i) = nhdr.srow_y[i];
            _sform(2, i) = nhdr.srow_z[i];
        }
        sform_code = nhdr.sform_code;
    }

    /**- set miscellaneous NIFTI stuff */
    if (is_nifti) {
        scaling_slope = fixFloat(nhdr.scl_slope);
        if (scaling_slope == 0.)
            scaling_slope = 1.;
        scaling_inter = fixFloat(nhdr.scl_inter);
        intent_code = nhdr.intent_code;
        intent_p1 = fixFloat(nhdr.intent_p1);
        intent_p2 = fixFloat(nhdr.intent_p2);
        intent_p3 = fixFloat(nhdr.intent_p3);
        toffset = fixFloat(nhdr.toffset);

        intent_name = string(nhdr.intent_name);
        xyz_units = XYZT_TO_SPACE(nhdr.xyzt_units);
        time_units = XYZT_TO_TIME(nhdr.xyzt_units);
        freq_dim = DIM_INFO_TO_FREQ_DIM(nhdr.dim_info);
        phase_dim = DIM_INFO_TO_PHASE_DIM(nhdr.dim_info);
        slice_dim = DIM_INFO_TO_SLICE_DIM(nhdr.dim_info);
        slice_code = nhdr.slice_code;
        slice_start = nhdr.slice_start;
        slice_end = nhdr.slice_end;
        slice_duration = fixFloat(nhdr.slice_duration);
    }

    /**- set Miscellaneous ANALYZE stuff */

    calibration_min = fixFloat(nhdr.cal_min);
    calibration_max = fixFloat(nhdr.cal_max);

    description = string(nhdr.descrip);
    aux_file = string(nhdr.aux_file);

    /**- set ioff from vox_offset (but at least sizeof(header)) */
    if (_hdrname == _imgname) {
        _voxoffset = (int)nhdr.vox_offset;
        if (_voxoffset < (int)sizeof(nhdr)) _voxoffset = (int)sizeof(nhdr);
    }
    else {
        _voxoffset = (int)nhdr.vox_offset;
    }

    /* clear extension fields */
    _num_ext = 0;
    _ext_list = NULL;
    /**- check for extensions (any errors here means no extensions) */
    //if( NIFTI_ONEFILE(nhdr) ) remaining = nim->iname_offset - sizeof(nhdr);
    //else                      remaining = filesize - sizeof(nhdr);

    //(void)nifti_read_extensions(nim, fp, remaining);

    if (_hdrname != _imgname) {
        // Need to close the header and open the image
        if (_gz) {
            gzclose(_file.zipped);
            _file.zipped = gzopen(_imgname.c_str(), "rb");
        }
        else {
            fclose(_file.unzipped);
            _file.unzipped = fopen(_imgname.c_str(), "rb");
        }
    }
}

void NiftiImage::writeHeader()
{
    struct nifti_1_header nhdr;
    memset(&nhdr, 0, sizeof(nhdr));  /* zero out header, to be safe */
                                     /**- load the ANALYZE-7.5 generic parts of the header struct */
    nhdr.sizeof_hdr = sizeof(nhdr);
    nhdr.regular = 'r';             /* for some stupid reason */

    for (int i = 0; i < 8; i++)
    {	// Copy this way so types can be changed
        nhdr.dim[i] = _dim[i];
        nhdr.pixdim[i] = _voxdim[i];
    }

    nhdr.datatype = _datatype.code;
    nhdr.bitpix = 8 * _datatype.size;

    if (calibration_max > calibration_min) {
        nhdr.cal_max = calibration_max;
        nhdr.cal_min = calibration_min;
    }

    if (scaling_slope != 0.0) {
        nhdr.scl_slope = scaling_slope;
        nhdr.scl_inter = scaling_inter;
    }

    strncpy(nhdr.descrip, description.c_str(), 80);
    strncpy(nhdr.aux_file, aux_file.c_str(), 24);


    if (_imgname == _hdrname)
        strcpy(nhdr.magic, "n+1");
    else
        strcpy(nhdr.magic, "ni1");
    for (int i = 1; i < 8; i++)
        nhdr.pixdim[i] = fabs(nhdr.pixdim[i]);

    nhdr.intent_code = intent_code;
    nhdr.intent_p1 = intent_p1;
    nhdr.intent_p2 = intent_p2;
    nhdr.intent_p3 = intent_p3;
    strncpy(nhdr.intent_name, intent_name.c_str(), 16);

    // Check that _voxoffset is sensible
    if (_imgname == _hdrname && _voxoffset < nhdr.sizeof_hdr)
        _voxoffset = 352;
    nhdr.vox_offset = _voxoffset;
    nhdr.xyzt_units = SPACE_TIME_TO_XYZT(xyz_units, time_units);
    nhdr.toffset = toffset;

    nhdr.qform_code = qform_code;
    Quaterniond Q(_qform.rotation());
    Translation3d T(_qform.translation());
    Affine3d S;
    S = Scaling((double)_voxdim[1], (double)_voxdim[2], (double)_voxdim[3]);    // S = Scaling<double>(_voxdim[1], _voxdim[2], _voxdim[3]);

    // NIfTI REQUIRES a (or w) >= 0. Because Q and -Q represent the same
    // rotation, if w < 0 simply store -Q
    if (Q.w() < 0) {
        nhdr.quatern_b = -Q.x();
        nhdr.quatern_c = -Q.y();
        nhdr.quatern_d = -Q.z();
    }
    else {
        nhdr.quatern_b = Q.x();
        nhdr.quatern_c = Q.y();
        nhdr.quatern_d = Q.z();
    }
    nhdr.qoffset_x = T.x();
    nhdr.qoffset_y = T.y();
    nhdr.qoffset_z = T.z();

    nhdr.sform_code = sform_code;
    for (int i = 0; i < 4; i++) {
        nhdr.srow_x[i] = _sform(0, i);
        nhdr.srow_y[i] = _sform(1, i);
        nhdr.srow_z[i] = _sform(2, i);
    }

    nhdr.dim_info = FPS_INTO_DIM_INFO(freq_dim, phase_dim, slice_dim);
    nhdr.slice_code = slice_code;
    nhdr.slice_start = slice_start;
    nhdr.slice_end = slice_end;
    nhdr.slice_duration = slice_duration;

    if (_gz)
        _file.zipped = gzopen(_hdrname.c_str(), "wb");
    else
        _file.unzipped = fopen(_hdrname.c_str(), "wb");

    if (!_file.zipped)
        NIFTI_FAIL("Cannot open header file " + _hdrname + " for writing.");

    /* write the header and extensions */
    size_t bytesWritten;
    if (_gz)
        bytesWritten = gzwrite(_file.zipped, &nhdr, sizeof(nhdr));
    else
        bytesWritten = fwrite(&nhdr, sizeof(nhdr), 1, _file.unzipped);
    /* partial file exists, and errors have been printed, so ignore return */
    //if( nim->nifti_type != NIFTI_FTYPE_ANALYZE )
    //	(void)nifti_write_extensions(fp,nim);
    if (bytesWritten < sizeof(nhdr))
        NIFTI_FAIL("Could not write header to file " + _hdrname + ".");

    if (_hdrname != _imgname)
    {	// Close header and open image file
        if (_gz) {
            gzclose(_file.zipped);
            _file.zipped = gzopen(_imgname.c_str(), "wb");
        }
        else {
            fclose(_file.unzipped);
            _file.unzipped = fopen(_imgname.c_str(), "wb");
        }
        if (!_file.zipped)
            NIFTI_FAIL("Could not open image file " + _imgname + " for writing.");
    }
}

/**
*   Reads a sequence of bytes from the open NIfTI image.
*
*   Internal function to actually read bytes from an image file.
*   @param start Location in file to start the read
*   @param length Number of bytes to read
*   @param buffer Location to read bytes to. If none, or NULL, specified,
*          memory will be allocated internally and a pointer returned.
*   @return On success a pointer to the read bytes (if buffer was specified
*           then will be the same). NULL on fail.
*/
char *NiftiImage::readBytes(size_t start, size_t length, char *buffer)
{
    bool didAllocate = false;
    if (!buffer)
    {
        buffer = new char[length];
        didAllocate = true;
    }

    if (_mode == NIFTI_CLOSED)
    {
        NIFTI_ERROR("Cannot read from a closed file.");
        if (didAllocate) delete[] buffer;
        return NULL;
    }
    if (_mode == NIFTI_WRITE)
    {
        NIFTI_ERROR("Cannot read from a file opened for writing.");
        if (didAllocate) delete[] buffer;
        return NULL;
    }
    if (length == 0)
    {
        NIFTI_ERROR("Asked to read a buffer of 0 bytes.");
        if (didAllocate) delete[] buffer;
        return NULL;
    }

    seek(_voxoffset + start, SEEK_SET);
    size_t obj_read;
    if (_gz)
    {
        size_t bytesRead = gzread(_file.zipped, buffer, static_cast<unsigned int>(length));
        obj_read = bytesRead / length;
    }
    else
        obj_read = fread(buffer, length, 1, _file.unzipped);

    if (obj_read != 1)
        NIFTI_ERROR("Read buffer returned wrong number of bytes.");

    if (_datatype.swapsize > 1 && _swap)
        SwapBytes(length / _datatype.swapsize, _datatype.swapsize, buffer);
    return buffer;
}

/**
*   Writes a sequence of bytes to the open NIfTI image.
*
*   Internal function to actually write bytes to an image file.
*   @param buffer Buffer to write bytes from.
*   @param start Location in file to start the write
*   @param length Number of bytes to write
*/
void NiftiImage::writeBytes(char *buffer, size_t start, size_t length)
{
    if (_mode == NIFTI_CLOSED)
    {
        NIFTI_ERROR("Cannot write to a closed file.");
        return;
    }
    if (_mode == NIFTI_READ)
    {
        NIFTI_ERROR("Cannot write to a file opened for writing.");
        return;
    }
    seek(_voxoffset + start, SEEK_SET);
    size_t obj_written;
    if (_gz)
    {
        size_t bytesWritten = gzwrite(_file.zipped, buffer, static_cast<unsigned int>(length));
        obj_written = bytesWritten / length;
    }
    else
        obj_written = fwrite(buffer, length, 1, _file.unzipped);

    if (obj_written != 1)
        NIFTI_ERROR("Write buffer failed.");
}

char *NiftiImage::readRawVolume(const int vol)
{
    size_t bytesPerVolume = voxelsPerVolume() * _datatype.size;
    char *raw = readBytes(vol * bytesPerVolume, bytesPerVolume);
    return raw;
}
char *NiftiImage::readRawAllVolumes()
{
    char *raw = readBytes(0, voxelsTotal() * _datatype.size);
    return raw;
}

bool NiftiImage::open(const string &filename, const char &mode)
{
    setFilenames(filename);
    if (_mode != NIFTI_CLOSED)
        NIFTI_FAIL("Attempted to open file " + filename +
            " using NiftiImage that is already open with file " + _imgname);
    if (mode == NIFTI_READ) {
        readHeader(_hdrname); // readHeader leaves _file pointing to image file on success
        if (!_file.zipped)
            return false;
        _mode = NIFTI_READ;
        seek(_voxoffset, SEEK_SET);
    }
    else if (mode == NIFTI_WRITE) {
        writeHeader(); // writeHeader ensures file is opened to image file on success
        if (!_file.zipped)
            return false;
        _mode = NIFTI_WRITE;
        seek(_voxoffset, SEEK_SET);
    }
    else {
        NIFTI_FAIL(string("Invalid NiftImage mode '") + mode + "'.");
    }
    return true;
}

void NiftiImage::close()
{
    // If we've been writing subvolumes then we may not have written a complete file
    // Write a single zero-byte at the end to persuade the OS to write a file of the
    // correct size.
    if (_mode == NIFTI_CLOSED) {
        NIFTI_ERROR("file " + _basename + " is already closed.");
        return;
    }
    seek(0, SEEK_END);
    long correctEnd = (voxelsTotal() * _datatype.size + _voxoffset);
    char zero = 0;
    if (_gz) {
        long pos = gztell(_file.zipped);
        if (pos < correctEnd) {
            gzseek(_file.zipped, correctEnd - 1, SEEK_SET);
            gzwrite(_file.zipped, &zero, 1);
        }
        gzflush(_file.zipped, Z_FINISH);
        gzclose(_file.zipped);
        _file.zipped = NULL;
    }
    else {
        long pos = ftell(_file.unzipped);
        if (pos < correctEnd) {
            fseek(_file.unzipped, correctEnd - 1, SEEK_SET);
            fwrite(&zero, 1, 1, _file.unzipped);
        }
        fflush(_file.unzipped);
        fclose(_file.unzipped);
        _file.unzipped = NULL;
    }
    _mode = NIFTI_CLOSED;
}

int NiftiImage::ndim() const { return _dim[0]; }
int NiftiImage::nx() const { return _dim[1]; }
int NiftiImage::ny() const { return _dim[2]; }
int NiftiImage::nz() const { return _dim[3]; }
int NiftiImage::nt() const { return _dim[4]; }
void NiftiImage::setnt(const int nt)
{
    if (_mode == NIFTI_CLOSED)
    {
        if (nt > 1)
        {
            _dim[4] = nt;
            _dim[0] = 4;
        }
        else
        {
            _dim[4] = 1;
            _dim[0] = 3;
        }
    }
    else
    {
        cerr << "NiftiImage: Cannot change the dimensions of an image once opened." << endl;
        exit(EXIT_FAILURE);
    }
}

void NiftiImage::setDims(const int nx, const int ny, const int nz, const int nt)
{
    //setnt will check if the file is closed
    setnt(nt);
    _dim[1] = nx;
    _dim[2] = ny;
    _dim[3] = nz;
}
int NiftiImage::voxelsPerSlice() const { return _dim[1] * _dim[2]; }
int NiftiImage::voxelsPerVolume() const { return _dim[1] * _dim[2] * _dim[3]; }
int NiftiImage::voxelsTotal() const { return _dim[1] * _dim[2] * _dim[3] * _dim[4]; }
float NiftiImage::dx() const { return _voxdim[1]; }
float NiftiImage::dy() const { return _voxdim[2]; }
float NiftiImage::dz() const { return _voxdim[3]; }

int NiftiImage::datatype() const { return _datatype.code; }
/*  The map is declared here because making it a static member of NiftiImage was
*  causing problems with looking up the datatype in close() when called by
*  ~NiftiImage. It's possible for C++ to destruct static members even when
*  objects still exist in another translation unit.
*/
void NiftiImage::setDatatype(const int dt)
{
    static const DTMap DataTypes{
        { NIFTI_TYPE_UINT8,{ NIFTI_TYPE_UINT8, 1, 0, "NIFTI_TYPE_UINT8" } },
        { NIFTI_TYPE_INT16,{ NIFTI_TYPE_INT16, 2, 2, "NIFTI_TYPE_INT16" } },
        { NIFTI_TYPE_INT32,{ NIFTI_TYPE_INT32, 4, 4, "NIFTI_TYPE_INT32" } },
        { NIFTI_TYPE_FLOAT32,{ NIFTI_TYPE_FLOAT32, 4, 4, "NIFTI_TYPE_FLOAT32" } },
        { NIFTI_TYPE_COMPLEX64,{ NIFTI_TYPE_COMPLEX64, 8, 4, "NIFTI_TYPE_COMPLEX64" } },
        { NIFTI_TYPE_FLOAT64,{ NIFTI_TYPE_FLOAT64, 8, 8, "NIFTI_TYPE_FLOAT64" } },
        { NIFTI_TYPE_RGB24,{ NIFTI_TYPE_RGB24, 3, 0, "NIFTI_TYPE_RGB24" } },
        { NIFTI_TYPE_INT8,{ NIFTI_TYPE_INT8, 1, 0, "NIFTI_TYPE_INT8" } },
        { NIFTI_TYPE_UINT16,{ NIFTI_TYPE_UINT16, 2, 2, "NIFTI_TYPE_UINT16" } },
        { NIFTI_TYPE_UINT32,{ NIFTI_TYPE_UINT32, 4, 4, "NIFTI_TYPE_UINT32" } },
        { NIFTI_TYPE_INT64,{ NIFTI_TYPE_INT64, 8, 8, "NIFTI_TYPE_INT64" } },
        { NIFTI_TYPE_UINT64,{ NIFTI_TYPE_UINT64, 8, 8, "NIFTI_TYPE_UINT64" } },
        { NIFTI_TYPE_FLOAT128,{ NIFTI_TYPE_FLOAT128, 16, 16, "NIFTI_TYPE_FLOAT128" } },
        { NIFTI_TYPE_COMPLEX128,{ NIFTI_TYPE_COMPLEX128, 16,  8, "NIFTI_TYPE_COMPLEX128" } },
        { NIFTI_TYPE_COMPLEX256,{ NIFTI_TYPE_COMPLEX256, 32, 16, "NIFTI_TYPE_COMPLEX256" } },
        { NIFTI_TYPE_RGBA32,{ NIFTI_TYPE_RGBA32, 4,   0, "NIFTI_TYPE_RGBA32" } }
    };

    if (_mode == NIFTI_READ)
    {
        cerr << "NiftiImage: Cannot set the datatype of a file opened for reading." << endl;
        return;
    }
    DTMap::const_iterator it = DataTypes.find(dt);
    if (it == DataTypes.end())
        NIFTI_ERROR("Attempted to set invalid datatype.");
    else
        _datatype = it->second;
}

bool NiftiImage::voxelsCompatible(const NiftiImage &other) const
{
    if ((nx() == other.nx()) && (ny() == other.ny()) && (nz() == other.nz()) &&
        (dx() == other.dx()) && (dy() == other.dy()) && (dz() == other.dz()))
        // Voxel numbers and sizes match, can process on a volume basis
        return true;
    else
        return false;
}

void NiftiImage::checkVoxelsCompatible(const NiftiImage &other) const
{
    if (!voxelsCompatible(other)) {
        stringstream message;
        message << "voxels do not match." << endl
            << _basename << endl
            << "Image dims: " << nx() << " " << ny() << " " << nz()
            << " Voxel size: " << dx() << " " << dy() << " " << dz()
            << other._basename << endl
            << "Image dims: " << other.nx() << " " << other.ny() << " " << other.nz()
            << " Voxel size: " << other.dx() << " " << other.dy() << " " << other.dz();
        NIFTI_FAIL(message.str());
    }
}

bool NiftiImage::compatible(const NiftiImage &other) const
{
    if (voxelsCompatible(other) &&
        ((ijk_to_xyz() - other.ijk_to_xyz()).norm() < (numeric_limits<double>::epsilon() * ijk_to_xyz().size())))
        // Then we have the same number of voxels, dimensions are the same,
        // and get transformed to the same spatial locations.
        return true;
    else
        return false;
}

void NiftiImage::checkCompatible(const NiftiImage &other) const
{
    if (!compatible(other)) {
        stringstream message;
        message << "volumes do not match." << endl
            << _basename << endl
            << "Image dims: " << nx() << " " << ny() << " " << nz()
            << " Voxel size: " << dx() << " " << dy() << " " << dz()
            << " Transform:" << endl << ijk_to_xyz() << endl
            << other._basename << endl
            << "Image dims: " << other.nx() << " " << other.ny() << " " << other.nz()
            << " Voxel size: " << other.dx() << " " << other.dy() << " " << other.dz()
            << " Transform:" << endl << other.ijk_to_xyz() << endl
            ;
        NIFTI_FAIL(message.str());
    }
}

const Matrix4d &NiftiImage::qform() const { return _qform.matrix(); }
const Matrix4d &NiftiImage::sform() const { return _sform.matrix(); }
const Matrix4d &NiftiImage::ijk_to_xyz() const
{
    if ((sform_code > 0) && (sform_code >= qform_code))
        return _sform.matrix();
    else // There is always a _qform matrix
        return _qform.matrix();
}
const Matrix4d &NiftiImage::xyz_to_ijk() const
{
    static Matrix4d inverse;
    if ((sform_code > 0) && (sform_code >= qform_code))
        inverse = _sform.matrix().inverse();
    else // There is always a _qform matrix
        inverse = _qform.matrix().inverse();
    return inverse;
}

