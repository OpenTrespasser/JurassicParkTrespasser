/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/File/FileTest.cpp                                                 $
 * 
 * 1     11/06/96 12:01p Blee
 * 
 **********************************************************************************************/

#include <iostream>

#include "Spec.hpp"
#include "Image.hpp"
#include "Section.hpp"

struct SGeom
{
	int number_of_vertices;
	union 
	{
		char* vertices;
		TSymbolHandle sh_vertices;
	};
};

CArea* write_vertices(CSection* section, void* vertices, int size)
{
	CArea* v_area = section->reserve_area();
	//CArea* v_area = section->reserve_area(size); // alternate
	v_area->write_data(vertices, size);
	
	return v_area;
}

CArea* write_geom(CFileImage* fi, CSection* section, SGeom* geom)
{
	// Reserve area for geometry.
	CArea* g_area = section->reserve_area(sizeof(SGeom));
	//CArea* g_area = section->reserve_area(); // alternate

	// Write number of vertices.
	g_area->write_data(&geom->number_of_vertices, sizeof(geom->number_of_vertices));

	// Write vertices and get back reference.
	CArea* v_area = write_vertices(section, geom->vertices, geom->number_of_vertices);
	TSymbol* vsym = fi->create_symbol("Vertices_0001");
	v_area->assoc_symbol(vsym);

	// Write reference.
	g_area->write_reference(v_area);

	return g_area;
}

main()
{
	// Let's construct a geom type to write and read.

	SGeom geom;
	geom.number_of_vertices = 100;
	geom.vertices = new char[geom.number_of_vertices];
	char* p = geom.vertices;
	for (int i = 0; i < geom.number_of_vertices; ++i)
		*p++ = 'v';

	// WRITE
	
	// Open for writing, this file must not exist or will assert.
	CFileImage* file_image = new CFileImage("c:\\test.grof");
	
	// Create a section to store geom.
	CSection* section = file_image->create_section();

	// Write geometry.
	CArea* g_area = write_geom(file_image, section, &geom);

	// Create a symbol for this geometry.
	TSymbol* gsym = file_image->create_symbol("Geom_0001");
	g_area->assoc_symbol(gsym);

#if 0 // We can set a type here.
#define GEOM_TYPE 1
	gsym->set_dword(GEOM_TYPE);
#endif

	// Write out file image.
	file_image->write_out();

#ifdef _DEBUG
	file_image->dump();
#endif	
	delete file_image;

	// READ

#ifdef _DEBUG
	file_image = new CFileImage("c:\\test.grof");
#endif

#if 0 // Alternate method with types.
	symbol = file_image->get_symbol("Geom_0001");
	SGeom* pgeom = (SGeom*)file_image->get_data(symbol);
	assert(symbol->get_dword() == GEOM_TYPE);
#else
	gsym = file_image->get_symbol("Geom_0001");
	SGeom* pgeom = (SGeom*)file_image->get_data(gsym, true);
#endif
	file_image->dump();

	// Delete storage.

	delete [] geom.vertices;
	
	return 0;
}
