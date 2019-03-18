add_executable(test_MaskExtractorFilter test_MaskExtractorFilter.cpp ../include/MaskExtractorFilter.h)
target_link_libraries(test_MaskExtractorFilter
	MuscateMetadata
	MetadataHelper
    "${Boost_LIBRARIES}"
    "${OTB_LIBRARIES}"
    "${OTBITK_LIBRARIES}"
)

target_include_directories(test_MaskExtractorFilter PUBLIC ../include)
add_test(test_MaskExtractorFilter test_MaskExtractorFilter)

add_executable(test_ComputeNDVI test_ComputeNDVI.cpp ../include/ComputeNDVI.h ../src/ComputeNDVI.cpp)
target_link_libraries(test_ComputeNDVI
	MuscateMetadata
	MetadataHelper
    "${Boost_LIBRARIES}"
    "${OTB_LIBRARIES}"
    "${OTBITK_LIBRARIES}"
)

target_include_directories(test_ComputeNDVI PUBLIC ../include)
add_test(test_ComputeNDVI test_ComputeNDVI)

add_executable(test_CreateS2AnglesRaster test_CreateS2AnglesRaster.cpp ../include/CreateS2AnglesRaster.h ../src/CreateS2AnglesRaster.cpp)
target_link_libraries(test_CreateS2AnglesRaster
	MuscateMetadata
	MetadataHelper
    "${Boost_LIBRARIES}"
    "${OTB_LIBRARIES}"
    "${OTBITK_LIBRARIES}"
)

target_include_directories(test_CreateS2AnglesRaster PUBLIC ../include)
add_test(test_CreateS2AnglesRaster test_CreateS2AnglesRaster)

add_executable(test_DirectionalCorrection test_DirectionalCorrection.cpp
				../include/DirectionalCorrection.h
				../include/DirectionalCorrectionFunctor.h
				../include/DirectionalModel.h
				../src/DirectionalModel.cpp
				../src/DirectionalCorrectionFunctor.txx
				../src/DirectionalCorrection.cpp)
target_link_libraries(test_DirectionalCorrection
	MuscateMetadata
	MetadataHelper
    "${Boost_LIBRARIES}"
    "${OTB_LIBRARIES}"
    "${OTBITK_LIBRARIES}"
)

target_include_directories(test_DirectionalCorrection PUBLIC ../include)
add_test(test_DirectionalCorrection test_DirectionalCorrection)
