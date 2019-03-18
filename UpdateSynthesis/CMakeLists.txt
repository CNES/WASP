otb_create_application(
  NAME           UpdateSynthesis
  SOURCES        include/UpdateSynthesisFunctor.h src/UpdateSynthesisFunctor.txx src/UpdateSynthesis.cpp
  LINK_LIBRARIES MuscateMetadata MetadataHelper ${OTB_LIBRARIES})

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

target_include_directories(otbapp_UpdateSynthesis PUBLIC include)
install(TARGETS otbapp_UpdateSynthesis DESTINATION lib/otb/applications/)
