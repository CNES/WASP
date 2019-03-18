add_executable(test_MuscateMetadataReader test_MuscateMetadataReader.cpp)
target_link_libraries(test_MuscateMetadataReader
    MuscateMetadata
  	MetadataHelper
    "${Boost_LIBRARIES}")
add_test(test_MuscateMetadataReader test_MuscateMetadataReader)

add_executable(test_MuscateMetadataWriteRead test_MuscateMetadataWriteRead.cpp)
target_link_libraries(test_MuscateMetadataWriteRead
    MuscateMetadata
  	MetadataHelper    
    "${Boost_LIBRARIES}")
add_test(test_MuscateMetadataWriteRead test_MuscateMetadataWriteRead)

add_executable(test_MuscateMetadataReadWrite test_MuscateMetadataReadWrite.cpp)
target_link_libraries(test_MuscateMetadataReadWrite
    MuscateMetadata
  	MetadataHelper   
    "${Boost_LIBRARIES}")
add_test(test_MuscateMetadataReadWrite test_MuscateMetadataReadWrite)
