#include <GU/GU_Detail.h>
#include <GU/GU_PrimPacked.h>
#include <UT/UT_IOTable.h>
#include <UT/UT_Assert.h>

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

namespace ColorPalette {
    auto hasher = [](const UT_Vector3I &color) { return color.hash(); };
    auto equaler = [](const UT_Vector3I &c1, const UT_Vector3I &c2) { return c1.hash() == c2.hash(); };
}

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
    // We need this to ensure only packed prims are present
    // TODO: Extent to check if there are volume prims also and handle it
    // TODO: Extend to support also points (without packed cubes)
    UT_ASSERT_P(gdp.getNumPrimitives() == gdp.getNumPoints());


    GA_ROHandleV3 col_attr_h(gdp.findFloatTuple(GA_ATTRIB_POINT, "Cd"));
    UT_ASSERT_P(col_attr_h.isValid());
    using ColorPaletteSet = std::unordered_set<UT_Vector3I, decltype(ColorPalette::hasher), decltype(ColorPalette::equaler)>;
    using PaletteIndices = std::vector<int>;
    auto color_map = ColorPaletteSet(256, ColorPalette::hasher, ColorPalette::equaler);
    auto palette_indices = PaletteIndices{0, 0};
    palette_indices.resize(gdp.getNumPoints());

    GA_Offset ptoff;
    GA_FOR_ALL_PTOFF(&gdp, ptoff)
    {
        const auto point_index = gdp.pointIndex(ptoff);
        const auto point_color = col_attr_h.get(ptoff);
        const auto color_integer = UT_Vector3I(SYSclamp(point_color*256.0, UT_Vector3F(0.0), UT_Vector3F(255.0)));
        auto color_map_it = color_map.insert(color_integer);
        const auto palette_index = std::distance(color_map_it.first, color_map.end());
        // NOTE: We take last one if color hasn't been found but palette exceeded its size
        palette_indices[point_index] =  palette_index < 255 ? palette_index : 255;
    }

    for(const auto i : palette_indices) {
        std::cerr  << i << ",";
    }



    if(false) {
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








}
