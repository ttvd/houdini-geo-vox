#include "GEO_Vox.h"

#include <UT/UT_DSOVersion.h>
#include <GEO/GEO_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimVolume.h>
#include <UT/UT_IOTable.h>
#include <UT/UT_Assert.h>
#include <UT/UT_Endian.h>
#include <UT/UT_Algorithm.h>
#include <SYS/SYS_Math.h>
#include <GU/GU_PrimPacked.h>
#include <OpenImageIO/hash.h>

#define GEOVOX_FILE_SAVE
#define GEOVOX_VERBOSE
#define GEOVOX_SWAP_HOUDINI_AXIS
#define GEOVOX_VOLUME_NAME "color_lut"

#define GEOVOX_MAKE_ID(A, B, C, D) ( A ) | ( B << 8 ) | ( C << 16 ) | ( D << 24 )

const unsigned int GEO_Vox::s_vox_magic = GEOVOX_MAKE_ID('V', 'O', 'X', ' ');
const unsigned int GEO_Vox::s_vox_main = GEOVOX_MAKE_ID('M', 'A', 'I', 'N');
const unsigned int GEO_Vox::s_vox_size = GEOVOX_MAKE_ID('S', 'I', 'Z', 'E');
const unsigned int GEO_Vox::s_vox_xyzi = GEOVOX_MAKE_ID('X', 'Y', 'Z', 'I');
const unsigned int GEO_Vox::s_vox_rgba = GEOVOX_MAKE_ID('R', 'G', 'B', 'A');

const unsigned int GEO_Vox::s_vox_version = 150u;
const unsigned int GEO_Vox::s_vox_palette_size = 256u;


// Taken from https://voxel.codeplex.com/wikipage?title=Sample%20Codes .
const unsigned int GEO_Vox::s_vox_default_palette[256u] =
{
    0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff,
    0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff,
    0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff, 0xff6699ff, 0xff3399ff,
    0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff,
    0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff,
    0xff0033ff, 0xffff00ff, 0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff,
    0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc,
    0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
    0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc,
    0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc,
    0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc, 0xff6633cc, 0xff3333cc,
    0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc,
    0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99,
    0xff00ff99, 0xffffcc99, 0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99,
    0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999,
    0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
    0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399,
    0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099,
    0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66, 0xff66ff66, 0xff33ff66,
    0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66,
    0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966,
    0xff009966, 0xffff6666, 0xffcc6666, 0xff996666, 0xff666666, 0xff336666,
    0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366,
    0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
    0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33,
    0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33,
    0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933, 0xff669933, 0xff339933,
    0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633,
    0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333,
    0xff003333, 0xffff0033, 0xffcc0033, 0xff990033, 0xff660033, 0xff330033,
    0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00,
    0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
    0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900,
    0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600,
    0xff006600, 0xffff3300, 0xffcc3300, 0xff993300, 0xff663300, 0xff333300,
    0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000,
    0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077,
    0xff000055, 0xff000044, 0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00,
    0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400,
    0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
    0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000,
    0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777,
    0xff555555, 0xff444444, 0xff222222, 0xff111111
};


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
    unsigned int vox_child_bytes_read = 0u;

    GU_Detail* gu_detail = dynamic_cast<GU_Detail*>(detail);
    UT_ASSERT(gu_detail);

    // Not sure what's going on since H18, seems to be consuming magic, but not flaging it as such.

    if(!ate_magic)
    {
        unsigned int vox_magic_number = 0;
        if(stream.bread(&vox_magic_number) != 1)
        {
            detail->clearAndDestroy();
            return GA_Detail::IOStatus(false);
        }

        UTswap_int32(vox_magic_number, vox_magic_number);
        if(!checkMagicNumber(vox_magic_number))
        {
            detail->clearAndDestroy();
            return GA_Detail::IOStatus(false);
        }
    }

    unsigned int vox_version = 0;
    if(stream.bread(&vox_version) != 1)
    {
        detail->clearAndDestroy();
        return GA_Detail::IOStatus(false);
    }

    UTswap_int32(vox_version, vox_version);
#ifdef GEOVOX_VERBOSE
    std::cerr << "vox_version: " << vox_version << "\n";
#endif
    if(GEO_Vox::s_vox_version != vox_version)
    {
        detail->clearAndDestroy();
        return GA_Detail::IOStatus(false);
    }

    GEO_VoxChunk vox_chunk_main;
    if(!ReadVoxChunk(stream, vox_chunk_main, vox_child_bytes_read))
    {
        detail->clearAndDestroy();
        return GA_Detail::IOStatus(false);
    }
#ifdef GEOVOX_VERBOSE
    std::cerr << "vox_chunk_main.chunk_id: " << (char*)&vox_chunk_main.chunk_id << "\n";
    std::cerr << "vox_chunk_main.content_size: " << vox_chunk_main.content_size << "\n";
    std::cerr << "vox_chunk_main.children_chunk_size: " << vox_chunk_main.children_chunk_size << "\n";
#endif
    if(GEO_Vox::s_vox_main != vox_chunk_main.chunk_id)
    {
        detail->clearAndDestroy();
        return GA_Detail::IOStatus(false);
    }

    // We skip the content of main chunk.
    if(!stream.seekg(vox_chunk_main.content_size, UT_IStream::UT_SEEK_CUR))
    {
        detail->clearAndDestroy();
        return GA_Detail::IOStatus(false);
    }

    // Variables to read voxel data into.
    unsigned int vox_size_x = 0u;
    unsigned int vox_size_y = 0u;
    unsigned int vox_size_z = 0u;

    UT_Array<GEO_VoxPaletteColor> vox_palette;
    UT_Array<GEO_VoxVoxel> vox_voxels;

    // Reset bytes read.
    vox_child_bytes_read = 0u;

    // We start reading chunks specified in the main chunk.
    while(vox_child_bytes_read != vox_chunk_main.children_chunk_size)
    {
        GEO_VoxChunk vox_chunk_child;
        if(!ReadVoxChunk(stream, vox_chunk_child, vox_child_bytes_read))
        {
            detail->clearAndDestroy();
            return GA_Detail::IOStatus(false);
        }

        if(GEO_Vox::s_vox_size == vox_chunk_child.chunk_id)
        {
#ifdef GEOVOX_VERBOSE
            std::cerr << "Reading: " << (char*)&GEO_Vox::s_vox_size << "\n";
            std::cerr << "vox_chunk_child.chunk_id: " << (char*)&vox_chunk_child.chunk_id << "\n";
            std::cerr << "vox_chunk_child.content_size: " << vox_chunk_child.content_size << "\n";
            std::cerr << "vox_chunk_child.children_chunk_size: " << vox_chunk_child.children_chunk_size << "\n";
#endif
            if(stream.bread(&vox_size_x) != 1)
            {
                detail->clearAndDestroy();
                return GA_Detail::IOStatus(false);
            }

            UTswap_int32(vox_size_x, vox_size_x);
            vox_child_bytes_read += sizeof(unsigned int);

            if(stream.bread(&vox_size_y) != 1)
            {
                detail->clearAndDestroy();
                return GA_Detail::IOStatus(false);
            }

            UTswap_int32(vox_size_y, vox_size_y);
            vox_child_bytes_read += sizeof(unsigned int);

            if(stream.bread(&vox_size_z) != 1)
            {
                detail->clearAndDestroy();
                return GA_Detail::IOStatus(false);
            }

            UTswap_int32(vox_size_z, vox_size_z);
            vox_child_bytes_read += sizeof(unsigned int);
        }
        else if(GEO_Vox::s_vox_xyzi == vox_chunk_child.chunk_id)
        {
#ifdef GEOVOX_VERBOSE
            std::cerr << "Reading: " << (char*)&GEO_Vox::s_vox_xyzi << "\n";
            std::cerr << "vox_chunk_child.chunk_id: " << (char*)&vox_chunk_child.chunk_id << "\n";
            std::cerr << "vox_chunk_child.content_size: " << vox_chunk_child.content_size << "\n";
            std::cerr << "vox_chunk_child.children_chunk_size: " << vox_chunk_child.children_chunk_size << "\n";
#endif
            unsigned int vox_voxel_count = 0u;

            if(stream.bread(&vox_voxel_count) != 1)
            {
                detail->clearAndDestroy();
                return GA_Detail::IOStatus(false);
            }

            UTswap_int32(vox_voxel_count, vox_voxel_count);
            vox_child_bytes_read += sizeof(unsigned int);
#ifdef GEOVOX_VERBOSE
            std::cerr << "vox_size_: " << vox_size_x << ", " << vox_size_y << ", " << vox_size_z << "\n";
            std::cerr << "vox_voxel_count: " << vox_voxel_count << "\n";
#endif

            vox_voxels.setSize(vox_voxel_count);
            for(unsigned int idx = 0; idx < vox_voxel_count; ++idx)
            {
                GEO_VoxVoxel vox_voxel;
                if(!ReadVoxel(stream, vox_voxel, vox_child_bytes_read))
                {
                    detail->clearAndDestroy();
                    return GA_Detail::IOStatus(false);
                }
                vox_voxels(idx) = vox_voxel;
            }
            std::cerr  << "\n";
        }
        else if(GEO_Vox::s_vox_rgba == vox_chunk_child.chunk_id)
        {
#ifdef GEOVOX_VERBOSE
            std::cerr << "Reading: " << (char*)&GEO_Vox::s_vox_rgba << "\n";
            std::cerr << "vox_chunk_child.chunk_id: " << (char*)&vox_chunk_child.chunk_id << "\n";
            std::cerr << "vox_chunk_child.content_size: " << vox_chunk_child.content_size << "\n";
            std::cerr << "vox_chunk_child.children_chunk_size: " << vox_chunk_child.children_chunk_size << "\n";
            std::cerr << "Reading at idx: ";
#endif
            vox_palette.setSize(GEO_Vox::s_vox_palette_size);
            for(unsigned int idx = 0; idx < GEO_Vox::s_vox_palette_size; ++idx)
            {
                GEO_VoxPaletteColor vox_palette_color;
                if(!ReadPaletteColor(stream, vox_palette_color, vox_child_bytes_read))
                {
                    std::cerr << "READNIG ERROR AT idx: " << idx << "\n";
                    detail->clearAndDestroy();
                    return GA_Detail::IOStatus(false);
                }
#ifdef GEOVOX_VERBOSE
            std::cerr << idx << ", ";
#endif

                vox_palette(idx) = vox_palette_color;
            }
             std::cerr  << "\n";
        }
        else
        {
#ifdef GEOVOX_VERBOSE
            std::cerr << "Ignoring at: " << vox_child_bytes_read << "bytes\n";
#endif
            // We don't know this chunk, skip content in addition to skipping children.
            if(!stream.seekg(vox_chunk_child.content_size, UT_IStream::UT_SEEK_CUR))
            {
                detail->clearAndDestroy();
                return GA_Detail::IOStatus(false);
            }

            vox_child_bytes_read += vox_chunk_child.content_size;
        }

        // Skip children.
        if(!stream.seekg(vox_chunk_child.children_chunk_size, UT_IStream::UT_SEEK_CUR))
        {
            detail->clearAndDestroy();
            return GA_Detail::IOStatus(false);
        }

        if(vox_chunk_child.children_chunk_size > 0u)
        {
            vox_child_bytes_read += vox_chunk_child.children_chunk_size;
        }
    }

    // If there was no palette, use default.
    if(0 == vox_palette.size())
    {
        vox_palette.setSize(GEO_Vox::s_vox_palette_size);
        for(unsigned int idx = 0; idx < GEO_Vox::s_vox_palette_size; ++idx)
        {
            GEO_VoxPaletteColor vox_palette_color;
            ConvertDefaultPaletteColor(GEO_Vox::s_vox_default_palette[idx], vox_palette_color);
            vox_palette(idx) = vox_palette_color;
        }
    }

    detail->addStringTuple(GA_ATTRIB_PRIMITIVE, "name", 1);
    GA_RWHandleS name_attrib(detail->findPrimitiveAttribute("name"));

    UT_Matrix3 xform;
    xform.identity();

#ifdef GEOVOX_SWAP_HOUDINI_AXIS
    xform.scale(vox_size_x * 0.5f, vox_size_z * 0.5f, vox_size_y * 0.5f);
#else
    xform.scale(vox_size_x * 0.5f, vox_size_y * 0.5f, vox_size_z * 0.5f);
#endif

    GU_PrimVolume* volume = (GU_PrimVolume*) GU_PrimVolume::build((GU_Detail*) detail);
    volume->setTransform(xform);
    name_attrib.set(volume->getMapOffset(), GEOVOX_VOLUME_NAME);

    UT_VoxelArrayWriteHandleF handle = volume->getVoxelWriteHandle();

#ifdef GEOVOX_SWAP_HOUDINI_AXIS
    handle->size(vox_size_x, vox_size_z, vox_size_y);
#else
    handle->size(vox_size_x, vox_size_y, vox_size_z);
#endif

    for(unsigned int idx_vox = 0, vox_entries = vox_voxels.entries(); idx_vox < vox_entries; ++idx_vox)
    {
        GEO_VoxVoxel vox_voxel = vox_voxels(idx_vox);
        const GEO_VoxPaletteColor& vox_palette_color = vox_palette(vox_voxel.palette_index);

        if(!IsPaletteColorEmpty(vox_palette_color))
        {
#ifdef GEOVOX_SWAP_HOUDINI_AXIS
            handle->setValue(vox_voxel.x, vox_voxel.z, vox_voxel.y, (float) vox_voxel.palette_index);
#else
            handle->setValue(vox_voxel.x, vox_voxel.y, vox_voxel.z, (float) vox_voxel.palette_index);
#endif
        }
    }

    return GA_Detail::IOStatus(true);
}


GA_Detail::IOStatus
GEO_Vox::fileSave(const GEO_Detail* detail, std::ostream& stream)
{
#ifndef GEOVOX_FILE_SAVE
    return GA_Detail::IOStatus(false);
#else

    const auto* gu_detail = dynamic_cast<const GU_Detail*>(detail);
    UT_ASSERT(gu_detail);

    GA_ROHandleV3 color_attr_h(detail->findFloatTuple(GA_ATTRIB_POINT, "Cd"));

    bool isRgb = false;
    auto palette = Palette{};
    if (color_attr_h.isValid())
    {
        palette.reserve(256);
        isRgb = true;
    }
    // FIXME: Currently fails on palette output, so bellow statement makes sense
//    isRgb = false;

    // For now only packed prims ie. #points == #prims
    // TODO: Extent to check if there are volume prims also and handle it
    // TODO: Extend to support only points geo (without packed cubes)
    UT_ASSERT_P(detail->getNumPrimitives() == detail->getNumPoints());

    static const uint32_t zero_size = 0u;
    unsigned int vox_magic_number = GEO_Vox::s_vox_magic;
    if(stream.write((char*)(&vox_magic_number), 4).fail())
    {
            return GA_Detail::IOStatus(false);
    }

    unsigned int vox_version = GEO_Vox::s_vox_version;
    if(stream.write((char*)(&vox_version), 4).fail())
    {
        return GA_Detail::IOStatus(false);
    }

    // Main chunk : 'MAIN' | children size (4bytes) | content size (4 bytes)
    {
        const uint32_t numVoxels = detail->getNumPoints();
        const uint32_t children_chunk_size = ComputeChunkSize(*gu_detail, numVoxels, isRgb);
        bool write_error = false;
        write_error |= stream.write((char*)&GEO_Vox::s_vox_main, 4).fail();
        write_error |= stream.write((char*)&zero_size, sizeof(uint32_t)).fail();
        write_error |= stream.write((char*)&children_chunk_size, sizeof(uint32_t)).fail();

        if (write_error)
        {
            return GA_Detail::IOStatus(false);
        }
    }

    // Size chunk: 'SIZE' | children size (4 bytes) | content size (12 bytes) | 3x4 bytes
    {
        bool write_error = false;
        write_error |= stream.write((char*)&GEO_Vox::s_vox_size, 4).fail();
        const uint32_t twelve_zero_size[2] = {12u, 0};
        write_error |= stream.write(reinterpret_cast<const char*>(&twelve_zero_size), sizeof(int32_t)*2).fail();

        const auto vox_size = ComputeVoxelResolution(*gu_detail, 0, 0);
        const int vox_size_int[3] = {static_cast<int>(vox_size.x()),
                                     static_cast<int>(vox_size.z()),
                                     static_cast<int>(vox_size.y())};

        write_error |= stream.write((char*)&vox_size_int, 4*3).fail();

        if (write_error)
        {
            return GA_Detail::IOStatus(false);
        }
    }

    // Chunk XYZI: 'XYZI' | children size = 0 | content size = numvoxel (4B) + numvoxel*4
    {
        const uint32_t numVoxels = detail->getNumPoints();
        const uint32_t content_size = numVoxels * sizeof(uint32_t) + sizeof(uint32_t);

        bool write_error = false;
        write_error |= stream.write((char*)&GEO_Vox::s_vox_xyzi, 4).fail();
        write_error |= stream.write(reinterpret_cast<const char*>(&content_size), sizeof(uint32_t)).fail();
        write_error |= stream.write(reinterpret_cast<const char*>(&zero_size), sizeof(uint32_t)).fail();
        write_error |= stream.write(reinterpret_cast<const char*>(&numVoxels), sizeof(uint32_t)).fail();

        // Palette generation from points' color
        UT_ExintArray palette_indices{};
        palette_indices.bumpSize(detail->getNumPoints());
        if (isRgb)
        {
            CreateColorPalette(*gu_detail, palette, palette_indices);
        }
        const auto vox_size = ComputeVoxelResolution(*gu_detail, 0, 0);
        GA_Offset ptoff;
        GA_FOR_ALL_PTOFF(detail, ptoff)
        {
            const auto pos = detail->getPos3(ptoff);
            const auto ptnum = detail->pointIndex(ptoff);
            const uint8_t x = SYSfloor(pos.x()+vox_size.x()/2.0f);
            const uint8_t y = SYSfloor(pos.y()+vox_size.y()/2.0f);
            const uint8_t z = SYSfloor(pos.z()+vox_size.z()/2.0f);
            // FIXME: We could use CreateColorPalette to make constant index instead of this:
            const uint8_t i = isRgb ? palette_indices[ptnum] : 1;
            const GEO_VoxVoxel voxel{x, z, y, i};
            write_error |= stream.write((char*)&voxel, 4).fail();
        }

        if (write_error)
        {
            return GA_Detail::IOStatus(false);
        }
    }

    // Chunk RGBA: 'RGBA' | children size = 0 (4B) | content size = 1024 == 4*256
    if (isRgb)
    {
        const uint32_t rgb_size = 1024;// GEO_Vox::s_vox_palette_size * sizeof(uint32_t);
        bool write_error = false;
        write_error |= stream.write((char*)&GEO_Vox::s_vox_rgba, 4).fail();
        write_error |= stream.write((char*)&rgb_size, sizeof(uint32_t)).fail();
        write_error |= stream.write(reinterpret_cast<const char*>(&zero_size), sizeof(uint32_t)).fail();

        write_error |= stream.write((char*)&GEO_Vox::s_vox_default_palette, rgb_size).fail();

//        for (auto it = palette.begin(); it != palette.end(); ++it)
//        {
//            const GEO_VoxPaletteColor color = it->second;
//            write_error = stream.write((char*)&color.data_c, 4).fail();
//        }

        if (write_error)
        {
            return GA_Detail::IOStatus(false);
        }
    }

    return GA_Detail::IOStatus(true);
#endif
}


bool
GEO_Vox::ReadVoxChunk(UT_IStream& stream, GEO_VoxChunk& chunk, unsigned int& bytes_read)
{
    if(stream.bread(&chunk.chunk_id) != 1)
    {
        return false;
    }

    UTswap_int32(chunk.chunk_id, chunk.chunk_id);
    bytes_read += sizeof(unsigned int);

    if(stream.bread(&chunk.content_size) != 1)
    {
        return false;
    }

    UTswap_int32(chunk.content_size, chunk.content_size);
    bytes_read += sizeof(unsigned int);

    if(stream.bread(&chunk.children_chunk_size) != 1)
    {
        return false;
    }

    UTswap_int32(chunk.children_chunk_size, chunk.children_chunk_size);
    bytes_read += sizeof(unsigned int);

    return true;
}

bool
GEO_Vox::ReadPaletteColor(UT_IStream& stream, GEO_VoxPaletteColor& palette_color, unsigned int& bytes_read)
{
    if(stream.bread(&palette_color.r) != 1)
    {
        return false;
    }

    bytes_read += sizeof(unsigned char);

    if(stream.bread(&palette_color.g) != 1)
    {
        return false;
    }

    bytes_read += sizeof(unsigned char);

    if(stream.bread(&palette_color.b) != 1)
    {
        return false;
    }

    bytes_read += sizeof(unsigned char);

    if(stream.bread(&palette_color.a) != 1)
    {
        return false;
    }

    bytes_read += sizeof(unsigned char);

    return true;
}


void
GEO_Vox::ConvertDefaultPaletteColor(unsigned int color, GEO_VoxPaletteColor& palette_color)
{
    palette_color.data_u = color;
}


GEO_VoxColor
GEO_Vox::ConvertPaletteColor(const GEO_VoxPaletteColor& palette_color) const
{
    GEO_VoxColor color;

    color.r = SYSclamp(palette_color.r, 0, 255) / 255.0f;
    color.g = SYSclamp(palette_color.g, 0, 255) / 255.0f;
    color.b = SYSclamp(palette_color.b, 0, 255) / 255.0f;
    color.a = SYSclamp(palette_color.a, 0, 255) / 255.0f;

    return color;
}


bool
GEO_Vox::IsPaletteColorEmpty(const GEO_VoxPaletteColor& palette_color) const
{
    return 0x00000000 == palette_color.data_u;
}

uint32_t
GEO_Vox::ComputeChunkSize(const GU_Detail& gdp, const int numVoxels, const bool isRgb)
{
    const uint32_t size_bytes = 12 + 12;
    const uint32_t xyzi_bytes = 12 + numVoxels*sizeof(unsigned int) + sizeof(unsigned int);
    const uint32_t rgba_bytes = isRgb ? 12u + GEO_Vox::s_vox_palette_size*sizeof(unsigned int) : 0u;
    return size_bytes + xyzi_bytes + rgba_bytes;
}

UT_Vector3I
GEO_Vox::ComputeVoxelResolution(const GU_Detail& gdp, const int numVoxels, const bool isRgb)
{
    const auto *cube = UTverify_cast<const GU_PrimPacked *>(gdp.getPrimitive(0));

    UT_BoundingBoxF cube_bbox;
    UT_BoundingBoxF gdp_bbox;
    gdp.getBBox(&gdp_bbox);
    cube->getBBox(&cube_bbox);

    const auto vox_size = UT_Vector3I{
            static_cast<int64>(gdp_bbox.sizeX() / cube_bbox.sizeX()),
            static_cast<int64>(gdp_bbox.sizeY() / cube_bbox.sizeY()),
            static_cast<int64>(gdp_bbox.sizeZ() / cube_bbox.sizeZ())
    };

    return vox_size;
}

bool
GEO_Vox::ReadVoxel(UT_IStream& stream, GEO_VoxVoxel& vox_voxel, unsigned int& bytes_read)
{
    if(stream.bread(&vox_voxel.x) != 1)
    {
        return false;
    }

    bytes_read += sizeof(unsigned char);

    if(stream.bread(&vox_voxel.y) != 1)
    {
        return false;
    }

    bytes_read += sizeof(unsigned char);

    if(stream.bread(&vox_voxel.z) != 1)
    {
        return false;
    }

    bytes_read += sizeof(unsigned char);

    if(stream.bread(&vox_voxel.palette_index) != 1)
    {
        return false;
    }

    bytes_read += sizeof(unsigned char);

    return true;
}

void GEO_Vox::CreateColorPalette(const GU_Detail &gdp, Palette &palette, UT_ExintArray &palette_indices) const
{
    GA_ROHandleV3 col_attr_h(gdp.findFloatTuple(GA_ATTRIB_POINT, "Cd"));
    UT_ASSERT_P(col_attr_h.isValid());

    GA_Offset ptoff;
    GA_FOR_ALL_PTOFF(&gdp, ptoff)
    {
        const auto ptidx = gdp.pointIndex(ptoff);

        const auto color = col_attr_h.get(ptoff);
        const auto colorI= UT_Vector3I(SYSclamp(color*256.0, UT_Vector3F(0.0), UT_Vector3F(255.0)));
        const auto hash  = colorI.hash();

        auto palette_iter = palette.find(hash);
        if (palette_iter == palette.end() && palette.size() < GEO_Vox::s_vox_palette_size)
        {
            const auto rgba = GEO_VoxPaletteColor{{{static_cast<unsigned char>(colorI.x()),
                                                    static_cast<unsigned char>(colorI.x()),
                                                    static_cast<unsigned char>(colorI.x()),
                                                   1}}}; // TODO alpha?

            std::pair<Palette::iterator, bool> result = palette.insert(std::make_pair(hash, rgba));
            palette_iter = result.first;
        }

        const uint palette_index = std::distance(palette_iter, palette.begin());
        // NOTE: We take last one if color hasn't been found but palette exceeded its size
        palette_indices[ptidx] = palette_index ? palette_index < 255 : 255;
    }


}
