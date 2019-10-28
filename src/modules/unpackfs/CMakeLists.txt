set(_src UnpackFSJob.cpp Workers.cpp)

calamares_add_plugin( unpackfs
    TYPE job
    EXPORT_MACRO PLUGINDLLEXPORT_PRO
    SOURCES ${_src}
    LINK_PRIVATE_LIBRARIES
        calamares
    SHARED_LIB
)

if ( ECM_FOUND AND BUILD_TESTING )
    ecm_add_test(
            Tests.cpp
            ${_src}
        TEST_NAME
            unpackfstest
        LINK_LIBRARIES
            calamares
            Qt5::Core
            Qt5::Test
    )
    calamares_automoc( unpackfstest )
endif()
