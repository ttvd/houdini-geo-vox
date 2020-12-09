#pragma once

#include <GEO/GEO_IOTranslator.h>
#include <UT/UT_String.h>


class GU_Detail;

struct GEO_VoxChunk
{
    unsigned int chunk_id;
    unsigned int content_size;
    unsigned int children_chunk_size;
};

struct GEO_VoxPaletteColor
{
    union
    {
        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };
        unsigned char data_c[4];
        unsigned int data_u;
    };
};

struct GEO_VoxColor
{
    union
    {
        struct
        {
            float r;
            float g;
            float b;
            float a;
        };
        float data[4];
    };
};

struct GEO_VoxVoxel
{
    unsigned char x;
    unsigned char y;
    unsigned char z;
    unsigned char palette_index;
};

namespace Rgb {
    auto Hash = [](const UT_Vector4I &color) { return color.hash(); };
    auto Equal = [](const UT_Vector4I &c1, const UT_Vector4I &c2) { return c1.hash() == c2.hash(); };
    using Palette = std::unordered_set<UT_Vector4I, decltype(Hash), decltype(Equal)>;
    using Indices = std::vector<int>;
}

class GEO_Vox : public GEO_IOTranslator
{
    public:
        GEO_Vox();
        GEO_Vox(const GEO_Vox& ref);
        ~GEO_Vox() override;

    public:
        GEO_IOTranslator* duplicate() const override;
        const char* formatName() const override;
        int checkExtension(const char* name) override;
        int checkMagicNumber(unsigned magic) override;
        GA_Detail::IOStatus fileLoad(GEO_Detail* detail, UT_IStream& stream, bool ate_magic) override;
        GA_Detail::IOStatus fileSave(const GEO_Detail* detail, std::ostream& stream) override;
        GA_Detail::IOStatus	fileSaveToFile(const GEO_Detail *gdp, const char *filename) override;

    protected:
        //! Read a chunk.
        bool ReadVoxChunk(UT_IStream& stream, GEO_VoxChunk& chunk, unsigned int& bytes_read);

        //! Read a palette entry.
        bool ReadPaletteColor(UT_IStream& stream, GEO_VoxPaletteColor& palette_color, unsigned int& bytes_read);

        //! Convert palette entry from default palette value.
        void ConvertDefaultPaletteColor(unsigned int color, GEO_VoxPaletteColor& palette_color);

        //! Read a voxel entry.
        bool ReadVoxel(UT_IStream& stream, GEO_VoxVoxel& vox_voxel, unsigned int& bytes_read);

        //! Convert palette to a color.
        GEO_VoxColor ConvertPaletteColor(const GEO_VoxPaletteColor& palette_color) const;

        //! Return true if palette color corresponds to an empty voxel.
        bool IsPaletteColorEmpty(const GEO_VoxPaletteColor& palette_color) const;

        //! Compute main chunk size from geometry present in GU_Detail
        static uint32_t ComputeChunkSize(const GU_Detail& gdp, int numVoxels, bool isRgb);

        //! Compute voxel size
        //! TODO: This is only valid for pure packed prims gdp
        static UT_Vector3I ComputeVoxelResolution(const GU_Detail& gdp, int numVoxels, bool isRgb);

        //! Create Palette from attribute
        static void CreateColorPalette(const GU_Detail& gdp, Rgb::Palette &palette, Rgb::Indices &indices);

    protected:
        //! Magic numbers used by parser.
        static const unsigned int s_vox_magic;
        static const unsigned int s_vox_main;
        static const unsigned int s_vox_size;
        static const unsigned int s_vox_xyzi;
        static const unsigned int s_vox_rgba;

        //! Palette size.
        static const unsigned int s_vox_palette_size;

        //! Supported version.
        static const unsigned int s_vox_version;

        //! Default palette data.
        static const unsigned int s_vox_default_palette[256];

    protected:

        //! Cached filename.
        UT_String m_filename;
};
