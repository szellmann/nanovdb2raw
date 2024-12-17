#include <string>
#include <nanovdb/io/IO.h>
#include <nanovdb/NanoVDB.h>
#include <nanovdb/math/SampleFromVoxels.h>
#include "math_.h" // nanovdb doesn't accept files named "math.h" on the include path

using int3 = math::vec3i;
using float2 = math::vec2f;
using float3 = math::vec3f;

static std::string g_inFileName;
static std::string g_outFileName;
static int3 g_dims{0,0,0};

static void parseCommandline(int argc, char **argv) {
  try {
    for (int i = 1; i < argc; i++) {
      const std::string arg = argv[i];
      if (arg == "-o")
        g_outFileName = argv[++i];
      else if (arg == "-dims") {
        g_dims.x = atoi(argv[++i]);
        g_dims.y = atoi(argv[++i]);
        g_dims.z = atoi(argv[++i]);
      } else if (arg[0] != '-')
        g_inFileName = arg;
      else
        throw std::runtime_error("./app <in.nvdb> -o <out.raw> -dims w h d");
    }
  }
  catch (std::exception &e) {
    std::cerr << e.what() << '\n';
    exit(1);
  }
}

int main(int argc, char **argv) {

  parseCommandline(argc, argv);

  auto gridHandle = nanovdb::io::readGrid(argv[1]);
  auto grid = gridHandle.grid<float>();
  auto acc = grid->getAccessor();
  auto smp = nanovdb::math::createSampler<1>(acc);

  std::ofstream of(g_outFileName);
  if (!of.good()) {
    std::cerr << "Cannot open " << g_outFileName << " for writing!\n";
    exit(1);
  }

  auto bbox = gridHandle.gridMetaData()->indexBBox();

  float3 lower{ (float)bbox.min()[0], (float)bbox.min()[1], (float)bbox.min()[2] };
  float3 upper{ (float)bbox.max()[0], (float)bbox.max()[1], (float)bbox.max()[2] };

  float3 cellSize
      = (upper-lower) / float3{(float)g_dims.x,(float)g_dims.y,(float)g_dims.z};

  std::vector<float> rawOutput(g_dims.x * size_t(g_dims.y) * g_dims.z);

  for (int z=0; z<g_dims.z; ++z) {
    for (int y=0; y<g_dims.y; ++y) {
      for (int x=0; x<g_dims.x; ++x) {
        size_t index = x + size_t(g_dims.x) * y + size_t(g_dims.x) * g_dims.y * z;

        float3 P = lower + cellSize * float3{(float)x, (float)y, (float)z};
        float value = smp(nanovdb::math::Vec3<float>(P.x,P.y,P.z));
        rawOutput[index] = value;
      }
    }
  }

  of.write((const char *)rawOutput.data(), sizeof(rawOutput[0]) * rawOutput.size());

  std::cout << "Output written to " << g_outFileName << '\n';
  std::cout << "gridOrigin: " << lower << '\n';
  std::cout << "cellSize: " << cellSize << '\n';
  std::cout << "gridDims: " << g_dims << '\n';
}
