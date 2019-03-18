otb_create_application(
  NAME          CompositePreprocessing
  SOURCES       src/CompositePreprocessing.cpp
          		include/ComputeNDVI.h
        		src/ComputeNDVI.cpp
        		include/MaskExtractorFilter.h
        		include/MaskExtractorFunctor.h
        		src/MaskExtractorFilter.txx
        		include/DirectionalCorrectionFunctor.h
        		src/DirectionalCorrectionFunctor.txx
        		src/DirectionalCorrection.cpp
        		include/DirectionalModel.h
        		src/DirectionalModel.cpp
        		include/DirectionalCorrection.h
        		include/DirectionalCorrectionFilter.h
        		include/CreateS2AnglesRaster.h
        		src/CreateS2AnglesRaster.cpp
        		src/PreprocessingAdapter.cpp
        		src/PreprocessingSentinel.cpp
        		src/PreprocessingVenus.cpp
  LINK_LIBRARIES ${OTB_LIBRARIES} MuscateMetadata MetadataHelper)

target_include_directories(otbapp_CompositePreprocessing PUBLIC include)

install(TARGETS otbapp_CompositePreprocessing DESTINATION lib/otb/applications/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

install(FILES scattering_coeffs_10m.txt scattering_coeffs_20m.txt DESTINATION share/)

