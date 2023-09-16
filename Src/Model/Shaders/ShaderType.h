#pragma once

#include <shaderc/shaderc.h>

enum ShaderType {
    NOT_RECOGNIZED = -1,
    VERTEX_SHADER = shaderc_vertex_shader,
    FRAGMENT_SHADER = shaderc_fragment_shader,
    GEOMETRY_SHADER = shaderc_geometry_shader,
    TESS_CONTROL_SHADER = shaderc_tess_control_shader,
    TESS_EVAL_SHADER = shaderc_tess_evaluation_shader,
    COMPUTE_SHADER = shaderc_compute_shader,
};
