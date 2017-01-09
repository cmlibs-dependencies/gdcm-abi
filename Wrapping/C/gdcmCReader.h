/*
 * gdcmCReader.h
 *
 *  Created on: 3/07/2008
 *      Author: blackett
 */

#ifndef GDCMCREADER_H_
#define GDCMCREADER_H_

#include "gdcmConfigure.h"

#ifdef GDCM_HAVE_STDINT_H
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif // __STDC_LIMIT_MACROS
#include <stdint.h>
//#undef __STDC_LIMIT_MACROS
#else
#ifdef GDCM_HAVE_INTTYPES_H
// Old system only have this
#include <inttypes.h>   // For uint8_t uint16_t and uint32_t
#else
// Broken plateforms do not respect C99 and do not provide those typedef
// Special case for recent Borland compiler, comes with stdint.h
#if defined(__BORLANDC__) && (__BORLANDC__ < 0x0560) || defined(__MINGW32__)
typedef  signed char         int8_t;
typedef  signed short        int16_t;
typedef  signed int          int32_t;
typedef  unsigned char       uint8_t;
typedef  unsigned short      uint16_t;
typedef  unsigned int        uint32_t;
typedef  unsigned __int64    uint64_t;
#elif defined(_MSC_VER)
#include "stdint.h"
#else
#error "Sorry your platform is not supported"
#endif // defined(_MSC_VER) || defined(__BORLANDC__) && (__BORLANDC__ < 0x0560)  || defined(__MINGW32__)
#endif // GDCM_HAVE_INTTYPES_H
#endif // GDCM_HAVE_STDINT_H

typedef void *gdcmCReaderClientDataHandle;
typedef void *gdcmCReaderDataHandle;
typedef unsigned int gdcmCReaderDataLength;
typedef int gdcmCReaderDataOffset;
enum gdcmCReaderDataDirection
{
	GDCMCREADERSEEKBEGINNING,
	GDCMCREADERSEEKCURRENT,
	GDCMCREADERSEEKEND
};

typedef	gdcmCReaderDataLength (*gdcmCReaderClientDataReadFunction)(
	gdcmCReaderClientDataHandle, gdcmCReaderDataHandle, gdcmCReaderDataLength);
typedef	gdcmCReaderDataOffset (*gdcmCReaderClientDataSeekFunction)(
		gdcmCReaderClientDataHandle, gdcmCReaderDataOffset, enum gdcmCReaderDataDirection);
typedef	void (*gdcmCReaderClientDataCloseFunction)(gdcmCReaderClientDataHandle);

struct gdcmCReader;

struct gdcmCImageReader;

struct gdcmCFile;

struct gdcmCDataSet;

struct gdcmCDataElement;

struct gdcmCByteValue;

struct gdcmCTag;

struct gdcmCImage;

struct gdcmCPixelFormat;

struct gdcmCDicts;

struct gdcmCDictEntry;

typedef uint32_t gdcmCVL;

struct gdcmCVR;

struct gdcmCReader *gdcmCReaderCreate(void);

enum gdcmCScalarType
{
    GDCMCSCALARTYPE_UINT8,
    GDCMCSCALARTYPE_INT8,
    GDCMCSCALARTYPE_UINT12,
    GDCMCSCALARTYPE_INT12,
    GDCMCSCALARTYPE_UINT16,
    GDCMCSCALARTYPE_INT16,
    GDCMCSCALARTYPE_UINT32,  // For some DICOM files (RT or SC)
    GDCMCSCALARTYPE_INT32,   //                        "   "
    GDCMCSCALARTYPE_FLOAT16, // sure why not...
    GDCMCSCALARTYPE_FLOAT32, // good ol' 'float'
    GDCMCSCALARTYPE_FLOAT64, // aka 'double'
    GDCMCSCALARTYPE_UNKNOWN // aka BitsAllocated == 0 && PixelRepresentation == 0
};

enum gdcmCVRType
{
   GDCMCVRTYPE_INVALID = 0, // For Item/(Seq) Item Delimitation Item
   GDCMCVRTYPE_AE = 1,
   GDCMCVRTYPE_AS = 2,
   GDCMCVRTYPE_AT = 4,
   GDCMCVRTYPE_CS = 8,
   GDCMCVRTYPE_DA = 16,
   GDCMCVRTYPE_DS = 32,
   GDCMCVRTYPE_DT = 64,
   GDCMCVRTYPE_FD = 128,
   GDCMCVRTYPE_FL = 256,
   GDCMCVRTYPE_IS = 512,
   GDCMCVRTYPE_LO = 1024,
   GDCMCVRTYPE_LT = 2048,
   GDCMCVRTYPE_OB = 4096,
   GDCMCVRTYPE_OF = 8192,
   GDCMCVRTYPE_OW = 16384,
   GDCMCVRTYPE_PN = 32768,
   GDCMCVRTYPE_SH = 65536,
   GDCMCVRTYPE_SL = 131072,
   GDCMCVRTYPE_SQ = 262144,
   GDCMCVRTYPE_SS = 524288,
   GDCMCVRTYPE_ST = 1048576,
   GDCMCVRTYPE_TM = 2097152,
   GDCMCVRTYPE_UI = 4194304,
   GDCMCVRTYPE_UL = 8388608,
   GDCMCVRTYPE_UN = 16777216,
   GDCMCVRTYPE_US = 33554432,
   GDCMCVRTYPE_UT = 67108864,
   GDCMCVRTYPE_OB_OW = GDCMCVRTYPE_OB | GDCMCVRTYPE_OW,
   GDCMCVRTYPE_US_SS = GDCMCVRTYPE_US | GDCMCVRTYPE_SS,
   GDCMCVRTYPE_US_SS_OW = GDCMCVRTYPE_US | GDCMCVRTYPE_SS | GDCMCVRTYPE_OW,
   // The following do not have a VRString equivalent (ie cannot be found in PS 3.6)
   GDCMCVRTYPE_VL16 = GDCMCVRTYPE_AE | GDCMCVRTYPE_AS | GDCMCVRTYPE_AT | GDCMCVRTYPE_CS | GDCMCVRTYPE_DA | GDCMCVRTYPE_DS | GDCMCVRTYPE_DT | GDCMCVRTYPE_FD | GDCMCVRTYPE_FL | GDCMCVRTYPE_IS | GDCMCVRTYPE_LO | GDCMCVRTYPE_LT | GDCMCVRTYPE_PN | GDCMCVRTYPE_SH | GDCMCVRTYPE_SL | GDCMCVRTYPE_SS | GDCMCVRTYPE_ST | GDCMCVRTYPE_TM | GDCMCVRTYPE_UI | GDCMCVRTYPE_UL | GDCMCVRTYPE_US, // if( VR & VL16 ) => VR has its VL coded over 16bits
   GDCMCVRTYPE_VL32 = GDCMCVRTYPE_OB | GDCMCVRTYPE_OW | GDCMCVRTYPE_OF | GDCMCVRTYPE_SQ | GDCMCVRTYPE_UN | GDCMCVRTYPE_UT, // if( VR & VR_VL32 ) => VR has its VL coded over 32bits
   GDCMCVRTYPE_VRASCII = GDCMCVRTYPE_AE | GDCMCVRTYPE_AS | GDCMCVRTYPE_CS | GDCMCVRTYPE_DA | GDCMCVRTYPE_DS | GDCMCVRTYPE_DT | GDCMCVRTYPE_IS | GDCMCVRTYPE_LO | GDCMCVRTYPE_LT | GDCMCVRTYPE_PN | GDCMCVRTYPE_SH | GDCMCVRTYPE_ST | GDCMCVRTYPE_TM | GDCMCVRTYPE_UI | GDCMCVRTYPE_UT,
   GDCMCVRTYPE_VRBINARY = GDCMCVRTYPE_AT | GDCMCVRTYPE_FL | GDCMCVRTYPE_FD | GDCMCVRTYPE_OB | GDCMCVRTYPE_OF | GDCMCVRTYPE_OW | GDCMCVRTYPE_SL | GDCMCVRTYPE_SQ | GDCMCVRTYPE_SS | GDCMCVRTYPE_UL | GDCMCVRTYPE_UN | GDCMCVRTYPE_US, // FIXME: UN ?
   // PS 3.5:
   // Data Elements with a VR of SQ, OF, OW, OB or UN shall always have a Value Multiplicity of one.
   // GDCM is adding a couple more: AS, LT, ST, UT
   GDCMCVRTYPE_VR_VM1 = GDCMCVRTYPE_AS | GDCMCVRTYPE_LT | GDCMCVRTYPE_ST | GDCMCVRTYPE_UT | GDCMCVRTYPE_SQ | GDCMCVRTYPE_OF | GDCMCVRTYPE_OW | GDCMCVRTYPE_OB | GDCMCVRTYPE_UN, // All those VR have a VM1
   GDCMCVRTYPE_VRALL = GDCMCVRTYPE_VRASCII | GDCMCVRTYPE_VRBINARY,
   GDCMCVRTYPE_VR_END = GDCMCVRTYPE_UT+1  // Invalid VR, need to be max(VRType)+1
};

void gdcmCReaderDestroy(struct gdcmCReader *reader);

int gdcmCReaderRead(struct gdcmCReader *reader);

void gdcmCReaderSetFilename(struct gdcmCReader *reader,
	const char *filename);

void gdcmCReaderSetClientStreamFunctions(struct gdcmCImageReader *reader,
	gdcmCReaderClientDataHandle user_data,
	gdcmCReaderClientDataReadFunction read_function,
	gdcmCReaderClientDataSeekFunction seek_function,
	gdcmCReaderClientDataCloseFunction close_function);

struct gdcmCImageReader *gdcmCImageReaderCreate(void);

void gdcmCImageReaderDestroy(struct gdcmCImageReader *reader);

int gdcmCImageReaderRead(struct gdcmCImageReader *reader);

void gdcmCImageReaderSetFilename(struct gdcmCImageReader *reader,
	const char *filename);

void gdcmCImageReaderSetClientStreamFunctions(struct gdcmCImageReader *reader,
	gdcmCReaderClientDataHandle user_data,
	gdcmCReaderClientDataReadFunction read_function,
	gdcmCReaderClientDataSeekFunction seek_function,
	gdcmCReaderClientDataCloseFunction close_function);

struct gdcmCFile *gdcmCImageReaderGetFile(struct gdcmCImageReader *reader);

struct gdcmCDataSet *gdcmCFileGetDataSet(struct gdcmCFile *file);

struct gdcmCDataElement *gdcmCDataSetFindNextDataElement(
	struct gdcmCDataSet *dataset, struct gdcmCTag *tag);

char *gdcmCDataSetGetPrivateCreator(
		struct gdcmCDataSet *dataset, struct gdcmCTag *tag);

struct gdcmCTag *gdcmCTagCreate(uint16_t group, uint16_t element);

uint16_t gdcmCTagGetGroup(struct gdcmCTag *tag);

uint16_t gdcmCTagGetElement(struct gdcmCTag *tag);

int gdcmCTagIsPrivate(struct gdcmCTag *tag);

int gdcmCTagIsPrivateCreator(struct gdcmCTag *tag);

void gdcmCTagDestroy(struct gdcmCTag *tag);

struct gdcmCTag *gdcmCDataElementGetTag(struct gdcmCDataElement *dataelement);

gdcmCVL gdcmCDataElementGetVL(struct gdcmCDataElement *dataelement);

struct gdcmCByteValue *gdcmCDataElementGetByteValue(struct gdcmCDataElement *dataelement);

char *gdcmCByteValuePrintASCII(struct gdcmCByteValue *bytevalue);

char *gdcmCByteValuePrintHex(struct gdcmCByteValue *bytevalue);

void gdcmCDeleteString(char **string);

int gdcmCByteValueGetBuffer(struct gdcmCByteValue *bytevalue, char *buffer, unsigned long length);

struct gdcmCVR *gdcmCDataElementGetVR(struct gdcmCDataElement *dataelement);

enum gdcmCVRType gdcmCVRGetType(struct gdcmCVR *vr);

struct gdcmCDicts *gdcmCGlobalInstanceGetDicts(void);

struct gdcmCDictEntry *gdcmCDictsGetDictEntry(struct gdcmCDicts *dicts,
	struct gdcmCTag *tag, char *owner);

const char *gdcmCDictEntryGetName(struct gdcmCDictEntry *dictentry);

struct gdcmCVR *gdcmCDictEntryGetVR(struct gdcmCDictEntry *dictentry);

struct gdcmCImage *gdcmCImageReaderGetImage(struct gdcmCImageReader *reader);

unsigned int gdcmCImageGetNumberOfDimensions(struct gdcmCImage *cimage);

unsigned int gdcmCImageGetDimension(struct gdcmCImage *cimage,
	unsigned int dimension_index);

unsigned long gdcmCImageGetBufferLength(struct gdcmCImage *cimage);
 
int gdcmCImageGetBuffer(struct gdcmCImage *cimage,
	char *data);

struct gdcmCPixelFormat *gdcmCImageGetPixelFormat(struct gdcmCImage *image);

unsigned short gdcmCPixelFormatGetSamplesPerPixel(struct gdcmCPixelFormat *pixelformat);

enum gdcmCScalarType gdcmCPixelFormatGetScalarType(struct gdcmCPixelFormat *pixelformat);

unsigned short gdcmCPixelFormatGetHighBit(struct gdcmCPixelFormat *pixelformat);

unsigned short gdcmCPixelFormatGetBitsAllocated(struct gdcmCPixelFormat *pixelformat);

unsigned short gdcmCPixelFormatGetBitsStored(struct gdcmCPixelFormat *pixelformat);

#endif /* GDCMCREADER_H_ */
