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

const unsigned int GEO_Vox::s_vox_version = 150u;
const unsigned int GEO_Vox::s_vox_palette_size = 256u;


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
        if(stream.bread(&vox_magic_number) != 1)
        {
            return GA_Detail::IOStatus(status);
        }

        if(!checkMagicNumber(vox_magic_number))
        {
            return GA_Detail::IOStatus(status);
        }
    }

    unsigned vox_version = 0;
    if(stream.bread(&vox_version) != 1)
    {
        return GA_Detail::IOStatus(status);
    }

    if(GEO_Vox::s_vox_version != vox_version)
    {
        return GA_Detail::IOStatus(status);
    }

    GEO_VoxChunk vox_chunk_main;
    if(!ReadVoxChunk(stream, vox_chunk_main))
    {
        return GA_Detail::IOStatus(status);
    }

    if(GEO_Vox::s_vox_main != vox_chunk_main.chunk_id)
    {
        return GA_Detail::IOStatus(status);
    }

    // We skip the content of main chunk.
    if(!stream.seekg(vox_chunk_main.content_size, UT_IStream::UT_SEEK_BEG))
    {
        return GA_Detail::IOStatus(status);
    }

    // Variables to read voxel data into.
    unsigned int vox_size_x = 0u;
    unsigned int vox_size_y = 0u;
    unsigned int vox_size_z = 0u;

    UT_Array<GEO_VoxPaletteColor> vox_palette;
    UT_Array<GEO_VoxVoxel> vox_voxels;

    // We start reading chunks specified in the main chunk.
    while(true)
    {
        GEO_VoxChunk vox_chunk_child;
        if(!ReadVoxChunk(stream, vox_chunk_child))
        {
            return GA_Detail::IOStatus(status);
        }

        if(GEO_Vox::s_vox_size == vox_chunk_child.chunk_id)
        {
            if(stream.bread(&vox_size_x) != 1)
            {
                return GA_Detail::IOStatus(status);
            }

            if(stream.bread(&vox_size_y) != 1)
            {
                return GA_Detail::IOStatus(status);
            }

            if(stream.bread(&vox_size_z) != 1)
            {
                return GA_Detail::IOStatus(status);
            }
        }
        else if(GEO_Vox::s_vox_xyzi == vox_chunk_child.chunk_id)
        {
            unsigned int vox_voxel_count = 0u;

            if(stream.bread(&vox_voxel_count) != 1)
            {
                return GA_Detail::IOStatus(status);
            }

            vox_voxels.setSize(vox_voxel_count);
            for(unsigned int idx = 0; idx < vox_voxel_count; ++idx)
            {
                GEO_VoxVoxel vox_voxel;
                if(!ReadVoxel(stream, vox_voxel))
                {
                    return GA_Detail::IOStatus(status);
                }

                vox_voxels[idx] = vox_voxel;
            }
        }
        else if(GEO_Vox::s_vox_rgba == vox_chunk_child.chunk_id)
        {
            vox_palette.setSize(GEO_Vox::s_vox_palette_size);
            for(unsigned int idx = 0; idx < GEO_Vox::s_vox_palette_size; ++idx)
            {
                GEO_VoxPaletteColor vox_palette_color;
                if(!ReadPaletteColor(stream, vox_palette_color))
                {
                    return GA_Detail::IOStatus(status);
                }

                vox_palette[idx] = vox_palette_color;
            }
        }
        else
        {
            // We don't know this chunk, skip content in addition to skipping children.
            if(!stream.seekg(vox_chunk_child.content_size, UT_IStream::UT_SEEK_BEG))
            {
                return GA_Detail::IOStatus(status);
            }
        }

        // Skip children.
        if(!stream.seekg(vox_chunk_child.children_chunk_size, UT_IStream::UT_SEEK_BEG))
        {
            return GA_Detail::IOStatus(status);
        }
    }

    if(!status)
    {
        detail->clearAndDestroy();
    }

    status = true;
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
    if(stream.bread(&chunk.chunk_id) != 1)
    {
        return false;
    }

    if(stream.bread(&chunk.content_size) != 1)
    {
        return false;
    }

    if(stream.bread(&chunk.children_chunk_size) != 1)
    {
        return false;
    }

    chunk.children_chunks_start = 3 * sizeof(unsigned int) + chunk.content_size * sizeof(unsigned char);
    chunk.children_chunks_end = chunk.children_chunks_start + chunk.children_chunk_size * sizeof(unsigned char);

    return true;
}


bool
GEO_Vox::ReadPaletteColor(UT_IStream& stream, GEO_VoxPaletteColor& palette_color)
{
    if(stream.bread(&palette_color.r) != 1)
    {
        return false;
    }

    if(stream.bread(&palette_color.g) != 1)
    {
        return false;
    }

    if(stream.bread(&palette_color.b) != 1)
    {
        return false;
    }

    if(stream.bread(&palette_color.a) != 1)
    {
        return false;
    }

    return true;
}


bool
GEO_Vox::ReadVoxel(UT_IStream& stream, GEO_VoxVoxel& vox_voxel)
{
    if(stream.bread(&vox_voxel.x) != 1)
    {
        return false;
    }

    if(stream.bread(&vox_voxel.y) != 1)
    {
        return false;
    }

    if(stream.bread(&vox_voxel.z) != 1)
    {
        return false;
    }

    if(stream.bread(&vox_voxel.palette_index) != 1)
    {
        return false;
    }

    return true;
}
