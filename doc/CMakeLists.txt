OPTION(BUILD_DOCS "Build the documentation" ON)


include(DocBookToHTML.cmake)
include(DocBookToPDF.cmake)
include(AnyToPNG.cmake)
include(SvgToPDF.cmake)

IF (BUILD_DOCS)
  FILE( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api/html)
  FILE( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/xml)

  #list the xml files:
  SET( XML_SOURCES
    xml/orocos-overview.xml xml/orocos-installation.xml
    xml/orocos-user-manual.xml xml/orocos-components-manual.xml
    xml/orocos-rtt-changes.xml xml/orocos-user-manual.xml xml/orocos-ocl-intro.xml
    xml/orocos-transports-corba.xml xml/orocos-transports-mqueue.xml
    xml/orocos-rtt-plugins.xml xml/orocos-typekit-plugin.xml xml/orocos-faq.xml )

  SEPARATE_ARGUMENTS(XML_SOURCES)

  DOCBOOK_TO_HTML( HTML_OUTPUT xsl/orocos-html.xsl 
    CATALOG
    ${CMAKE_CURRENT_BINARY_DIR}/xml/catalog.xml
    FILES 
    ${XML_SOURCES} )

  LIST(REMOVE_ITEM XML_SOURCES xml/orocos-faq.xml)

  DOCBOOK_TO_PDF( PDF_OUTPUT xsl/orocos-pdf.xsl
    CATALOG
    ${CMAKE_CURRENT_BINARY_DIR}/xml/catalog.xml
    FILES
    ${XML_SOURCES} )

  FILE( GLOB PNGS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/[^.]*.png )
  FILE( GLOB DIAS  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/[^.]*.dia )
  FILE( GLOB SVGS  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/[^.]*.svg )
  FILE( GLOB ICONS  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} xml/images/icons/[^.]*.svg )

  ANY_TO_PNG( RESULT_PNGS ${PNGS} ${DIAS} ${SVGS} ${ICONS})

  SVG_TO_PDF( RESULT_PDFS orocos_cheat_sheet.svg rtt_cheat_sheet.svg)
  set(RESULT_XML_PDFS ${CMAKE_CURRENT_BINARY_DIR}/xml/orocos_cheat_sheet.pdf ${CMAKE_CURRENT_BINARY_DIR}/xml/rtt_cheat_sheet.pdf)

  CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/xml/catalog.xml.in ${CMAKE_CURRENT_BINARY_DIR}/xml/catalog.xml IMMEDIATE @ONLY)
  CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/xml/orocos.ent.in ${CMAKE_CURRENT_BINARY_DIR}/xml/orocos.ent IMMEDIATE @ONLY)

  ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xml/orocos-html.css
	 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/orocos-html.css ${CMAKE_CURRENT_BINARY_DIR}/xml/orocos-html.css
	 DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/orocos-html.css
	 )

  ADD_CUSTOM_COMMAND(OUTPUT ${RESULT_XML_PDFS}
	 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/orocos_cheat_sheet.pdf ${CMAKE_CURRENT_BINARY_DIR}/xml/
	 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/rtt_cheat_sheet.pdf ${CMAKE_CURRENT_BINARY_DIR}/xml/
	 DEPENDS ${RESULT_PDFS}
	 )

  ADD_CUSTOM_TARGET(docpng DEPENDS ${RESULT_PNGS})
  ADD_CUSTOM_TARGET(dochtml DEPENDS ${HTML_OUTPUT} ${CMAKE_CURRENT_BINARY_DIR}/xml/orocos-html.css)
  ADD_CUSTOM_TARGET(docpdf DEPENDS ${PDF_OUTPUT} ${RESULT_XML_PDFS})
  ADD_DEPENDENCIES(dochtml docpng)
  ADD_DEPENDENCIES(docpdf docpng)

  ADD_CUSTOM_TARGET(message ALL echo "Use: make docapi dochtml docpdf to build the documentation.")

  INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/xml DESTINATION share/doc/liborocos-rtt )
  INSTALL(FILES ${RESULT_PDFS} DESTINATION share/doc/liborocos-rtt/xml OPTIONAL)
  IF (DOXYGEN)
    INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api DESTINATION share/doc/liborocos-rtt )
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtt.tag DESTINATION share/doc/liborocos-rtt OPTIONAL)
  ENDIF (DOXYGEN)
ENDIF (BUILD_DOCS)
