add_executable(test_MetadataBands test_MetadataBands.cpp)
target_link_libraries(test_MetadataBands
	MuscateMetadata
	MetadataHelper
    "${Boost_LIBRARIES}"
    "${OTBCommon_LIBRARIES}"
    "${OTBITK_LIBRARIES}"
    )

add_executable(test_MuscateMetadataHelper test_MuscateMetadataHelper.cpp)
target_link_libraries(test_MuscateMetadataHelper
	MuscateMetadata
	MetadataHelper
    "${Boost_LIBRARIES}"
    "${OTBCommon_LIBRARIES}"
    "${OTBITK_LIBRARIES}"
    )


add_executable(test_MuscateMetadataHelperVenus test_MuscateMetadataHelperVenus.cpp)
target_link_libraries(test_MuscateMetadataHelperVenus
	MuscateMetadata
	MetadataHelper
    "${Boost_LIBRARIES}"
    "${OTBCommon_LIBRARIES}"
    "${OTBITK_LIBRARIES}"
    )

add_executable(test_ImageResampler test_ImageResampler.cpp ../include/ImageResampler.h)
target_link_libraries(test_ImageResampler
	MuscateMetadata
	MetadataHelper
    ${Boost_LIBRARIES}
    ${OTB_LIBRARIES}
    )

target_include_directories(test_MuscateMetadataHelper PUBLIC ../include)
add_test(test_MuscateMetadataHelper test_MuscateMetadataHelper)

target_include_directories(test_MetadataBands PUBLIC ../include)
add_test(test_MetadataBands test_MetadataBands)

target_include_directories(test_MuscateMetadataHelperVenus PUBLIC ../include)
add_test(test_MuscateMetadataHelperVenus test_MuscateMetadataHelperVenus)

target_include_directories(test_ImageResampler PUBLIC ../include)
add_test(test_ImageResampler test_ImageResampler)
