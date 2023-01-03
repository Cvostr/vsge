cmake_minimum_required (VERSION 3.14)

project ("Shaders")

set(SHADERS_FILES   pbr/pbr.vert
                    pbr/pbr.frag
                    pbr/brdf_lut.comp
                    pbr/irmap.comp
                    pbr/spmap.comp
                    
                    vegetable/vegetable.vert
                    vegetable/vegetable.frag

                    terrain/terrain.vert
                    terrain/terrain.frag

                    particle/particle.vert
                    particle/particle.frag

                    postprocess/postprocess.vert
                    DeferredLight/deferred_pbr.frag
                    DeferredLight/deferred_pbr_envmap.frag

                    skybox/skybox.vert
                    skybox/skybox.frag

                    shadowmap/shadowmap.vert
                    shadowmap/shadowmap_terrain.vert
                    shadowmap/shadowmap.frag
                    shadowmap/shadowmap.geom

                    shadowmap/shadowmap_process.vert
                    shadowmap/shadowmap_process.frag

                    grass/grass.vert
                    grass/grass.frag

                    ui/ui.vert
                    ui/ui.frag
                    ui/ui_add.comp

                    postprocess/gamma_correction/gamma_correction.frag
                    postprocess/gamma_correction/gamma_correction_alpha.frag
                    postprocess/copy.frag
                    postprocess/ui_combine.frag

                    postprocess/hdr_bloom/bloom_bright.frag
                    postprocess/hdr_bloom/bloom_blur_v.frag
                    postprocess/hdr_bloom/bloom_blur_h.frag

                    postprocess/ssao/ssao.frag
                    postprocess/ssao/ssao_blur.frag

                    editor/mark_gizmo_object.frag)

add_custom_target(
    shaders
    DEPENDS ${SHADERS_FILES})

add_dependencies(shaders compiler)

set(COMPILER ${CMAKE_CURRENT_BINARY_DIR}/../ShadersBuilder/compiler)

set(DEST_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(COMPILER_ARGS -ob ${DEST_DIR}/shaders.bundle -om ${DEST_DIR}/shaders.map)

add_custom_command(OUTPUT ${COUTPUT}
  COMMAND ${COMPILER} ${COMPILER_ARGS} ${SHADERS_FILES} 
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Shaders
  DEPENDS ${SHADERS_FILES}
  TARGET shaders
  VERBATIM
)
