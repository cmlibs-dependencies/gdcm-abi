/*
 * gdcmCReader.cxx
 *
 *  Created on: 3/07/2008
 *      Author: blackett
 */

extern "C"
{
#include "gdcmCReader.h"
}
#include "gdcmReader.h"
#include "gdcmImageReader.h"
#include "gdcmImage.h"
#include "gdcmDict.h"
#include "gdcmDicts.h"
#include "gdcmGroupDict.h"
#include "gdcmVR.h"
#include "gdcmVM.h"
#include "gdcmElement.h"
#include "gdcmGlobal.h"
#include <streambuf>


class gdcmCMembuf : public std::streambuf
{
public:
	gdcmCMembuf(gdcmCReaderClientDataHandle user_data,
		gdcmCReaderClientDataReadFunction read_function,
		gdcmCReaderClientDataSeekFunction seek_function,
		gdcmCReaderClientDataCloseFunction close_function)
	: user_data(user_data), read_function(read_function),
	seek_function(seek_function), close_function(close_function)
	{
		input_buffer = new char[buffer_size + 10];
		setg(0,0,0);
		total_read = 0;
	}
	
	~gdcmCMembuf()
	{
		close_function(user_data);
		delete [] input_buffer;
	}
	
	int underflow()
	{
		int putback_allowed = 
			egptr() - eback();
		if (putback_allowed > putback_size)
			putback_allowed = putback_size;
		if (putback_allowed)
		{
			memmove(egptr() - putback_allowed, input_buffer, putback_allowed);
		}
		int read_count = read_function(user_data,
			input_buffer + putback_allowed, buffer_size - putback_allowed);
		if (read_count > 0)
		{
			total_read += read_count;
			setg(input_buffer, input_buffer + putback_allowed,
				input_buffer + putback_allowed + read_count);
			return traits_type::to_int_type(*gptr());
		}
		else
		{
			return traits_type::eof();
		}
	}

	std::streampos seekpos(std::streampos pos, std::ios_base::openmode mode)
	{
		return(seekoff(pos, std::ios_base::beg, mode));
	}

	std::streampos seekoff(std::streamoff off, std::ios_base::seekdir dir,
			std::ios_base::openmode mode)
	{
		char *p;
		char *ebackp = eback();
		char *egptrp = egptr();
		char *gptrp = gptr();
		switch (dir)
		{
		case std::ios_base::beg:
			p = egptr() + off - total_read;
			break;
		case std::ios_base::cur:
			p = gptr() + off;
			break;
		default:
			p = 0;
			break;
		}
		if (p >= eback() && p < egptr())
		{
			setg(eback(), p, egptr());
			return std::streampos(p + total_read - egptr());
		}
		else
			return -1;
	}

private:
	char *input_buffer;
	static const unsigned int buffer_size = 2048;
	static const unsigned int putback_size = 512;
	
	/* The total number of characters read so far. */
	unsigned long total_read;
	
	gdcmCReaderClientDataHandle user_data;
	gdcmCReaderClientDataReadFunction read_function;
	gdcmCReaderClientDataSeekFunction seek_function;
	gdcmCReaderClientDataCloseFunction close_function;
};

struct gdcmCReader
{
	gdcm::Reader *reader;
	gdcmCMembuf *membuf;
	std::istream *stream;
};

gdcmCReader *gdcmCReaderCreate()
{
	gdcmCReader *creader = new gdcmCReader;
	creader->reader = new gdcm::Reader();
	creader->membuf = NULL;
	return (creader);
}

void gdcmCReaderDestroy(gdcmCReader *creader)
{
	if (creader->stream)
	{
		delete creader->stream;
	}
	if (creader->membuf)
	{
		delete creader->membuf;
	}
	delete creader->reader;
	delete creader;
}

int gdcmCReaderRead(struct gdcmCReader *creader)
{
	bool result;
	result = creader->reader->Read();
	return (true == result ? 1 : 0);
}

void gdcmCReaderSetFileName(struct gdcmCReader *creader,
	const char *filename)
{
	creader->reader->SetFileName(filename);
}

void gdcmCReaderSetClientStreamFunctions(struct gdcmCReader *creader,
	gdcmCReaderClientDataHandle user_data,
	gdcmCReaderClientDataReadFunction read_function,
	gdcmCReaderClientDataSeekFunction seek_function,
	gdcmCReaderClientDataCloseFunction close_function)
{
	creader->membuf = new gdcmCMembuf(user_data,
		read_function, seek_function, close_function);
	creader->stream = new std::istream(creader->membuf);
	creader->reader->SetStream(*creader->stream);
}

struct gdcmCImageReader
{
	gdcm::ImageReader *reader;
	gdcmCMembuf *membuf;
	std::istream *stream;
};

gdcmCImageReader *gdcmCImageReaderCreate()
{
	gdcmCImageReader *creader = new gdcmCImageReader;
	creader->reader = new gdcm::ImageReader();
	creader->membuf = NULL;
	return (creader);
}

void gdcmCImageReaderDestroy(gdcmCImageReader *creader)
{
	if (creader->stream)
	{
		delete creader->stream;
	}
	if (creader->membuf)
	{
		delete creader->membuf;
	}
	delete creader->reader;
	delete creader;
}

int gdcmCImageReaderRead(struct gdcmCImageReader *creader)
{
	bool result;
	result = creader->reader->Read();
	return (true == result ? 1 : 0);
}

void gdcmCImageReaderSetFileName(struct gdcmCImageReader *creader,
	const char *filename)
{
	creader->reader->SetFileName(filename);
}

void gdcmCImageReaderSetClientStreamFunctions(struct gdcmCImageReader *creader,
	gdcmCReaderClientDataHandle user_data,
	gdcmCReaderClientDataReadFunction read_function,
	gdcmCReaderClientDataSeekFunction seek_function,
	gdcmCReaderClientDataCloseFunction close_function)
{
	creader->membuf = new gdcmCMembuf(user_data,
		read_function, seek_function, close_function);
	creader->stream = new std::istream(creader->membuf);
	creader->reader->SetStream(*creader->stream);
}

struct gdcmCImage *gdcmCImageReaderGetImage(struct gdcmCImageReader *creader)
{
	return((gdcmCImage*)&creader->reader->GetImage());
}

struct gdcmCFile *gdcmCImageReaderGetFile(struct gdcmCImageReader *reader)
{
	return((gdcmCFile*)&reader->reader->GetFile());
}

struct gdcmCDataSet *gdcmCFileGetDataSet(struct gdcmCFile *file)
{
	gdcm::File *cxxfile = (gdcm::File *)file;
	return((gdcmCDataSet *)&cxxfile->GetDataSet());
}

struct gdcmCDataElement *gdcmCDataSetFindNextDataElement(
	struct gdcmCDataSet *dataset, struct gdcmCTag *tag)
{
	gdcm::DataSet *cxxdataset = (gdcm::DataSet *)dataset;
	gdcm::Tag *cxxtag = (gdcm::Tag *)tag;
	return((gdcmCDataElement *)&cxxdataset->FindNextDataElement(*cxxtag));
}

char *gdcmCDataSetGetPrivateCreator(
		struct gdcmCDataSet *dataset, struct gdcmCTag *tag)
{
	gdcm::DataSet *cxxdataset = (gdcm::DataSet *)dataset;
	gdcm::Tag *cxxtag = (gdcm::Tag *)tag;
	const std::string &string = cxxdataset->GetPrivateCreator(*cxxtag);
	char *return_string = new char [string.length() + 1];
	memcpy(return_string, string.c_str(), string.length() + 1);
	return return_string;
}

struct gdcmCTag *gdcmCTagCreate(uint16_t group, uint16_t element)
{
	return((gdcmCTag *) new gdcm::Tag(group, element));
}

uint16_t gdcmCTagGetGroup(struct gdcmCTag *tag)
{
	gdcm::Tag *cxxtag = (gdcm::Tag *)tag;
	return(cxxtag->GetGroup());
}

uint16_t gdcmCTagGetElement(struct gdcmCTag *tag)
{
	gdcm::Tag *cxxtag = (gdcm::Tag *)tag;
	return(cxxtag->GetElement());
}

int gdcmCTagIsPrivate(struct gdcmCTag *tag)
{
	gdcm::Tag *cxxtag = (gdcm::Tag *)tag;
	return(cxxtag->IsPrivate());
}

int gdcmCTagIsPrivateCreator(struct gdcmCTag *tag)
{
	gdcm::Tag *cxxtag = (gdcm::Tag *)tag;
	return(cxxtag->IsPrivateCreator());
}

void gdcmCTagDestroy(struct gdcmCTag *tag)
{
	gdcm::Tag *cxxtag = (gdcm::Tag *)tag;
	delete (cxxtag);
}

struct gdcmCTag *gdcmCDataElementGetTag(struct gdcmCDataElement *dataelement)
{
	gdcm::DataElement *cxxdataelement = (gdcm::DataElement *)dataelement;
	return((gdcmCTag *)&cxxdataelement->GetTag());	
}

gdcmCVL gdcmCDataElementGetVL(struct gdcmCDataElement *dataelement)
{
	gdcm::DataElement *cxxdataelement = (gdcm::DataElement *)dataelement;
	return((gdcmCVL)cxxdataelement->GetVL());	
}

struct gdcmCByteValue *gdcmCDataElementGetByteValue(struct gdcmCDataElement *dataelement)
{
	gdcm::DataElement *cxxdataelement = (gdcm::DataElement *)dataelement;
	return((gdcmCByteValue *)cxxdataelement->GetByteValue());	
}

char *gdcmCByteValuePrintASCII(struct gdcmCByteValue *bytevalue)
{
	gdcm::ByteValue *cxxbytevalue = (gdcm::ByteValue *)bytevalue;
	std::ostringstream outputstring;
	gdcm::VL MaxPrintLength = 0x100; // Need to be %2 
	gdcm::VL l = std::min( cxxbytevalue->GetLength(), MaxPrintLength );
	cxxbytevalue->PrintASCII(outputstring, l);
	char *return_string = new char [outputstring.str().length() + 1];
	memcpy(return_string, outputstring.str().c_str(), outputstring.str().length() + 1);
	return return_string;
}

char *gdcmCByteValuePrintHex(struct gdcmCByteValue *bytevalue)
{
	gdcm::ByteValue *cxxbytevalue = (gdcm::ByteValue *)bytevalue;
	std::ostringstream outputstring;
	gdcm::VL MaxPrintLength = 0x100; // Need to be %2 
	gdcm::VL l = std::min( cxxbytevalue->GetLength(), MaxPrintLength );
	cxxbytevalue->PrintHex(outputstring, l);
	char *return_string = new char [outputstring.str().length() + 1];
	memcpy(return_string, outputstring.str().c_str(), outputstring.str().length() + 1);
	return return_string;
}

void gdcmCDeleteString(char **string)
{
	delete [] *string;
	string = NULL;
}

int gdcmCByteValueGetBuffer(struct gdcmCByteValue *bytevalue, char *buffer, unsigned long length)
{
	gdcm::ByteValue *cxxbytevalue = (gdcm::ByteValue *)bytevalue;
	return ((int)cxxbytevalue->GetBuffer(buffer, length));
}

struct gdcmCVR *gdcmCDataElementGetVR(struct gdcmCDataElement *dataelement)
{
	gdcm::DataElement *cxxdataelement = (gdcm::DataElement *)dataelement;
	return((gdcmCVR *)&cxxdataelement->GetVR());	
}

enum gdcmCVRType gdcmCVRGetType(struct gdcmCVR *vr)
{
	gdcm::VR *cxxvr = (gdcm::VR *)vr;
	const gdcm::VR::VRType type = *cxxvr;
	return ((enum gdcmCVRType )type);
}

struct gdcmCDicts *gdcmCGlobalInstanceGetDicts(void)
{
	const gdcm::Global& g = gdcm::GlobalInstance;
	return ((gdcmCDicts *) &g.GetDicts());
}

struct gdcmCDictEntry *gdcmCDictsGetDictEntry(struct gdcmCDicts *dicts,
	struct gdcmCTag *tag, char *owner)
{
	gdcm::Dicts *cxxdicts = (gdcm::Dicts *)dicts;
	gdcm::Tag *cxxtag = (gdcm::Tag *)tag;
	return((gdcmCDictEntry *)&cxxdicts->GetDictEntry(*cxxtag, owner));	
}

const char *gdcmCDictEntryGetName(struct gdcmCDictEntry *dictentry)
{
	gdcm::DictEntry *cxxdictentry = (gdcm::DictEntry *)dictentry;
	return(cxxdictentry->GetName());
}

struct gdcmCVR *gdcmCDictEntryGetVR(struct gdcmCDictEntry *dictentry)
{
	gdcm::DictEntry *cxxdictentry = (gdcm::DictEntry *)dictentry;
	return((gdcmCVR *)&cxxdictentry->GetVR());
}

unsigned int gdcmCImageGetNumberOfDimensions(struct gdcmCImage *cimage)
{
	gdcm::Image *image = (gdcm::Image *)cimage;
	return(image->GetNumberOfDimensions());
}

unsigned int gdcmCImageGetDimension(struct gdcmCImage *cimage,
	unsigned int dimension_index)
{
	gdcm::Image *image = (gdcm::Image *)cimage;
	return(image->GetDimension(dimension_index));
}

unsigned long gdcmCImageGetBufferLength(struct gdcmCImage *cimage)
{
	gdcm::Image *image = (gdcm::Image *)cimage;
	return(image->GetBufferLength());
}
 
int gdcmCImageGetBuffer(struct gdcmCImage *cimage,
	char *data)
{
	gdcm::Image *image = (gdcm::Image *)cimage;
	return(image->GetBuffer(data));
}

struct gdcmCPixelFormat *gdcmCImageGetPixelFormat(struct gdcmCImage *cimage)
{
	gdcm::Image *image = (gdcm::Image *)cimage;
	return((struct gdcmCPixelFormat *)&image->GetPixelFormat());
}

unsigned short gdcmCPixelFormatGetSamplesPerPixel(struct gdcmCPixelFormat *cpixelformat)
{
	gdcm::PixelFormat *pixelformat = (gdcm::PixelFormat *)cpixelformat;
	return(pixelformat->GetSamplesPerPixel());
}

enum gdcmCScalarType gdcmCPixelFormatGetScalarType(struct gdcmCPixelFormat *cpixelformat)
{
	gdcm::PixelFormat *pixelformat = (gdcm::PixelFormat *)cpixelformat;
	gdcm::PixelFormat::ScalarType scalartype = pixelformat->GetScalarType();
	enum gdcmCScalarType cscalartype;
	switch(scalartype)
	{
		case gdcm::PixelFormat::UINT8:
		{
			cscalartype = GDCMCSCALARTYPE_UINT8;
		} break;
		case gdcm::PixelFormat::INT8:
		{
			cscalartype = GDCMCSCALARTYPE_INT8;
		} break;
		case gdcm::PixelFormat::UINT12:
		{
			cscalartype = GDCMCSCALARTYPE_UINT12;
		} break;
		case gdcm::PixelFormat::INT12:
		{
			cscalartype = GDCMCSCALARTYPE_INT12;
		} break;
		case gdcm::PixelFormat::UINT16:
		{
			cscalartype = GDCMCSCALARTYPE_UINT16;
		} break;
		case gdcm::PixelFormat::INT16:
		{
			cscalartype = GDCMCSCALARTYPE_INT16;
		} break;
		case gdcm::PixelFormat::UINT32:
		{
			cscalartype = GDCMCSCALARTYPE_UINT32;
		} break;
		case gdcm::PixelFormat::INT32:
		{
			cscalartype = GDCMCSCALARTYPE_INT32;
		} break;
		case gdcm::PixelFormat::FLOAT16:
		{
			cscalartype = GDCMCSCALARTYPE_FLOAT16;
		} break;
		case gdcm::PixelFormat::FLOAT32:
		{
			cscalartype = GDCMCSCALARTYPE_FLOAT32;
		} break;
		case gdcm::PixelFormat::FLOAT64:
		{
			cscalartype = GDCMCSCALARTYPE_FLOAT64;
		} break;
		case gdcm::PixelFormat::UNKNOWN:
		{
			cscalartype = GDCMCSCALARTYPE_UNKNOWN;
		} break;
		default:
		{
			cscalartype = GDCMCSCALARTYPE_UNKNOWN;
		} break;
	}
	return(cscalartype);
}

unsigned short gdcmCPixelFormatGetHighBit(struct gdcmCPixelFormat *pixelformat)
{
	gdcm::PixelFormat *cxxpixelformat = (gdcm::PixelFormat *)pixelformat;
	return(cxxpixelformat->GetHighBit());
}

unsigned short gdcmCPixelFormatGetBitsAllocated(struct gdcmCPixelFormat *pixelformat)
{
	gdcm::PixelFormat *cxxpixelformat = (gdcm::PixelFormat *)pixelformat;
	return(cxxpixelformat->GetBitsAllocated());
}

unsigned short gdcmCPixelFormatGetBitsStored(struct gdcmCPixelFormat *pixelformat)
{
	gdcm::PixelFormat *cxxpixelformat = (gdcm::PixelFormat *)pixelformat;
	return(cxxpixelformat->GetBitsStored());
}
