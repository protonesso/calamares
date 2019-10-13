calamares_add_plugin( unpackfs
    TYPE job
    EXPORT_MACRO PLUGINDLLEXPORT_PRO
    SOURCES
        UnpackFSJob.cpp
    LINK_PRIVATE_LIBRARIES
        calamares
    SHARED_LIB
)

if ( ECM_FOUND AND BUILD_TESTING )
    ecm_add_test(
            Tests.cpp
        TEST_NAME
            unpackfstest
        LINK_LIBRARIES
            calamares
            Qt5::Core
            Qt5::Test
    )
    calamares_automoc( unpackfstest )
endif()
