#include "GEO_Vox.h"


#include <UT/UT_DSOVersion.h>
#include <GEO/GEO_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <GU/GU_Detail.h>
#include <UT/UT_IOTable.h>
#include <UT/UT_Assert.h>

#define GEOVOX_MAKE_ID(A, B, C, D) ( A ) | ( B << 8 ) | ( C << 16 ) | ( D << 24 )

const unsigned int GEO_Vox::s_vox_magic = GEOVOX_MAKE_ID('V', 'O', 'X', ' ');
const unsigned int GEO_Vox::s_vox_main = GEOVOX_MAKE_ID('M', 'A', 'I', 'N');
const unsigned int GEO_Vox::s_vox_size = GEOVOX_MAKE_ID('S', 'I', 'Z', 'E');
const unsigned int GEO_Vox::s_vox_xyzi = GEOVOX_MAKE_ID('X', 'Y', 'Z', 'I');
const unsigned int GEO_Vox::s_vox_rgba = GEOVOX_MAKE_ID('R', 'G', 'B', 'A');


const unsigned GEO_Vox::s_vox_version = 150u;


void
newGeometryIO(void* parm)
{
    GU_Detail::registerIOTranslator(new GEO_Vox());

    UT_ExtensionList* extension = UTgetGeoExtensions();
    if(!extension->findExtension("vox"))
    {
        extension->addExtension("vox");
    }
}


GEO_Vox::GEO_Vox() :
    m_filename(UT_String::ALWAYS_DEEP)
{

}


GEO_Vox::GEO_Vox(const GEO_Vox& ref) :
    m_filename(UT_String::ALWAYS_DEEP, ref.m_filename)
{

}


GEO_Vox::~GEO_Vox()
{

}


GEO_IOTranslator*
GEO_Vox::duplicate() const
{
    return new GEO_Vox(*this);
}


const char*
GEO_Vox::formatName() const
{
    return "Vox";
}


int
GEO_Vox::checkExtension(const char* name)
{
    UT_String str_name(UT_String::ALWAYS_DEEP, name);
    const char* ext = str_name.fileExtension();

    if(!ext)
    {
        return 0;
    }

    if(!strcasecmp(ext, ".vox"))
    {
        m_filename = str_name;
        return 1;
    }

    return 0;
}


int
GEO_Vox::checkMagicNumber(unsigned magic)
{
    return GEO_Vox::s_vox_magic == magic;
}


GA_Detail::IOStatus
GEO_Vox::fileLoad(GEO_Detail* detail, UT_IStream& stream, bool ate_magic)
{
    bool status = false;

    GU_Detail* gu_detail = dynamic_cast<GU_Detail*>(detail);
    UT_ASSERT(gu_detail);

    if(!ate_magic)
    {
        unsigned vox_magic_number = 0;
        if(stream.read(&vox_magic_number) != 1)
        {
            return GA_Detail::IOStatus(status);
        }

        if(!checkMagicNumber(vox_magic_number))
        {
            return GA_Detail::IOStatus(status);
        }
    }

    unsigned vox_version = 0;
    if(stream.read(&vox_version) != 1)
    {
        return GA_Detail::IOStatus(status);
    }

    if(GEO_Vox::s_vox_version != vox_version)
    {
        return GA_Detail::IOStatus(status);
    }

    if(!status)
    {
        detail->clearAndDestroy();
    }

    return GA_Detail::IOStatus(status);
}


GA_Detail::IOStatus
GEO_Vox::fileSave(const GEO_Detail* detail, std::ostream& stream)
{
    return GA_Detail::IOStatus(false);
}


bool
GEO_Vox::ReadVoxChunk(UT_IStream& stream, GEO_VoxChunk& chunk)
{
    if(stream.read(&chunk.chunk_id) != 1)
    {
        return false;
    }

    if(stream.read(&chunk.content_size) != 1)
    {
        return false;
    }

    if(stream.read(&chunk.children_chunk_size) != 1)
    {
        return false;
    }
}
