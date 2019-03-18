set(WeightOnCloud_HEADERS
	include/CloudMaskBinarization.h
	include/CloudsInterpolation.h
	include/CloudWeightComputation.h
	include/CuttingImageFilter.h
	include/GaussianFilter.h
	include/PaddingImageHandler.h
	include/ROIImageFilter.h
)

otb_create_application(
  NAME           WeightOnClouds
  SOURCES        ${WeightOnCloud_HEADERS}
                 src/WeightOnClouds.cpp
  LINK_LIBRARIES MuscateMetadata MetadataHelper ${OTB_LIBRARIES})

target_include_directories(otbapp_WeightOnClouds PUBLIC include)
install(TARGETS otbapp_WeightOnClouds DESTINATION lib/otb/applications/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()