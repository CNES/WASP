otb_create_application(
  NAME           WeightAOT
  SOURCES        src/WeightAOT.cpp src/WeightAOTComputation.cpp
  LINK_LIBRARIES MuscateMetadata MetadataHelper ${OTB_LIBRARIES})

target_include_directories(otbapp_WeightAOT PUBLIC include)
install(TARGETS otbapp_WeightAOT DESTINATION lib/otb/applications/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()