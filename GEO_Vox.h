#pragma once

#include <GEO/GEO_IOTranslator.h>
#include <UT/UT_String.h>

class GEO_PrimPoly;
class GU_Detail;

class GEO_Vox : public GEO_IOTranslator
{
    public:

        GEO_Vox();
        GEO_Vox(const GEO_Vox& ref);
        virtual ~GEO_Vox();

    public:

        virtual GEO_IOTranslator* duplicate() const;
        virtual const char* formatName() const;
        virtual int checkExtension(const char* name);
        virtual int checkMagicNumber(unsigned magic);
        virtual GA_Detail::IOStatus fileLoad(GEO_Detail* detail, UT_IStream& stream, bool ate_magic);
        virtual GA_Detail::IOStatus fileSave(const GEO_Detail* detail, std::ostream& stream);

    protected:

        //! Magic numbers used by parser.
        static const unsigned int s_vox_magic;
        static const unsigned int s_vox_main;
        static const unsigned int s_vox_size;
        static const unsigned int s_vox_xyzi;
        static const unsigned int s_vox_rgba;

        //! Supported version.
        static const unsigned int s_vox_version;

    protected:

        //! Cached filename.
        UT_String m_filename;
};
