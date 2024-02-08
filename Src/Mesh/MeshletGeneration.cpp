
#include "MeshletGeneration.h"
#include "glm/common.hpp"
#include <cassert>
#include <cstdint>
#include <unordered_map>

std::vector<Meshlet> MeshletGeneration::MeshletizeUnoptimized(uint32_t maxVerts, uint32_t maxIndices,
                                                              const std::vector<uint32_t> indices,
                                                              const std::vector<glm::vec3> positions)
{

    Meshlet meshlet;

    std::vector<Meshlet> meshlets;
    std::unordered_map<uint32_t, glm::vec3> vertex_accu;

    std::vector<uint32_t> index_stack;

    index_stack.reserve(maxIndices);

    for (uint32_t offset = 0; offset < indices.size();)
    {

        meshlet = {};

        size_t offsetEnd = glm::min(size_t(offset + maxIndices), indices.size());

        uint32_t triangleCount = 0;

        for (uint32_t i = offset; i < offsetEnd; i++)
        {
            triangleCount += uint32_t((i + 1) % 3 == 0);

            auto it = vertex_accu.find(indices[i]);

            if (it == vertex_accu.end())
            {
                if (maxVerts <= vertex_accu.size())
                    break;

                vertex_accu[indices[i]] = positions[indices[i]];
            }

            index_stack.push_back(indices[i]);
        }

        assert(maxVerts >= vertex_accu.size());

        uint32_t i = 0;

        for (auto& vert : vertex_accu)
        {
            meshlet.vertices[i] = vert.first;
        }

        assert(maxIndices >= index_stack.size());

        meshlet.vertexCount = vertex_accu.size();

        for (uint32_t i = 0; i < triangleCount * 3; i++)
        {

            meshlet.indices[i] = index_stack[i];
        }

        meshlet.indicesCount = index_stack.size();

        meshlets.push_back(meshlet);

        index_stack.clear();
        vertex_accu.clear();

        offset += triangleCount * 3;
    }

    return meshlets;

    // for offset: u32 = 0; offset < u32(len(indices)); {
    //
    //        meshlet = Meshlet{}
    //
    //        offset_end := min(offset + max_indices, u32(len(indices)))
    //
    // 	indices_slice := indices[offset:offset_end]
    //
    // 	triangle_count: u32 = 0
    //
    // 	for index, i in indices_slice {
    //
    // 		triangle_count += u32((i + 1) % 3) == 0 ? 1 : 0
    //
    // 		_, ok := vertex_accu[index]
    //
    // 		if !ok {
    // 			if max_verts <= u32(len(vertex_accu)) {
    // 				break
    // 			}
    // 			vertex_accu[index] = vertices[index]
    // 		}
    //
    // 		append(&index_stack, index)
    // 	}
    //
    // 	vert_index: u32 = 0
    // 	for key, _ in vertex_accu {
    //
    // 		if vert_index >= max_verts {
    // 			break
    // 		}
    //
    // 		meshlet.vertices[vert_index] = key
    // 		vert_index += 1
    // 	}
    //
    // 	meshlet.vertex_count = u32(len(vertex_accu))
    //
    //        for i in (0..<triangle_count*3) {
    //            meshlet.indices[i] = index_stack[i]
    //        }
    //
    // 	meshlet.index_count = u32(triangle_count * 3)
    //
    // 	append(&meshlets, meshlet)
    //
    //        clear(&index_stack)
    //        clear(&vertex_accu)
    //
    //        offset += triangle_count * 3
    // }
    //
    //
    // return meshlets
}
