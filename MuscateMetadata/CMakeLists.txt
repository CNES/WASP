set(MuscateMetadata_HEADERS
    include/FluentXML.hpp
    include/tinyxml_utils.hpp
    include/string_utils.hpp
    include/MuscateMetadata.hpp
    include/MuscateMetadataReader.hpp
    include/MuscateMetadataWriter.hpp
    include/MetadataUtil.hpp
    include/ViewingAngles.hpp)

set(MuscateMetadata_SOURCES
    src/FluentXML.cpp
    src/tinyxml_utils.cpp
    src/string_utils.cpp
    src/MuscateMetadataReader.cpp
    src/MuscateMetadataWriter.cpp
    src/ViewingAngles.cpp
    src/MetadataUtil.cpp)

add_library(MuscateMetadata SHARED ${MuscateMetadata_HEADERS} ${MuscateMetadata_SOURCES})
target_link_libraries(MuscateMetadata
    "${Boost_LIBRARIES}"
    "${OTBCommon_LIBRARIES}"
    "${OTBTinyXML_LIBRARIES}"
    "${OTBITK_LIBRARIES}")

target_include_directories(MuscateMetadata PUBLIC include)

install(TARGETS MuscateMetadata DESTINATION lib/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
