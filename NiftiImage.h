/** \file NiftiImage.h
\brief Declaration for NiftiImage class
- Written by Tobias Wood, IoP KCL
- Based on nifti1_io.h (Thanks to Robert Cox et al)
- This code is released to the public domain. Do with it what you will.
*/
#ifndef NIFTI_IMAGE
#define NIFTI_IMAGE

#include <stdio.h>
#include <string.h>
#include <zlib.h>

#include <string>
#include <iostream>
#include <algorithm>
#include <complex>
#include <map>

#include <Eigen/Geometry>

#include "nifti1.h"                  /*** NIFTI-1 header specification ***/

using namespace std;
using namespace Eigen;

/*! \enum analyze_75_orient_code
*  \brief Old-style analyze75 orientation
*         codes.
*/
typedef enum _analyze75_orient_code {
    a75_transverse_unflipped = 0,
    a75_coronal_unflipped = 1,
    a75_sagittal_unflipped = 2,
    a75_transverse_flipped = 3,
    a75_coronal_flipped = 4,
    a75_sagittal_flipped = 5,
    a75_orient_unknown = 6
} analyze_75_orient_code;

/*****************************************************************************/
/*------------------ NIfTI version of ANALYZE 7.5 structure -----------------*/

/* (based on fsliolib/dbh.h, but updated for version 7.5) */

typedef struct {
    /* header info fields - describes the header    overlap with NIfTI */
    /*                                              ------------------ */
    int sizeof_hdr;                  /* 0 + 4        same              */
    char data_type[10];              /* 4 + 10       same              */
    char db_name[18];                /* 14 + 18      same              */
    int extents;                     /* 32 + 4       same              */
    short int session_error;         /* 36 + 2       same              */
    char regular;                    /* 38 + 1       same              */
    char hkey_un0;                   /* 39 + 1                40 bytes */

                                     /* image dimension fields - describes image sizes */
    short int dim[8];                /* 0 + 16       same              */
    short int unused8;               /* 16 + 2       intent_p1...      */
    short int unused9;               /* 18 + 2         ...             */
    short int unused10;              /* 20 + 2       intent_p2...      */
    short int unused11;              /* 22 + 2         ...             */
    short int unused12;              /* 24 + 2       intent_p3...      */
    short int unused13;              /* 26 + 2         ...             */
    short int unused14;              /* 28 + 2       intent_code       */
    short int datatype;              /* 30 + 2       same              */
    short int bitpix;                /* 32 + 2       same              */
    short int dim_un0;               /* 34 + 2       slice_start       */
    float pixdim[8];                 /* 36 + 32      same              */

    float vox_offset;                /* 68 + 4       same              */
    float funused1;                  /* 72 + 4       scl_slope         */
    float funused2;                  /* 76 + 4       scl_inter         */
    float funused3;                  /* 80 + 4       slice_end,        */
                                     /* slice_code,       */
                                     /* xyzt_units        */
    float cal_max;                   /* 84 + 4       same              */
    float cal_min;                   /* 88 + 4       same              */
    float compressed;                /* 92 + 4       slice_duration    */
    float verified;                  /* 96 + 4       toffset           */
    int glmax, glmin;                 /* 100 + 8              108 bytes */

                                      /* data history fields - optional */
    char descrip[80];                /* 0 + 80       same              */
    char aux_file[24];               /* 80 + 24      same              */
    char orient;                     /* 104 + 1      NO GOOD OVERLAP   */
    char originator[10];             /* 105 + 10     FROM HERE DOWN... */
    char generated[10];              /* 115 + 10                       */
    char scannum[10];                /* 125 + 10                       */
    char patient_id[10];             /* 135 + 10                       */
    char exp_date[10];               /* 145 + 10                       */
    char exp_time[10];               /* 155 + 10                       */
    char hist_un0[3];                /* 165 + 3                        */
    int views;                       /* 168 + 4                        */
    int vols_added;                  /* 172 + 4                        */
    int start_field;                 /* 176 + 4                        */
    int field_skip;                  /* 180 + 4                        */
    int omax, omin;                  /* 184 + 8                        */
    int smax, smin;                  /* 192 + 8              200 bytes */
} nifti_analyze75;                                   /* total:  348 bytes */


                                                     /*****************************************************************************/
                                                     /*--------------- Prototypes of functions defined in this file --------------*/

int    disp_nifti_1_header(const char * info, const nifti_1_header * hp);
void   nifti_set_skip_blank_ext(int skip);
void   nifti_set_allow_upper_fext(int allow);

/*--------------------- Low level IO routines ------------------------------*/
/* other routines */
int    nifti_hdr_looks_good(const nifti_1_header * hdr);
int    nifti_is_valid_ecode(int ecode);
/*int    nifti_add_extension(nifti_image * nim, const char * data, int len,
int ecode );
int    nifti_compiled_with_zlib    (void);
int    nifti_copy_extensions (nifti_image *nim_dest,const nifti_image *nim_src);
int    nifti_free_extensions (nifti_image *nim);
int  * nifti_get_intlist     (int nvals , const char *str);
char * nifti_strdup          (const char *str);
int    valid_nifti_extensions(const nifti_image *nim);*/


/*-------------------- Some C convenience macros ----------------------------*/

/* NIfTI-1.1 extension codes:
see http://nifti.nimh.nih.gov/nifti-1/documentation/faq#Q21 */

#define NIFTI_ECODE_IGNORE           0  /* changed from UNKNOWN, 29 June 2005 */

#define NIFTI_ECODE_DICOM            2  /* intended for raw DICOM attributes  */

#define NIFTI_ECODE_AFNI             4  /* Robert W Cox: rwcox@nih.gov
http://afni.nimh.nih.gov/afni      */

#define NIFTI_ECODE_COMMENT          6  /* plain ASCII text only              */

#define NIFTI_ECODE_XCEDE            8  /* David B Keator: dbkeator@uci.edu
http://www.nbirn.net/Resources
/Users/Applications/
/xcede/index.htm              */

#define NIFTI_ECODE_JIMDIMINFO      10  /* Mark A Horsfield:
mah5@leicester.ac.uk
http://someplace/something         */

#define NIFTI_ECODE_WORKFLOW_FWDS   12  /* Kate Fissell: fissell@pitt.edu
http://kraepelin.wpic.pitt.edu
/~fissell/NIFTI_ECODE_WORKFLOW_FWDS
/NIFTI_ECODE_WORKFLOW_FWDS.html   */

#define NIFTI_ECODE_FREESURFER      14  /* http://surfer.nmr.mgh.harvard.edu  */

#define NIFTI_ECODE_PYPICKLE        16  /* embedded Python objects
http://niftilib.sourceforge.net
/pynifti                     */

/* LONI MiND codes: http://www.loni.ucla.edu/twiki/bin/view/Main/MiND */
#define NIFTI_ECODE_MIND_IDENT      18  /* Vishal Patel: vishal.patel@ucla.edu*/
#define NIFTI_ECODE_B_VALUE         20
#define NIFTI_ECODE_SPHERICAL_DIRECTION 22
#define NIFTI_ECODE_DT_COMPONENT    24
#define NIFTI_ECODE_SHC_DEGREEORDER 26  /* end LONI MiND codes                */

#define NIFTI_ECODE_VOXBO           28  /* Dan Kimberg: www.voxbo.org         */

#define NIFTI_ECODE_CARET           30  /* John Harwell: john@brainvis.wustl.edu
http://brainvis.wustl.edu/wiki
/index.php/Caret:Documentation
:CaretNiftiExtension             */

#define NIFTI_MAX_ECODE             30  /******* maximum extension code *******/

#define LNI_MAX_NIA_EXT_LEN 100000  /* consider a longer extension invalid */

/*! NIfTI header class */

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define NIFTI_ERROR( err ) do { cerr << __PRETTY_FUNCTION__ << ": " << ( err ) << flush << endl; } while(0)
#define NIFTI_FAIL( err ) do { NIFTI_ERROR( err ); exit(EXIT_FAILURE); } while(0)

class NiftiImage
{
private:

    struct NiftiDataType {
        int code, size, swapsize;
        string name;
    };
    typedef map<int, NiftiDataType> DTMap;
    typedef map<int, string> StringMap;

    union ZFile {
        FILE *unzipped;
        gzFile zipped;
    };
    static const int MaxZippedBytes = 1 << 30;


    int _dim[8];               //!< Number of voxels = nx*ny*nz*...*nw
    float _voxdim[8];          //!< Dimensions of each voxel
    Affine3d _qform, _sform;   //!< Tranformation matrices from voxel indices to physical co-ords

    string _basename, _imgname, _hdrname; // Paths to header and image files
    int _voxoffset;
    NiftiDataType _datatype;                   // Datatype on disk
    int _num_ext;
    nifti1_extension *_ext_list;
    char _mode;
    bool _gz;
    int _swap;                                 // True if byte order on disk is different to CPU
    ZFile _file;

    void setFilenames(const string &filename);
    static int needs_swap(short dim0, int hdrsize);
    static float fixFloat(const float f);

    size_t read(void *buff, size_t size, size_t nmemb);
    size_t write(const void *buff, size_t size, size_t nmemb);
    long seek(long offset, int whence);
    int rewind();

    static void SwapBytes(size_t n, int siz, void *ar);
    static void SwapNiftiHeader(struct nifti_1_header *h);
    static void SwapAnalyzeHeader(nifti_analyze75 *h);

    void readHeader(string path);
    void writeHeader();
    char *readBytes(size_t start, size_t length, char *buffer = NULL);
    void writeBytes(char *buffer, size_t start, size_t length);

    /**
    *   Internal function to convert the internal NIfTI data to the desired dataype.
    *
    *   Converts a sequence of bytes from a NIfTI image to the templated datatype.
    *
    *   @param T Desired datatype. Valid (scalar) conversions must exist.
    *   @param bytes Pointer to the NIfTI data
    *   @param nEl Number of elements (not bytes) in the data
    *   @param data Pointer to the storage for the converted data. If NULL then
    *          storage is allocated internally and a pointer returned.
    *   @return Pointer to the converted data.
    */
    template<typename T> T* convertFromBytes(char *const bytes, const size_t nEl, T *data = NULL)
    {
        if (!data)
            data = new T[nEl];
        for (size_t i = 0; i < nEl; i++) {
            switch (_datatype.code) {
            case NIFTI_TYPE_INT8:      data[i] = static_cast<T>(reinterpret_cast<char *>(bytes)[i]); break;
            case NIFTI_TYPE_UINT8:     data[i] = static_cast<T>(reinterpret_cast<unsigned char *>(bytes)[i]); break;
            case NIFTI_TYPE_INT16:     data[i] = static_cast<T>(reinterpret_cast<short *>(bytes)[i]); break;
            case NIFTI_TYPE_UINT16:    data[i] = static_cast<T>(reinterpret_cast<unsigned short *>(bytes)[i]); break;
            case NIFTI_TYPE_INT32:     data[i] = static_cast<T>(reinterpret_cast<int *>(bytes)[i]); break;
            case NIFTI_TYPE_UINT32:    data[i] = static_cast<T>(reinterpret_cast<unsigned int *>(bytes)[i]); break;
            case NIFTI_TYPE_FLOAT32:   data[i] = static_cast<T>(reinterpret_cast<float *>(bytes)[i]); break;
            case NIFTI_TYPE_FLOAT64:   data[i] = static_cast<T>(reinterpret_cast<double *>(bytes)[i]); break;
            case NIFTI_TYPE_INT64:     data[i] = static_cast<T>(reinterpret_cast<long *>(bytes)[i]); break;
            case NIFTI_TYPE_UINT64:    data[i] = static_cast<T>(reinterpret_cast<unsigned long *>(bytes)[i]); break;
            case NIFTI_TYPE_FLOAT128:  data[i] = static_cast<T>(reinterpret_cast<long double *>(bytes)[i]); break;
                // NOTE: C++11 specifies that C++ 'complex<type>' and C 'type complex'
                // should be interchangeable even at pointer level
            case NIFTI_TYPE_COMPLEX64:  data[i] = static_cast<T>(abs(reinterpret_cast<complex<float> *>(bytes)[i])); break;
            case NIFTI_TYPE_COMPLEX128: data[i] = static_cast<T>(abs(reinterpret_cast<complex<double> *>(bytes)[i])); break;
            case NIFTI_TYPE_COMPLEX256: data[i] = static_cast<T>(abs(reinterpret_cast<complex<long double> *>(bytes)[i])); break;
            case NIFTI_TYPE_RGB24: case NIFTI_TYPE_RGBA32:
                NIFTI_FAIL("RGB/RGBA datatypes not supported.");
                break;
            }
        }
        return data;
    }

    template<typename T> complex<T> *convertFromBytes(char *const bytes, const size_t nEl, complex<T> *data)
    {
        if (!data)
            data = new complex<T>[nEl];
        for (int i = 0; i < nEl; i++) {
            switch (_datatype.code) {
            case NIFTI_TYPE_INT8:      data[i].real(static_cast<T>(reinterpret_cast<char *>(bytes)[i])); break;
            case NIFTI_TYPE_UINT8:     data[i].real(static_cast<T>(reinterpret_cast<unsigned char *>(bytes)[i])); break;
            case NIFTI_TYPE_INT16:     data[i].real(static_cast<T>(reinterpret_cast<short *>(bytes)[i])); break;
            case NIFTI_TYPE_UINT16:    data[i].real(static_cast<T>(reinterpret_cast<unsigned short *>(bytes)[i])); break;
            case NIFTI_TYPE_INT32:     data[i].real(static_cast<T>(reinterpret_cast<int *>(bytes)[i])); break;
            case NIFTI_TYPE_UINT32:    data[i].real(static_cast<T>(reinterpret_cast<unsigned int *>(bytes)[i])); break;
            case NIFTI_TYPE_FLOAT32:   data[i].real(static_cast<T>(reinterpret_cast<float *>(bytes)[i])); break;
            case NIFTI_TYPE_FLOAT64:   data[i].real(static_cast<T>(reinterpret_cast<double *>(bytes)[i])); break;
            case NIFTI_TYPE_INT64:     data[i].real(static_cast<T>(reinterpret_cast<long *>(bytes)[i])); break;
            case NIFTI_TYPE_UINT64:    data[i].real(static_cast<T>(reinterpret_cast<unsigned long *>(bytes)[i])); break;
            case NIFTI_TYPE_FLOAT128:  data[i].real(static_cast<T>(reinterpret_cast<long double *>(bytes)[i])); break;
            case NIFTI_TYPE_COMPLEX64:  data[i] = static_cast<complex<T> >(reinterpret_cast<complex<float> *>(bytes)[i]); break;
            case NIFTI_TYPE_COMPLEX128: data[i] = static_cast<complex<T> >(reinterpret_cast<complex<double> *>(bytes)[i]); break;
            case NIFTI_TYPE_COMPLEX256: data[i] = static_cast<complex<T> >(reinterpret_cast<complex<long double> *>(bytes)[i]); break;
            case NIFTI_TYPE_RGB24: case NIFTI_TYPE_RGBA32:
                NIFTI_FAIL("RGB/RGBA datatypes not supported.");
                break;
            }
        }
        return data;
    }

    /**
    *   Internal function to convert data to the internal NIfTI datatype.
    *
    *   Converts from the templated type to a sequence of bytes suitable for writing to a NIfTI image.
    *   @param data Pointer to the data.
    *   @param T Desired datatype. Valid (scalar) conversions must exist.
    *   @param nEl Number of elements (not bytes) in the data
    *   @param bytes Pointer to the NIfTI data. If NULL then storage is
    *          allocated internally and a pointer returned.
    *   @return Pointer to the data stored in a byte array.
    */
    template<typename T> char *convertToBytes(const T *const data, const size_t nEl, char *bytes = NULL)
    {
        if (!bytes)
            bytes = new char[nEl * _datatype.size];
        for (size_t i = 0; i < nEl; i++) {
            switch (_datatype.code) {
            case NIFTI_TYPE_INT8:              reinterpret_cast<char *>(bytes)[i] = static_cast<char>(data[i]); break;
            case NIFTI_TYPE_UINT8:    reinterpret_cast<unsigned char *>(bytes)[i] = static_cast<unsigned char>(data[i]); break;
            case NIFTI_TYPE_INT16:            reinterpret_cast<short *>(bytes)[i] = static_cast<short>(data[i]); break;
            case NIFTI_TYPE_UINT16:  reinterpret_cast<unsigned short *>(bytes)[i] = static_cast<unsigned short>(data[i]); break;
            case NIFTI_TYPE_INT32:              reinterpret_cast<int *>(bytes)[i] = static_cast<int>(data[i]); break;
            case NIFTI_TYPE_UINT32:    reinterpret_cast<unsigned int *>(bytes)[i] = static_cast<unsigned int>(data[i]); break;
            case NIFTI_TYPE_FLOAT32:          reinterpret_cast<float *>(bytes)[i] = static_cast<float>(data[i]); break;
            case NIFTI_TYPE_FLOAT64:         reinterpret_cast<double *>(bytes)[i] = static_cast<double>(data[i]); break;
            case NIFTI_TYPE_INT64:             reinterpret_cast<long *>(bytes)[i] = static_cast<long>(data[i]); break;
            case NIFTI_TYPE_UINT64:   reinterpret_cast<unsigned long *>(bytes)[i] = static_cast<unsigned long>(data[i]); break;
            case NIFTI_TYPE_FLOAT128:   reinterpret_cast<long double *>(bytes)[i] = static_cast<long double>(data[i]); break;
            case NIFTI_TYPE_COMPLEX64:  reinterpret_cast<complex<float> *>(bytes)[i].real(static_cast<float>(data[i])); break;
            case NIFTI_TYPE_COMPLEX128: reinterpret_cast<complex<double> *>(bytes)[i].real(static_cast<double>(data[i])); break;
            case NIFTI_TYPE_COMPLEX256: reinterpret_cast<complex<long double> *>(bytes)[i].real(static_cast<long double>(data[i])); break;
            case NIFTI_TYPE_RGB24: case NIFTI_TYPE_RGBA32:
                NIFTI_FAIL("RGB/RGBA datatypes not supported."); break;
            }
        }
        return bytes;
    }

    template<typename T> char *convertToBytes(const complex<T> *const data, const size_t nEl, char *bytes = NULL)
    {
        if (!bytes)
            bytes = new char[nEl * _datatype.size];
        for (int i = 0; i < nEl; i++) {
            switch (_datatype.code) {
            case NIFTI_TYPE_INT8:              reinterpret_cast<char *>(bytes)[i] = static_cast<char>(abs(data[i])); break;
            case NIFTI_TYPE_UINT8:    reinterpret_cast<unsigned char *>(bytes)[i] = static_cast<unsigned char>(abs(data[i])); break;
            case NIFTI_TYPE_INT16:            reinterpret_cast<short *>(bytes)[i] = static_cast<short>(abs(data[i])); break;
            case NIFTI_TYPE_UINT16:  reinterpret_cast<unsigned short *>(bytes)[i] = static_cast<unsigned short>(abs(data[i])); break;
            case NIFTI_TYPE_INT32:              reinterpret_cast<int *>(bytes)[i] = static_cast<int>(abs(data[i])); break;
            case NIFTI_TYPE_UINT32:    reinterpret_cast<unsigned int *>(bytes)[i] = static_cast<unsigned int>(abs(data[i])); break;
            case NIFTI_TYPE_FLOAT32:          reinterpret_cast<float *>(bytes)[i] = static_cast<float>(abs(data[i])); break;
            case NIFTI_TYPE_FLOAT64:         reinterpret_cast<double *>(bytes)[i] = static_cast<double>(abs(data[i])); break;
            case NIFTI_TYPE_INT64:             reinterpret_cast<long *>(bytes)[i] = static_cast<long>(abs(data[i])); break;
            case NIFTI_TYPE_UINT64:   reinterpret_cast<unsigned long *>(bytes)[i] = static_cast<unsigned long>(abs(data[i])); break;
            case NIFTI_TYPE_FLOAT128:   reinterpret_cast<long double *>(bytes)[i] = static_cast<long double>(abs(data[i])); break;
            case NIFTI_TYPE_COMPLEX64:  reinterpret_cast<complex<float> *>(bytes)[i] = static_cast<complex<float> >(data[i]); break;
            case NIFTI_TYPE_COMPLEX128: reinterpret_cast<complex<double> *>(bytes)[i] = static_cast<complex<double> >(data[i]); break;
            case NIFTI_TYPE_COMPLEX256: reinterpret_cast<complex<long double> *>(bytes)[i] = static_cast<complex<long double> >(data[i]); break;
            case NIFTI_TYPE_RGB24: case NIFTI_TYPE_RGBA32:
                NIFTI_FAIL("RGB/RGBA datatypes not supported."); break;
            }
        }
        return bytes;
    }

public:
    /*
    *  Used when opening a NiftiImage to specify read or write. NiftiImages
    *  can only be open for either reading or writing at any one time, and
    *  must be closed before re-opening.
    */
    enum NIFTIIMAGE_MODES
    {
        NIFTI_CLOSED = 0,
        NIFTI_READ = 'r',
        NIFTI_WRITE = 'w'
    };

    ~NiftiImage();
    NiftiImage();
    NiftiImage(const NiftiImage &clone);
    NiftiImage(const int nx, const int ny, const int nz, const int nt,
        const float dx, const float dy, const float dz, const float dt,
        const int datatype);
    NiftiImage(const string &filename, const char &mode);
    NiftiImage &operator=(const NiftiImage &other);
    static void printDTypeList();

    bool open(const string &filename, const char &mode);
    void close();
    const string &basename();
    char *readRawVolume(const int vol);
    char *readRawAllVolumes();

    int ndim() const;
    int nx() const;
    int ny() const;
    int nz() const;
    int nt() const;
    int voxelsPerSlice() const;
    int voxelsPerVolume() const;
    int voxelsTotal() const;
    void setnt(const int nt);
    void setDims(int nx, int ny, int nz, int nt);

    int datatype() const;
    void setDatatype(const int dt);
    bool compatible(const NiftiImage &other) const; //!< Check if voxel dimensions, data size and transform match
    bool voxelsCompatible(const NiftiImage &other) const; //!< Looser check if voxel dimensions and data size match
    void checkVoxelsCompatible(const NiftiImage &other) const; //!< Exits program if voxels differ
    void checkCompatible(const NiftiImage &other) const; //!< Exits program if headers do not match
    float dx() const;
    float dy() const;
    float dz() const;
    float dt() const;
    void setVoxDims(double dx, double dy, double dz, double dt);

    float scaling_slope;
    float scaling_inter;
    float calibration_min;
    float calibration_max;

    int qform_code;
    int sform_code;
    const Matrix4d &qform() const;
    const Matrix4d &sform() const;
    void set_qform(const Matrix4d& new_qform);
    void set_qform(const Affine3d &R, const Affine3d &T);
    void set_qform(const double b, const double c, const double d, const double x, const double y, const double z);
    void set_sform(const Matrix4d& new_sform);
    const Matrix4d &ijk_to_xyz() const;
    const Matrix4d &xyz_to_ijk() const;

    int freq_dim;                //!< indexes (1,2,3, or 0) for MRI
    int phase_dim;                //!< directions in dim[]/pixdim[]
    int slice_dim;                //!< directions in dim[]/pixdim[]

    int   slice_code;             //!< code for slice timing pattern
    int   slice_start;            //!< index for start of slices
    int   slice_end;              //!< index for end of slices
    float slice_duration;         //!< time between individual slices
    float toffset;                //!< time coordinate offset

    int xyz_units;                //!< dx,dy,dz units: NIFTI_UNITS_* code
    int time_units;               //!< dt       units: NIFTI_UNITS_* code
    int   intent_code;           //!< statistic type (or something)
    float intent_p1;             //!< intent parameters
    float intent_p2;             //!< intent parameters
    float intent_p3;             //!< intent parameters
    string intent_name;      //!< optional description of intent data
    string description;      //!< optional text to describe dataset
    string aux_file;         //!< auxiliary filename

    int                num_ext;  //!< number of extensions in ext_list
    nifti1_extension * ext_list; //!< array of extension structs (with data)
    analyze_75_orient_code analyze75_orient; //!< for old analyze files, orient

    const string &dtypeName() const;
    const string &spaceUnits() const;
    const string &timeUnits() const;
    const string &intentName() const;
    const string &transformName() const;
    const string &sliceName() const;

    template<typename T> T *readVolume(const int &vol, T *converted = NULL)
    {
        size_t bytesPerVolume = voxelsPerVolume() * _datatype.size;
        char *raw = readBytes(vol * bytesPerVolume, bytesPerVolume);
        converted = convertFromBytes<T>(raw, voxelsPerVolume(), converted);
        delete[] raw;
        return converted;
    }

    template<typename T> T *readAllVolumes()
    {
        char *raw = readBytes(0, voxelsTotal() * _datatype.size);
        T *converted = convertFromBytes<T>(raw, voxelsTotal());
        delete[] raw;
        return converted;
    }

    template<typename T> T *readSubvolume(const int &sx, const int &sy, const int &sz, const int &st,
        const int &ex, const int &ey, const int &ez, const int &et,
        T *converted = NULL)
    {
        size_t lx, ly, lz, lt, total, toRead;
        lx = ((ex == -1) ? nx() : ex) - sx;
        ly = ((ey == -1) ? ny() : ey) - sy;
        lz = ((ez == -1) ? nz() : ez) - sz;
        lt = ((et == -1) ? nt() : et) - st;
        total = lx * ly * lz * lt;

        // Collapse successive full dimensions into a single compressed read
        toRead = lx * _datatype.size;
        if (lx == nx()) {
            toRead *= ly;
            if (ly == ny()) {
                toRead *= lz;
                if (lz == nz()) {
                    // If we've got to here we're actual reading the whole image
                    toRead *= lt;
                    lt = 1;
                }
                lz = 1;
            }
            ly = 1;
        }

        char *raw = new char[total * _datatype.size];
        char *nextRead = raw;
        for (int t = st; t < st + lt; t++)
        {
            size_t tOff = t * voxelsPerVolume();
            for (int z = sz; z < sz + lz; z++)
            {
                size_t zOff = z * voxelsPerSlice();
                for (int y = sy; y < sy + ly; y++)
                {
                    size_t yOff = y * nx();
                    if (readBytes((tOff + zOff + yOff) * _datatype.size, toRead, nextRead))
                        nextRead += toRead;
                    else
                        NIFTI_FAIL("failed to read subvolume from file.");
                }
            }
        }
        converted = convertFromBytes<T>(raw, total, converted);
        delete[] raw;
        return converted;
    }

    template<typename T> void writeVolume(const int vol, const T *data)
    {
        size_t bytesPerVolume = voxelsPerVolume() * _datatype.size;
        char *converted = convertToBytes(data, voxelsPerVolume());
        writeBytes(converted, vol * bytesPerVolume, bytesPerVolume);
        delete[] converted;
    }

    template<typename T> void writeAllVolumes(const T *data)
    {
        char *converted = convertToBytes(data, voxelsTotal());
        writeBytes(converted, 0, voxelsTotal() * _datatype.size);
        delete[] converted;
    }

    template<typename T> void writeSubvolume(const int &sx, const int &sy, const int &sz, const int &st,
        const int &ex, const int &ey, const int &ez, const int &et,
        const T *data)
    {
        size_t lx, ly, lz, lt, total, toWrite;
        lx = ((ex == -1) ? nx() : ex) - sx;
        ly = ((ey == -1) ? ny() : ey) - sy;
        lz = ((ez == -1) ? nz() : ez) - sz;
        lt = ((et == -1) ? nt() : et) - st;
        total = lx * ly * lz * lt;

        // Collapse successive full dimensions into a single write
        toWrite = lx * _datatype.size;
        if (lx == nx()) {
            toWrite *= ly;
            if (ly == ny()) {
                toWrite *= lz;
                if (lz == nz()) {
                    // If we've got to here we're actual writing the whole image
                    toWrite *= lt;
                    lt = 1;
                }
                lz = 1;
            }
            ly = 1;
        }

        char *raw = convertToBytes(data, total);
        char *nextWrite = raw;
        for (int t = st; t < st + lt; t++)
        {
            size_t tOff = t * voxelsPerVolume();
            for (int z = sz; z < sz + lz; z++)
            {
                size_t zOff = z * voxelsPerSlice();
                for (int y = sy; y < sy + ly; y++)
                {
                    size_t yOff = y * nx();
                    writeBytes(nextWrite, (tOff + zOff + yOff) * _datatype.size, toWrite);
                    nextWrite += toWrite;
                }
            }
        }
        delete[] raw;
    }
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif /* _NIFTI_IO_HEADER_ */
