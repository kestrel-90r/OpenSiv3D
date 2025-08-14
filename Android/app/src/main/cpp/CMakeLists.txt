cmake_minimum_required(VERSION 3.22.1)

project("opensiv3d")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(S3D_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../..)
set(S3D_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib/arm64-v8a)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(_DEBUG)
else()
  add_compile_definitions(NDEBUG)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
add_compile_definitions(_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION)

add_compile_definitions(ANDROID_BUILD)
add_compile_definitions(STRINGVIEW_CHAR32_CONVERSION)
add_compile_definitions(__ANDROID__)

add_compile_definitions(ONIG_ENCODING_ASCII)
add_compile_definitions(ONIG_ENCODING_UTF8)
add_compile_definitions(ONIG_ENCODING_UTF16_LE)
add_compile_definitions(ONIG_ENCODING_UTF32_LE)
add_compile_definitions(HAVE_CONFIG_H)
add_compile_definitions(ONIG_EXTERN=extern)
add_compile_definitions(ONIGURUMA_EXPORT)
add_compile_definitions(HAVE_STDINT_H)
add_compile_definitions(STDC_HEADERS=1)
add_compile_definitions(LVG_USE_EXTERNAL_PLUTOVG)

add_compile_definitions(IWARE_ANDROID)

add_compile_definitions(HAVE_STDINT_H)
add_compile_definitions(HAVE_ZLIB)
add_compile_definitions(MZ_ZIP_NO_ENCRYPTION)
add_compile_definitions(MZ_ZIP_NO_SIGNING)
add_compile_definitions(UNIX)
add_compile_definitions(MZ_ZIP_NO_COMPRESSION)
add_compile_definitions(MZ_ZIP_NO_BZIP2)
add_compile_definitions(MZ_ZIP_NO_LZMA)
add_compile_definitions(MZ_ZIP_NO_CRYPTO)
add_compile_definitions(MZ_ZIP_NO_WRITE)

add_compile_definitions(_XM_NO_INTRINSICS_)

add_compile_definitions(DIRECTX_MATH_VERSION=315)
add_compile_definitions(_XM_DISABLE_INTRINSICS_)
add_compile_definitions(_XM_NO_XMVECTOR_OVERLOADS_)

add_compile_definitions(WITH_MINIAUDIO)
add_compile_definitions(WITH_NOSOUND)

add_compile_definitions(ABSL_INTERNAL_USE_STD_OSTREAM=1)

add_compile_definitions(SIV3D_ENCODER_DISABLED=1)

set_source_files_properties(
    ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/unicode_property_data.c
    PROPERTIES
    COMPILE_FLAGS "-include ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/oniguruma.h"
)

include_directories(${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/AGDK)
add_library(agdk STATIC
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/AGDK/GameActivity.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/AGDK/GameActivityEvents.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/AGDK/game-text-input/gametextinput.cpp
)
include_directories(${S3D_ROOT}/Dependencies/boost_1_74_0/)
include_directories(../3rd/ffmpeg/usr/include)
include_directories(../3rd/fontconfig/usr/include)
include_directories(../3rd/freetype/usr/include)
include_directories(../3rd/giflib/usr/include)
include_directories(../3rd/harfbuzz/usr/include)
include_directories(../3rd/libcurl/usr/include)
include_directories(../3rd/libjpeg-turbo/usr/include)
include_directories(../3rd/libogg/usr/include)
include_directories(../3rd/libopus/usr/include)
include_directories(../3rd/libopusenc/usr/include)
include_directories(../3rd/libpng/usr/include)
include_directories(../3rd/libtiff/usr/include)
include_directories(../3rd/libvorbis/usr/include)
include_directories(../3rd/libwebp/usr/include)
include_directories(../3rd/libxft/usr/include)
include_directories(../3rd/libxrandr/usr/include)
include_directories(../3rd/libxrende/usr/include)
include_directories(../3rd/mpg123/usr/include)
include_directories(../3rd/opencv/usr/include)
include_directories(../3rd/openssl/usr/include)
include_directories(../3rd/opus-tools/usr/include)
include_directories(../3rd/opusfile/usr/include)
include_directories(../3rd/soundtouch/usr/include)
include_directories(../3rd/vorbis-tools/usr/include)
include_directories(../3rd/xorgproto/usr/include)

include_directories(${S3D_ROOT}/Siv3D/include/)
include_directories(${S3D_ROOT}/Siv3D/include/ThirdParty/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty)
include_directories(${S3D_ROOT}/Siv3D/src/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/libjpeg-turbo)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/libgif)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/curl)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/harfbuzz)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/libpng)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/libtiff)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/libwebp)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/ogg)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/opencv2)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/opus)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty-prebuilt/vorbis)

include_directories(${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/)
include_directories(${S3D_ROOT}/Siv3D/src/Siv3D-Platform/macOS_Linux/)
include_directories(${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/)
include_directories(${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Mouse/)

include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/absl/)
#include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/angelscript/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/asio/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/concaveman-cpp/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/cpptoml/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/cpu_features/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/DirectXMath/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/DirectXTK/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/double-conversion/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/Earcut/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/easyexif/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/fmt/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/freetype/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/gif_load/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/gsl/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/detail/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/levenshtein-sse/)
#include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/lua/)

include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/miniutf/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/muparser/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/nlohmann/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/par_octasphere/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/par_shapes/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/pffft/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/plutovg/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/qr-code-generator-library/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/quirc/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/rectpack2D/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/RFC1321/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/rtaudio/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/serial/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/include/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/SplineLib/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/stb/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/stduuid/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/tinycolormap/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/tinyobjloader/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/TinySoundFont/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/tinyxml/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/Wintab/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/WinToast/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/xxHash/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/zlib/)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/)

include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma)
include_directories(${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/src)

add_library(opensiv3d SHARED
        Main.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/fmt/format.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/absl/numeric/int128.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/absl/random/discrete_distribution.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/absl/random/gaussian_distribution.cc

        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_broad_phase.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_chain_shape.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_circle_shape.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_collide_circle.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_collide_edge.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_collide_polygon.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_collision.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_distance.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_dynamic_tree.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_edge_shape.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_polygon_shape.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/collision/b2_time_of_impact.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/common/b2_block_allocator.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/common/b2_draw.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/common/b2_math.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/common/b2_settings.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/common/b2_stack_allocator.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/common/b2_timer.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_body.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_chain_circle_contact.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_chain_polygon_contact.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_circle_contact.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_contact.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_contact_manager.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_contact_solver.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_distance_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_edge_circle_contact.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_edge_polygon_contact.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_fixture.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_friction_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_gear_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_island.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_motor_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_mouse_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_polygon_circle_contact.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_polygon_contact.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_prismatic_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_pulley_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_revolute_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_weld_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_wheel_joint.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_world.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/dynamics/b2_world_callbacks.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/box2d/rope/b2_rope.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/DirectXMath/DirectXCollision.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/double-conversion/bignum-dtoa.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/double-conversion/bignum.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/double-conversion/cached-powers.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/double-conversion/double-to-string.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/double-conversion/fast-dtoa.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/double-conversion/fixed-dtoa.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/double-conversion/string-to-double.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/double-conversion/strtod.cc

        ${S3D_ROOT}/Siv3D/src/ThirdParty/easyexif/exif.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/fmt/format.cc

        ${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/src/system/amounts/amounts_non_windows.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/src/system/amounts/amounts_windows.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/src/system/memory/memory_darwin.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/src/system/memory/memory_non_windows_non_darwin.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/src/system/memory/memory_windows.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/src/system/OS_info/os_info_darwin.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/src/system/OS_info/os_info_non_windows_non_darwin.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/src/system/OS_info/os_info_windows.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/src/cpu/quantities_cache/quantities_cache_non_windows_non_darwin.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/infoware/src/system/displays/displays_default_blank.cpp

        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lapi.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lauxlib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lbaselib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lcode.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lcorolib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lctype.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/ldblib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/ldebug.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/ldo.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/ldump.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lfunc.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lgc.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/linit.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/liolib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/llex.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lmathlib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lmem.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/loadlib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lobject.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lopcodes.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/loslib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lparser.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lstate.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lstring.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lstrlib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/ltable.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/ltablib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/ltm.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lua.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lundump.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lutf8lib.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lvm.c
        #${S3D_ROOT}/Siv3D/src/ThirdParty/lua/lzio.c

        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/element.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/property.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/styledelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/layoutcontext.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/parser.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/canvas.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/lunasvg.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/clippathelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/defselement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/gelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/geometryelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/graphicselement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/markerelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/maskelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/paintelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/svgelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/stopelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/symbolelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/useelement.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/lunasvg/styleelement.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_zip_rw.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_zip.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_strm_zlib.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_strm_split.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_strm_os_posix.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_strm_mem.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_strm_buf.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_strm.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_os.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_os_posix.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_crypt.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/mz_compat.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/hmac.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/sha1.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/sha2.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/aestab.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/aeskey.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/minizip/aescrypt.c

        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/contour-combiners.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/Contour.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/edge-coloring.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/edge-segments.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/edge-selectors.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/EdgeHolder.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/equation-solver.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/msdf-edge-artifact-patcher.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/msdf-error-correction.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/msdfgen.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/rasterization.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/render-sdf.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/save-bmp.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/save-tiff.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/Scanline.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/sdf-error-estimation.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/shape-description.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/Shape.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/SignedDistance.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/msdfgen/core/Vector2.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/muparser/muParser.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/muparser/muParserBase.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/muparser/muParserBytecode.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/muparser/muParserCallback.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/muparser/muParserError.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/muparser/muParserInt.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/muparser/muParserTokenReader.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/ascii.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/big5.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/cp1251.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/euc_jp.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/euc_jp_prop.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/euc_kr.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/euc_tw.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/gb18030.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_1.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_10.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_11.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_13.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_14.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_15.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_16.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_2.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_3.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_4.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_5.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_6.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_7.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_8.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/iso8859_9.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/koi8.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/koi8_r.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/onig_init.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/regcomp.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/regenc.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/regerror.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/regexec.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/reggnu.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/regparse.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/regsyntax.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/regtrav.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/regversion.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/sjis.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/sjis_prop.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/st.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/unicode.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/unicode_fold1_key.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/unicode_fold2_key.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/unicode_fold3_key.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/unicode_unfold_key.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/utf16_be.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/utf16_le.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/utf32_be.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/utf32_le.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/Oniguruma/utf8.c

        ${S3D_ROOT}/Siv3D/src/ThirdParty/pffft/pffft.c

        ${S3D_ROOT}/Siv3D/src/ThirdParty/plutovg/plutovg-blend.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/plutovg/plutovg-dash.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/plutovg/plutovg-geometry.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/plutovg/plutovg-paint.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/plutovg/plutovg-rle.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/plutovg/plutovg.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/plutovg/sw_ft_math.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/plutovg/sw_ft_raster.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/plutovg/sw_ft_stroker.c

        ${S3D_ROOT}/Siv3D/src/ThirdParty/qr-code-generator-library/qrcodegen.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/quirc/decode.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/quirc/identify.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/quirc/quirc.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/quirc/version_db.c

        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/DetourAlloc.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/DetourAssert.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/DetourCommon.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/DetourNavMesh.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/DetourNavMeshBuilder.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/DetourNavMeshQuery.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/DetourNode.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/Recast.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/RecastAlloc.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/RecastArea.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/RecastAssert.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/RecastContour.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/RecastFilter.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/RecastLayers.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/RecastMesh.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/RecastMeshDetail.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/RecastRasterization.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/RecastDetour/RecastRegion.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/rtaudio/RtAudio.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/serial/impl/list_ports/list_ports_linux.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/serial/impl/unix.cc
        ${S3D_ROOT}/Siv3D/src/ThirdParty/serial/serial.cc

        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/monotone/soloud_monotone.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/noise/soloud_noise.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/sfxr/soloud_sfxr.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/speech/darray.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/speech/klatt.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/speech/resonator.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/speech/soloud_speech.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/speech/tts.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/tedsid/sid.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/tedsid/soloud_tedsid.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/tedsid/ted.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/vic/soloud_vic.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/vizsn/soloud_vizsn.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/wav/dr_impl.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/wav/soloud_wav.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/wav/soloud_wavstream.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/wav/stb_vorbis.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/wav/dr_flac.h
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/wav/dr_wav.h
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/audiosource/wav/dr_mp3.h

        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/backend/miniaudio/soloud_miniaudio.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/backend/nosound/soloud_nosound.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_audiosource.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_bus.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_core_3d.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_core_basicops.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_core_faderops.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_core_filterops.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_core_getters.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_core_setters.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_core_voicegroup.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_core_voiceops.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_fader.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_fft.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_fft_lut.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_file.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_filter.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_misc.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_queue.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/core/soloud_thread.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/filter/soloud_biquadresonantfilter.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/filter/soloud_dcremovalfilter.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/filter/soloud_echofilter.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/filter/soloud_eqfilter.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/filter/soloud_fftfilter.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/filter/soloud_flangerfilter.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/filter/soloud_freeverbfilter.cpp
        ${S3D_ROOT}/Siv3D/src/ThirdParty/soloud/src/filter/soloud_lofifilter.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/SplineLib/Splines.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/tinyobjloader/tiny_obj_loader.cc

        ${S3D_ROOT}/Siv3D/src/ThirdParty/tinyxml/tinyxml2.cpp

        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/common/debug.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/common/entropy_common.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/common/error_private.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/common/fse_decompress.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/common/pool.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/common/threading.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/common/xxhash.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/common/zstd_common.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/fse_compress.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/hist.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/huf_compress.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/zstdmt_compress.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/zstd_compress.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/zstd_compress_literals.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/zstd_compress_sequences.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/zstd_compress_superblock.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/zstd_double_fast.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/zstd_fast.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/zstd_lazy.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/zstd_ldm.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/compress/zstd_opt.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/decompress/huf_decompress.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/decompress/zstd_ddict.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/decompress/zstd_decompress.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/decompress/zstd_decompress_block.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/dictBuilder/cover.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/dictBuilder/divsufsort.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/dictBuilder/fastcover.c
        ${S3D_ROOT}/Siv3D/src/ThirdParty/zstd/dictBuilder/zdict.c

        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Siv3DMain.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/AndroidMain.cpp

        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Audio/SoundTouchFunctions.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/AudioCodec/AudioCodecFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/AudioCodec/CAudioCodec.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/AudioFormat/AAC/AACDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/AudioFormat/MP3/MP3Decoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/ChildProcess/ChildProcessDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Clipboard/CClipboard.cpp

        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Common/OpenGLES.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/ConstantBuffer/ConstantBufferDetailFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Cursor/CCursor.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Dialog/SivDialog_Android.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/DirectoryWatcher/DirectoryWatcherDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/DLL/SivDLL.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/DragDrop/CDragDrop.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/FileSystem/SivFileSystem_Android.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/FreestandingMessageBox/FreestandingMessageBox_Linux.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Mesh/MeshFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/MessageBox/SivMessageBox_Android.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Monitor/SivMonitor.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/PowerStatus/SivPowerStatus.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Renderer/GLES3/CRenderer_GLES3.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Renderer/RendererFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Renderer2D/Renderer2DFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Renderer3D/Renderer3DFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Resource/CResource.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Resource/SivResource_Android.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Shader/ShaderFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/System/CSystem.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/System/SivSystem_Android.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/TextToSpeech/CTextToSpeech.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Texture/TextureFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Time/SivTime.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/ToastNotification/CToastNotification.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/WebcamInfo/SivWebcamInfo.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Window/CWindow.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Window/WindowFactory.cpp

        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/CPUInfo/SivCPUInfo.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Mouse/CMouse.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/TextInput/CTextInput.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/TextInput/SivTextInput.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/TextInput/TextInputFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Pentablet/CPentablet.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Pentablet/PentabletFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Monitor/SivMonitor.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Gamepad/CGamepad.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Keyboard/CKeyboard.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Logger/CLogger.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/Console/CConsole.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/XInput/XInputFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/Android/Siv3D/BinaryReader/BinaryReaderDetail.cpp

        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/macOS_Linux/Siv3D/BinaryWriter/BinaryWriterDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/macOS_Linux/Siv3D/Date/SivDate_macOS_Linux.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/macOS_Linux/Siv3D/DateTime/SivDateTime_macOS_Linux.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/macOS_Linux/Siv3D/EnvironmentVariable/SivEnvironmentVariable.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/macOS_Linux/Siv3D/MemoryMappedFileView/MemoryMappedFileViewDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/macOS_Linux/Siv3D/MemoryMappedFile/MemoryMappedFileDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/macOS_Linux/Siv3D/System/SivSystem_macOS_Linux.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/macOS_Linux/Siv3D/Unicode/SivUnicode_macOS_Linux.cpp

        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/ConstantBuffer/GLES3/ConstantBufferDetail_GLES3.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Mesh/GLES3/CMesh_GLES3.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Mesh/GLES3/GLES3Mesh.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer/GLES3/BackBuffer/GLES3BackBuffer.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer/GLES3/BackBuffer/GLES3InternalTexture2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer/GLES3/BlendState/GLES3BlendState.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer/GLES3/DepthStencilState/GLES3DepthStencilState.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer/GLES3/RasterizerState/GLES3RasterizerState.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer/GLES3/SamplerState/GLES3SamplerState.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer2D/GLES3/CRenderer2D_GLES3.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer2D/GLES3/GLES3Renderer2DCommand.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer2D/GLES3/GLES3Vertex2DBatch.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer3D/GLES3/CRenderer3D_GLES3.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer3D/GLES3/GLES3Line3DBatch.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Renderer3D/GLES3/GLES3Renderer3DCommand.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Shader/GLES3/CShader_GLES3.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Shader/GLES3/PixelShader/GLES3PixelShader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Shader/GLES3/VertexShader/GLES3VertexShader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Shader/GLES3/ShaderPipeline/GLES3ShaderPipeline.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Texture/GLES3/CTexture_GLES3.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D-Platform/OpenGLES3/Siv3D/Texture/GLES3/GLES3Texture.cpp

        ${S3D_ROOT}/Siv3D/src/Siv3D/Addon/AddonFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Addon/CAddon.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Addon/SivAddon.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AnimatedGIFReader/AnimatedGIFReaderDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AnimatedGIFReader/SivAnimatedGIFReader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AnimatedGIFWriter/AnimatedGIFWriterDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AnimatedGIFWriter/SivAnimatedGIFWriter.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ArcEmitter2D/SivArcEmitter2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Asset/AssetFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Asset/CAsset.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Asset/IAssetDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Asset/SivAsset.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AssetMonitor/AssetMonitorFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AssetMonitor/CAssetMonitor.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AsyncHTTPTask/AsyncHTTPTaskDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AsyncHTTPTask/SivAsyncHTTPTask.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Audio/AudioBus.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Audio/AudioData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Audio/AudioFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Audio/CAudio.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Audio/DynamicAudioSource.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Audio/SivAudio.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioAsset/SivAudioAsset.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioAssetData/SivAudioAssetData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioDecoder/AudioDecoderFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioDecoder/CAudioDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioDecoder/SivAudioDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioEncoder/AudioEncoderFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioEncoder/CAudioEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioEncoder/SivAudioEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioFormat/MIDI/MIDIDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioFormat/OggVorbis/OggVorbisDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioFormat/OggVorbis/OggVorbisEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioFormat/Opus/OpusDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioFormat/Opus/OpusEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioFormat/WAVE/WAVEDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioFormat/WAVE/WAVEEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioGroup/AudioGroupDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/AudioGroup/SivAudioGroup.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Base64/SivBase64.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/BasicCamera3D/SivBasicCamera3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Bezier2/SivBezier2.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Bezier3/SivBezier3.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/BigFloat/SivBigFloat.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/BigInt/SivBigInt.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/BinaryReader/SivBinaryReader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/BinaryWriter/SivBinaryWriter.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Blob/SivBlob.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Box/SivBox.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Buffer2D/SivBuffer2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Byte/SivByte.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/CacheDirectory/CacheDirectory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Camera2DParameters/SivCamera2DParameters.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Camera2D/SivCamera2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/CascadeClassifier/CascadeClassifierDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/CascadeClassifier/SivCascadeClassifier.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ChildProcess/SivChildProcess.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Circle/SivCircle.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/CircleEmitter2D/SivCircleEmitter2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Clipboard/ClipboardFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Clipboard/SivClipboard.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Color/SivColor.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Colormap/SivColormap.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/CommandLine/SivCommandLine.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Common/Siv3DEngine.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Compression/SivCompression.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Cone/SivCone.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Console/ConsoleFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Console/SivConsole.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ConstantBuffer/Null/ConstantBufferDetail_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ConstantBuffer/SivConstantBuffer.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/CSV/SivCSV.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Cursor/CCursor_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Cursor/CursorFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Cursor/SivCursor.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Cylinder/SivCylinder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/DateTime/SivDateTime.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/DebugCamera3D/SivDebugCamera3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Dialog/SivDialog.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/DirectoryWatcher/SivDirectoryWatcher.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Disc/SivDisc.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/DragDrop/DragDropFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/DragDrop/SivDragDrop.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/DrawableText/SivDrawableText.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Duration/SivDuration.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/DynamicMesh/SivDynamicMesh.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/DynamicTexture/SivDynamicTexture.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Effect/CEffect.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Effect/EffectData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Effect/EffectFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Effect/SivEffect.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Ellipse/SivEllipse.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Emoji/SivEmoji.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Empty/CEmpty.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Empty/EmptyFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/EngineLog/SivEngineLog.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/EngineOptions/SivEngineOptions.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Error/SivError.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Exif/SivExif.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/FFT/CFFT.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/FFT/FFTFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/FFT/SivFFT.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/FileSystem/SivFileSystem.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/CFont.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/CFont_Headless.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/EmojiData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/FontCommon.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/GlyphCache/BitmapGlyphCache.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/GlyphCache/GlyphCacheCommon.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/GlyphCache/MSDFGlyphCache.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/GlyphCache/SDFGlyphCache.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/GlyphRenderer/agg/agg.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/GlyphRenderer/GlyphRenderer.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/GlyphRenderer/BitmapGlyphRenderer.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/GlyphRenderer/MSDFGlyphRenderer.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/GlyphRenderer/OutlineGlyphRenderer.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/GlyphRenderer/SDFGlyphRenderer.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/FontData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/FontFace.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/FontFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/IconData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Font/SivFont.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/FontAsset/SivFontAsset.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/FontAssetData/SivFontAssetData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/FormatData/SivFormatData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/FormatFloat/SivFormatFloat.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/FormatInt/SivFormatInt.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Formatter/SivFormatter.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/FormatUtility/SivFormatUtility.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Gamepad/GamepadFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Gamepad/SivGamepad.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/GamepadInfo/SivGamepadInfo.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/GeoJSON/SivGeoJSON.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Geometry2D/SivGeometry2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Geometry3D/SivGeometry3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/GlobalAudio/SivGlobalAudio.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/GrabCut/GrabCutDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/GrabCut/SivGrabCut.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Graphics/SivGraphics.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Graphics2D/SivGraphics2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Graphics3D/SivGraphics3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/GUI/CGUI.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/GUI/GUIFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Hash/SivHash.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/HTMLWriter/HTMLWriterDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/HTMLWriter/SivHTMLWriter.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/HTTPResponse/SivHTTPResponse.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Icon/SivIcon.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Image/ImagePainting.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Image/ShapePainting.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Image/SivImage.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageDecoder/CImageDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageDecoder/ImageDecoderFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageDecoder/SivImageDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageEncoder/CImageEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageEncoder/ImageEncoderFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageEncoder/SivImageEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/BMP/BMPDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/BMP/BMPEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/GIF/GIFDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/GIF/GIFEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/JPEG/JPEGDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/JPEG/JPEGEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/PNG/PNGDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/PNG/PNGEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/PPM/PPMDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/PPM/PPMEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/SVG/SVGDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/TGA/TGADecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/TGA/TGAEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/TIFF/TIFFDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/WebP/WebPDecoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageFormat/WebP/WebPEncoder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageProcessing/SivImageProcessing.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ImageROI/SivImageROI.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/InfinitePlane/SivInfinitePlane.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/INI/SivINI.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Input/InputState.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Input/SivInput.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/InputCombination/SivInputCombination.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/InputGroup/SivInputGroup.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Int128/SivInt128.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Interpolation/SivInterpolation.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/IntFormatter/SivIntFormatter.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/IPv4Address/SivIPv4Address.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/JoyCon/SivJoyCon.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/JSON/SivJSON.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Keyboard/KeyboardFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Keyboard/SivKeyboard.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/KlattTTS/SivKlattTTS.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/LicenseManager/CLicenseManager.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/LicenseManager/LicenseManagerFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/LicenseManager/SivLicenseManager.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Line/SivLine.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Line3D/SivLine3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/LineString/SivLineString.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Logger/LoggerFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Logger/SivLogger.cpp
        # Siv3D/src/Siv3D/ManagedScript/ManagedScriptDetail.cpp
        # Siv3D/src/Siv3D/ManagedScript/SivManagedScript.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Mat3x2/SivMat3x2.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Mat3x3/SivMat3x3.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Mat4x4/SivMat4x4.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MathParser/MathParserDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MathParser/SivMathParser.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MD5/SivMD5.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MD5Value/SivMD5Value.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MemoryMappedFileView/SivMemoryMappedFileView.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MemoryMappedFile/SivMemoryMappedFile.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Mesh/Null/CMesh_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Mesh/SivMesh.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MeshData/MeshUtility.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MeshData/SivMeshData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MessageBox/SivMessageBox.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Microphone/MicrophoneDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Microphone/SivMicrophone.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MicrophoneInfo/SivMicrophoneInfo.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MicrosecClock/SivMicrosecClock.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MillisecClock/SivMillisecClock.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Model/CModel.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Model/ModelData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Model/ModelFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Model/SivModel.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ModelObject/SivModelObject.cpp
        # Siv3D/src/Siv3D/Monitor/SivMonitor.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Mouse/MouseFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Mouse/SivMouse.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MSRenderTexture/SivMSRenderTexture.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/MultiPolygon/SivMultiPolygon.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/NavMesh/NavMeshDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/NavMesh/SivNavMesh.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Network/CNetwork.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Network/NetworkFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Network/SivNetwork.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/OpenCV_Bridge/SivOpenCV_Bridge.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/OrientedBox/SivOrientedBox.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ParseBool/SivParseBool.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ParseFloat/SivParseFloat.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ParseInt/SivParseInt.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Particle2D/SivParticle2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ParticleSystem2D/ParticleSystem2DDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ParticleSystem2D/SivParticleSystem2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Pentablet/Null/CPentablet_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Pentablet/SivPentablet.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2Body.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2BodyDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2Circle.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2ContactListener.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2DistanceJoint.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2DistanceJointDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2Line.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2LineString.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2MouseJoint.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2MouseJointDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2PivotJoint.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2PivotJointDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2Polygon.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2Quad.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2Rect.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2Shape.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2SliderJoint.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2SliderJointDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2Triangle.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2WheelJoint.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2WheelJointDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2World.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Physics2D/P2WorldDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/PixelShader/SivPixelShader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/PixelShaderAsset/SivPixelShaderAsset.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/PixelShaderAssetData/SivPixelShaderAssetData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Plane/SivPlane.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Point/SivPoint.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Polygon/PolygonDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Polygon/SivPolygon.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Polygon/Triangulation.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/PolygonEmitter2D/SivPolygonEmitter2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/PrimeNumber/SivPrimeNumber.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/PrimitiveMesh/CPrimitiveMesh.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/PrimitiveMesh/PrimitiveMeshFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Print/CPrint.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Print/PrintFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Print/SivPrint.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ProController/SivProController.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Profiler/CProfiler.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Profiler/ProfilerFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Profiler/SivProfiler.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ProfilerStat/SivProfilerStat.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/PutText/SivPutText.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/QR/SivQR.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/QRScanner/QRScannerDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/QRScanner/SivQRScanner.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Quad/SivQuad.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Quaternion/SivQuaternion.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Random/SivRandom.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Ray/SivRay.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/RDTSCClock/SivRDTSCClock.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Rect/SivRect.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/RectanglePacking/SivRectanglePacking.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/RectEmitter2D/SivRectEmitter2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/RectF/SivRectF.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/RegExp/RegExpDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/RegExp/SivRegExp.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Renderer/Null/CRenderer_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Renderer2D/Null/CRenderer2D_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Renderer3D/Null/CRenderer3D_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/RenderTexture/SivRenderTexture.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Resource/ResourceFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Resource/SivResource.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/RoundRect/SivRoundRect.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Say/SivSay.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Scene/CScene.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Scene/FrameCounter.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Scene/FrameTimer.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Scene/SceneFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Scene/SivScene.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScopedColorAdd2D/SivScopedColorAdd2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScopedColorMul2D/SivScopedColorMul2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScopedCustomShader2D/SivScopedCustomShader2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScopedCustomShader3D/SivScopedCustomShader3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScopedRenderStates2D/SivScopedRenderStates2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScopedRenderStates3D/SivScopedRenderStates3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScopedRenderTarget2D/SivScopedRenderTarget2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScopedRenderTarget3D/SivScopedRenderTarget3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScopedViewport2D/SivScopedViewport2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScopedViewport3D/SivScopedViewport3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScreenCapture/CScreenCapture.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScreenCapture/ScreenCaptureFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ScreenCapture/SivScreenCapture.cpp

        #${S3D_ROOT}/Siv3D/src/Siv3D/ScriptFunction/SivScriptFunction.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/ScriptModule/SivScriptModule.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/angelscript/scriptarray.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/angelscript/scriptbuilder.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/angelscript/scriptgrid.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/angelscript/scriptstdstring.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptAudio.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptAudioFormat.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptBezier2.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptBezier3.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptCamera2D.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptCameraControl.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptChar.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptCircle.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptCircular.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptColor.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptColorF.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptCopyOption.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptCursor.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptCursorStyle.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptDate.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptDateTime.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptDialog.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptDrawableText.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptDuration.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptDynamicTexture.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptEasing.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptEllipse.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptEmoji.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptFileSystem.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptFloat2.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptFloat3.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptFloat4.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptFloatingPoint.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptFloatRect.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptFont.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptFontMethod.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptFontStyle.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptFormat.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptGlobalAudio.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptGMInstrument.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptGraphics.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptGraphics2D.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptHSV.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptIcon.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptImage.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptImageFormat.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptINI.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptInput.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptInterpolation.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptKeyboard.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptLanguageCode.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptLine.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptLineString.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptLineStyle.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptLineStyleParameters.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptMat3x2.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptMath.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptMathConstants.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptMicrosecClock.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptMillisecClock.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptMixBus.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptMonitor.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptMouse.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptNamedParameter.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptNone_t.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptNumber.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptOffsetCircular.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptOpenMode.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptOptional.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptPalette.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptParse.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptPeriodic.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptPianoKey.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptPoint.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptPolygon.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptPrimeNumber.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptPrint.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptProfiler.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptPutText.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptQuad.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptRandom.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptRDTSCClock.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptRect.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptRectF.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptResizeMode.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptResource.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptRoundRect.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptSay.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptScene.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptScopedViewport2D.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptShape2D.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptSimpleGUI.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptSpecialFolder.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptSpline2D.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptStopwatch.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptSystem.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextEditState.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextEncoding.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextInput.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextInputMode.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextReader.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextStyle.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextToSpeech.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTexture.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTexturedCircle.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextureDesc.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTexturedQuad.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTexturedRoundRect.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextureFormat.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTexturePixelFormat.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextureRegion.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTextWriter.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptThreading.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTime.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTimer.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTransformer2D.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTriangle.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTriangleIndex.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTypeface.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptTypes.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptUserAction.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptUtility.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptVariableSpeedStopwatch.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptVec2.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptVec3.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptVec4.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptVideoTexture.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptWave.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptWaveSample.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptWindow.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptWindowStyle.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/Bind/ScriptYesNo.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/CScript.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/ScriptData.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/ScriptFactory.cpp
        #${S3D_ROOT}/Siv3D/src/Siv3D/Script/SivScript.cpp

        ${S3D_ROOT}/Siv3D/src/Siv3D/SerialPortInfo/SivSerialPortInfo.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Serial/SerialDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Serial/SivSerial.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Shader/Null/CShader_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Shader/SivShader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Shape2D/SivShape2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/SIMDMath/SivSIMDMath.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/SimpleAnimation/SivSimpleAnimation.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/SimpleGUI/SivSimpleGUI.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/SimpleHTTP/SivSimpleHTTP.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Sky/SivSky.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/SoundFont/CSoundFont.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/SoundFont/SivSoundFont.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/SoundFont/SoundFontDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/SoundFont/SoundFontFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Sphere/SivSphere.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Spline2D/SivSpline2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/String/SivString.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/String/Levenshtein.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/StringView/SivStringView.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Subdivision2D/SivSubdivision2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/SVG/SivSVG.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/SVG/SVGDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/System/SivSystem.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/System/SystemFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/System/SystemLog.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/System/SystemMisc.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TCPClient/SivTCPClient.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TCPClient/TCPClientDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TCPServer/SivTCPServer.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TCPServer/TCPServerDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextEncoding/SivTextEncoding.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextInput/SivTextInput.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextInput/TextInputFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextReader/SivTextReader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextReader/TextReaderDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextToSpeech/SivTextToSpeech.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextToSpeech/TextToSpeechFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Texture/Null/CTexture_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Texture/SivTexture.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Texture/TextureCommon.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextureAsset/SivTextureAsset.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextureAssetData/SivTextureAssetData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TexturedCircle/SivTexturedCircle.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TexturedQuad/SivTexturedQuad.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TexturedRoundRect/SivTexturedRoundRect.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextureFormat/SivTextureFormat.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextureRegion/SivTextureRegion.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextWriter/SivTextWriter.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TextWriter/TextWriterDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Threading/SivThreading.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TimeProfiler/SivTimeProfiler.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ToastNotification/SivToastNotification.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ToastNotification/ToastNotificationFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/TOMLReader/SivTOMLReader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Triangle/SivTriangle.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Triangle3D/SivTriangle3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Twitter/SivTwitter.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Unicode/SivUnicode.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Unicode/UnicodeUtility.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/UnicodeConverter/SivUnicodeConverter.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/UserAction/CUserAction.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/UserAction/UserActionFactory.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/UUIDValue/SivUUIDValue.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Vector2D/SivVector2D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Vector3D/SivVector3D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Vector4D/SivVector4D.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/VertexShader/SivVertexShader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/VertexShaderAsset/SivVertexShaderAsset.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/VertexShaderAssetData/SivVertexShaderAssetData.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/VideoReader/SivVideoReader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/VideoReader/VideoReaderDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/VideoWriter/SivVideoWriter.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/VideoWriter/VideoWriterDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/VideoTexture/SivVideoTexture.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/VideoTexture/VideoTextureDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ViewFrustum/SivViewFrustum.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Wave/SivWave.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Webcam/SivWebcam.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Webcam/WebcamDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Window/SivWindow.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Window/Null/CWindow_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/XInput/Null/CXInput_Null.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/XInput/SivXInput.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/XMLReader/SivXMLReader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ZIPReader/SivZIPReader.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/ZIPReader/ZIPReaderDetail.cpp
        ${S3D_ROOT}/Siv3D/src/Siv3D/Zlib/SivZlib.cpp

)


file(GLOB ANDROID_GLOB_SRC ../3rd/libandroid-glob/*.c)
add_library(android-glob SHARED ${ANDROID_GLOB_SRC})
target_include_directories(android-glob PUBLIC ../3rd/libandroid-glob)
set_target_properties(android-glob PROPERTIES
        OUTPUT_NAME android-glob
        POSITION_INDEPENDENT_CODE ON
        SOVERSION 0
)

file(GLOB POSIX_SEM_SRC ../3rd/libandroid-posix-semaphore/*.c)
add_library(android-posix-semaphore SHARED ${POSIX_SEM_SRC})
target_include_directories(android-posix-semaphore PUBLIC ../3rd/libandroid-posix-semaphore)
target_compile_definitions(android-posix-semaphore PRIVATE
_PATH_TMP=\"/data/data/com.kestrel.opensiv3d/cache/\"
sem_open=libandroid_sem_open
sem_close=libandroid_sem_close
sem_unlink=libandroid_sem_unlink
)

set_target_properties(android-posix-semaphore PROPERTIES
        OUTPUT_NAME android-posix-semaphore
        POSITION_INDEPENDENT_CODE ON
        SOVERSION 0
)

file(GLOB ANDROID_SUPPORT_SRC
        ../3rd/libandroid-support/src/musl-ctype/*.c
        ../3rd/libandroid-support/src/musl-ctype/wcwidth/*.c
        ../3rd/libandroid-support/src/musl-multibyte/*.c
)
add_library(android-support SHARED ${ANDROID_SUPPORT_SRC})
target_compile_features(android-support PRIVATE c_std_99)
target_compile_options(android-support PRIVATE -fPIC)

add_library(aom SHARED IMPORTED)
add_library(avcodec SHARED IMPORTED)
add_library(avformat SHARED IMPORTED)
add_library(avutil SHARED IMPORTED)
add_library(bluray SHARED IMPORTED)
add_library(brotlicommon SHARED IMPORTED)
add_library(brotlidec SHARED IMPORTED)
add_library(bz2 SHARED IMPORTED)
add_library(c++_shared SHARED IMPORTED)
add_library(crypto SHARED IMPORTED)
add_library(curl SHARED IMPORTED)
add_library(dav1d SHARED IMPORTED)
add_library(expat SHARED IMPORTED)
add_library(fontconfig SHARED IMPORTED)
add_library(freetype SHARED IMPORTED)
add_library(gif SHARED IMPORTED)
add_library(glib-2.0 SHARED IMPORTED)
add_library(gme SHARED IMPORTED)
add_library(gmp SHARED IMPORTED)
add_library(gnutls SHARED IMPORTED)
add_library(graphite2 SHARED IMPORTED)
add_library(harfbuzz SHARED IMPORTED)
add_library(hogweed SHARED IMPORTED)
add_library(iconv SHARED IMPORTED)
add_library(idn2 SHARED IMPORTED)
add_library(jpeg SHARED IMPORTED)
add_library(lcms2 SHARED IMPORTED)
add_library(lzma SHARED IMPORTED)
add_library(mp3lame SHARED IMPORTED)
add_library(mpg123 SHARED IMPORTED)
add_library(nettle SHARED IMPORTED)
add_library(nghttp2 SHARED IMPORTED)
add_library(nghttp3 SHARED IMPORTED)
add_library(ogg SHARED IMPORTED)
add_library(OpenCL SHARED IMPORTED)
add_library(opencore-amrnb SHARED IMPORTED)
add_library(opencore-amrwb SHARED IMPORTED)
add_library(opencv_java4 SHARED IMPORTED)
add_library(openmpt SHARED IMPORTED)
add_library(opus SHARED IMPORTED)
add_library(opusfile SHARED IMPORTED)
add_library(pcre2-8 SHARED IMPORTED)
#add_library(png SHARED IMPORTED)
add_library(png16 SHARED IMPORTED)
add_library(rav1e SHARED IMPORTED)
add_library(sharpyuv SHARED IMPORTED)
add_library(SoundTouch SHARED IMPORTED)
add_library(soxr SHARED IMPORTED)
add_library(srt SHARED IMPORTED)
add_library(ssh SHARED IMPORTED)
add_library(ssh2 SHARED IMPORTED)
add_library(ssl SHARED IMPORTED)
add_library(SvtAv1Enc SHARED IMPORTED)
add_library(swresample SHARED IMPORTED)
add_library(theoradec SHARED IMPORTED)
add_library(theoraenc SHARED IMPORTED)
add_library(tiff SHARED IMPORTED)
add_library(tiffxx SHARED IMPORTED)
add_library(turbojpeg SHARED IMPORTED)
add_library(udfread SHARED IMPORTED)
add_library(unistring SHARED IMPORTED)
add_library(vo-amrwbenc SHARED IMPORTED)
add_library(vorbis SHARED IMPORTED)
add_library(vorbisenc SHARED IMPORTED)
add_library(vorbisfile SHARED IMPORTED)
add_library(vpx SHARED IMPORTED)
add_library(webp SHARED IMPORTED)
add_library(webpmux SHARED IMPORTED)
add_library(x264 SHARED IMPORTED)
add_library(x265 SHARED IMPORTED)
add_library(xml2 SHARED IMPORTED)
add_library(xvidcore SHARED IMPORTED)
add_library(zstd SHARED IMPORTED)

set_target_properties(android-support PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libandroid-support.so)
set_target_properties(aom PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libaom.so)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libavcodec.so)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libavformat.so)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libavutil.so)
set_target_properties(bluray PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libbluray.so)
set_target_properties(brotlicommon PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libbrotlicommon.so)
set_target_properties(brotlidec PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libbrotlidec.so)
set_target_properties(bz2 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libbz2.so)
set_target_properties(c++_shared PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libc++_shared.so)
set_target_properties(crypto PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libcrypto.so)
set_target_properties(curl PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libcurl.so)
set_target_properties(dav1d PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libdav1d.so)
set_target_properties(expat PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libexpat.so)
set_target_properties(fontconfig PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libfontconfig.so)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libfreetype.so)
set_target_properties(gif PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libgif.so)
set_target_properties(glib-2.0 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libglib-2.0.so)
set_target_properties(gme PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libgme.so)
set_target_properties(gmp PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libgmp.so)
set_target_properties(gnutls PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libgnutls.so)
set_target_properties(graphite2 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libgraphite2.so)
set_target_properties(harfbuzz PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libharfbuzz.so)
set_target_properties(hogweed PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libhogweed.so)
set_target_properties(iconv PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libiconv.so)
set_target_properties(idn2 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libidn2.so)
set_target_properties(jpeg PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libjpeg.so)
set_target_properties(lcms2 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/liblcms2.so)
set_target_properties(lzma PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/liblzma.so)
set_target_properties(mp3lame PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libmp3lame.so)
set_target_properties(mpg123 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libmpg123.so)
set_target_properties(nettle PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libnettle.so)
set_target_properties(nghttp2 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libnghttp2.so)
set_target_properties(nghttp3 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libnghttp3.so)
set_target_properties(ogg PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libogg.so)
set_target_properties(OpenCL PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libOpenCL.so)
set_target_properties(opencore-amrnb PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libopencore-amrnb.so)
set_target_properties(opencore-amrwb PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libopencore-amrwb.so)
set_target_properties(opencv_java4 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libopencv_java4.so)
set_target_properties(openmpt PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libopenmpt.so)
set_target_properties(opus PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libopus.so)
set_target_properties(opusfile PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libopusfile.so)
set_target_properties(pcre2-8 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libpcre2-8.so)
set_target_properties(png16 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libpng16.so)
set_target_properties(rav1e PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/librav1e.so)
set_target_properties(sharpyuv PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libsharpyuv.so)
set_target_properties(SoundTouch PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libSoundTouch.so)
set_target_properties(soxr PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libsoxr.so)
set_target_properties(srt PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libsrt.so)
set_target_properties(ssh PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libssh.so)
set_target_properties(ssh2 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libssh2.so)
set_target_properties(ssl PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libssl.so)
set_target_properties(SvtAv1Enc PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libSvtAv1Enc.so)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libswresample.so)
set_target_properties(theoradec PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libtheoradec.so)
set_target_properties(theoraenc PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libtheoraenc.so)
set_target_properties(tiff PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libtiff.so)
set_target_properties(tiffxx PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libtiffxx.so)
set_target_properties(turbojpeg PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libturbojpeg.so)
set_target_properties(udfread PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libudfread.so)
set_target_properties(unistring PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libunistring.so)
set_target_properties(vo-amrwbenc PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libvo-amrwbenc.so)
set_target_properties(vorbis PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libvorbis.so)
set_target_properties(vorbisenc PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libvorbisenc.so)
set_target_properties(vorbisfile PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libvorbisfile.so)
set_target_properties(vpx PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libvpx.so)
set_target_properties(webp PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libwebp.so)
set_target_properties(webpmux PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libwebpmux.so)
set_target_properties(x264 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libx264.so)
set_target_properties(x265 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libx265.so)
set_target_properties(xml2 PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libxml2.so)
set_target_properties(xvidcore PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libxvidcore.so)
set_target_properties(zstd PROPERTIES IMPORTED_LOCATION ${S3D_LIBS_DIR}/libzstd.so)

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_options(opensiv3d PRIVATE
            -Wno-deprecated-builtins
            -Wno-c++20-extensions
            -Wno-inconsistent-missing-override
            -Wno-switch
            -Wno-macro-redefined
    )
endif()

target_link_libraries(opensiv3d
        EGL
        GLESv3
        jnigraphics
        android
        log
        z
        opencv_java4

        camera2ndk
        mediandk
        agdk

        android-glob
        android-posix-semaphore
        android-support
        aom
        avcodec
        avformat
        avutil
        bluray
        brotlicommon
        brotlidec
        bz2
        c++_shared
        crypto
        curl
        dav1d
        expat
        fontconfig
        freetype
        gif
        glib-2.0
        gme
        gmp
        gnutls
        graphite2
        harfbuzz
        hogweed
        iconv
        idn2
        jpeg
        lcms2
        lzma
        mp3lame
        mpg123
        nettle
        nghttp2
        nghttp3
        ogg
        OpenCL
        opencore-amrnb
        opencore-amrwb
        opencv_java4
        openmpt
        opus
        opusfile
        pcre2-8
        png16
        rav1e
        sharpyuv
        SoundTouch
        soxr
        srt
        ssh
        ssh2
        ssl
        SvtAv1Enc
        swresample
        theoradec
        theoraenc
        tiff
        tiffxx
        turbojpeg
        udfread
        unistring
        vo-amrwbenc
        vorbis
        vorbisenc
        vorbisfile
        vpx
        webp
        webpmux
        x264
        x265
        xml2
        xvidcore
        zstd
)

set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")
