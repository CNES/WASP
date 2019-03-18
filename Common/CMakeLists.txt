set(MetadataHelper_HEADERS
	include/MuscateMetadataHelper.h
    include/MetadataHelperFactory.h
    include/MetadataHelper.h
    include/MetadataBands.h
    include/MetadataAngles.h
    include/GlobalDefs.h
    include/ResamplingBandExtractor.h
    include/ImageResampler.h
    include/TestImageCreator.h
)

set(MetadataHelper_SOURCES
	src/MuscateMetadataHelper.cpp
	src/GlobalDefs.cpp
    src/MetadataHelper.cpp
    src/MetadataBands.cpp
    src/MetadataHelperFactory.cpp
)

add_library(MetadataHelper SHARED ${MetadataHelper_HEADERS} ${MetadataHelper_SOURCES})
target_link_libraries(MetadataHelper MuscateMetadata
    "${Boost_LIBRARIES}"
    "${OTBCommon_LIBRARIES}"
    "${OTBITK_LIBRARIES}")

target_include_directories(MetadataHelper PUBLIC include)
install(TARGETS MetadataHelper DESTINATION lib/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
