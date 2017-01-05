#include "builders/terrain/TerraExtras.hpp"
#include "builders/generators/TreeGenerator.hpp"
#include "mapcss/StyleConsts.hpp"
#include "lsys/LSystem.hpp"
#include "utils/MeshUtils.hpp"

using namespace utymap::builders;
using namespace utymap::mapcss;
using namespace utymap::math;

namespace {
    const std::string TreeFrequencyKey = "tree-" + StyleConsts::FrequencyKey();
    const std::string TreeChunkSize = "tree-chunk-size";
}

void TerraExtras::addForest(const BuilderContext& builderContext, TerraExtras::Context& extrasContext)
{
    // TODO
    // generate tree mesh
    Mesh treeMesh("");
    TreeGenerator generator(builderContext, extrasContext.style, treeMesh);
    generator.setPosition(Vector3(0, 0, 0)); // NOTE we will override coordinates later
    generator.run(lsys::LSystem());

    // forest mesh contains all trees belong to one chunk.
    Mesh forestMesh("forest");
  
    // go through mesh region triangles and insert copy of the tree
    std::size_t step = 3 * static_cast<std::size_t>(std::max(extrasContext.style.getValue(TreeFrequencyKey), 1.));
    int chunkSize = static_cast<int>(std::max(extrasContext.style.getValue(TreeChunkSize), 1.));
    int treesProcessed = 0;
    for (auto i = extrasContext.startTriangle; i < extrasContext.mesh.triangles.size(); i += step) {
        double centroidX = 0;
        double centroidY = 0;
        for (int j = 0; j < 3; j++) {
            int index = extrasContext.mesh.triangles[i + j] * 3;
            centroidX += extrasContext.mesh.vertices[index];
            centroidY += extrasContext.mesh.vertices[index + 1];
        }

        centroidX /= 3;
        centroidY /= 3;

        double elevation = builderContext.eleProvider.getElevation(builderContext.quadKey, centroidY, centroidX);

        utymap::utils::copyMesh(Vector3(centroidX, elevation, centroidY), treeMesh, forestMesh);
        // return chunk if necessary.
        if (++treesProcessed == chunkSize) {
            builderContext.meshCallback(forestMesh);
            forestMesh.clear();
            treesProcessed = 0;
        }
    }

    // complete last iteration
    if (treesProcessed > 0)
        builderContext.meshCallback(forestMesh);
}

void TerraExtras::addWater(const BuilderContext& builderContext, TerraExtras::Context& eeshContext)
{
    // TODO
}