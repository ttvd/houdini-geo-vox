#include <GU/GU_Detail.h>
#include <GU/GU_PrimPacked.h>
#include <UT/UT_IOTable.h>
#include <UT/UT_Assert.h>

#include <iostream>


int main(int argc, const char* argv[])
{
     GU_Detail::loadIODSOs();

    if (argc < 2) {
        std::cerr << "Usage: test geofile.bgeo\n";
        return 1;
    }

    GU_Detail gdp;
    UT_StringArray errors;
    if(!gdp.load(argv[1], 0,  &errors).success()) {
        std::cerr << "Cant open file: " << argv[1] << "\n";
        for (auto & err: errors) {
            std::cerr << err << "\n";
        }
        return 1;
    }

    const GU_PrimPacked *cube = UTverify_cast<const GU_PrimPacked *>(gdp.getPrimitive(0));

    UT_BoundingBoxF cube_bbox;
    UT_BoundingBoxF gdp_bbox;
    gdp.getBBox(&gdp_bbox);
    cube->getBBox(&cube_bbox);

    const auto vox_size = UT_Vector3I{
        static_cast<int64>(gdp_bbox.sizeX() / cube_bbox.sizeX()),
        static_cast<int64>(gdp_bbox.sizeY() / cube_bbox.sizeY()),
        static_cast<int64>(gdp_bbox.sizeZ() / cube_bbox.sizeZ())
    };

    GA_ROHandleV3 color_attr_h(gdp.findFloatTuple(GA_ATTRIB_POINT, "Cd"));
    if (color_attr_h.isInvalid()) {
        std::cerr << "No color attribute found.\n";
    }


    GA_Offset ptoff;
    GA_FOR_ALL_PTOFF(&gdp, ptoff)
    {
        const GA_Index index = gdp.pointIndex(ptoff);
        const auto pos = gdp.getPos3(ptoff);

    }

}
