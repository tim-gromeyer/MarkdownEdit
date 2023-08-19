find_package(Nuspell QUIET)

if (Nuspell_FOUND AND Nuspell_VERSION GREATER_EQUAL 5.1.0)
    list(APPEND 3rdparty Nuspell::nuspell)
else()
    message(WARNING "Nuspell not found. Spell checker is disabled!")
    target_compile_definitions(markdownedit PUBLIC NO_SPELLCHECK)
endif()
