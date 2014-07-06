/* ============================================================================
	SwFMODGeometry.cpp:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */


// Includes.
#include "SwFMOD.h"

/*-----------------------------------------------------------------------------
	FSwTriangulator
-----------------------------------------------------------------------------*/
struct FSwTriangulator
{
	TArray<FVector>		Points;
	TArray<INT>			Triangles;
	FBox				Box;

	bool Triangulate( UModel* model );
};


bool FSwTriangulator::Triangulate( UModel* model )
{
	guard(FSwTriangulator::Triangulate);

	// Triangulate convex polygons

	// Reset
	Points.Empty();
	Triangles.Empty();

	// Get points
	Points = model->Points;

	// Get faces
	FBspSurf surf;
	FBspNode node;
	for( TTransArray<FBspSurf>::TIterator it_surfs(model->Surfs); it_surfs; ++it_surfs )
	{
		surf = *it_surfs;
		FVector vert_adjust = FVector(0,0,0);//model->Vectors(surf.vNormal)*1.0;

		for( TArray<INT>::TIterator it_nodes(surf.Nodes); it_nodes; ++it_nodes )
		{
			node = model->Nodes(*it_nodes);

			// skip invalid nodes
			int vert_count = node.NumVertices;
			if( vert_count < 3 )
				continue;

			int vert_offset = node.iVertPool;

			unsigned int point1 = model->Verts(vert_offset).pVertex;
			unsigned int point2 = model->Verts(vert_offset+1).pVertex;
			unsigned int point3 = model->Verts(vert_offset+2).pVertex;

			Triangles.AddItem( point1 );
			Triangles.AddItem( point2 );
			Triangles.AddItem( point3 );

			Points( point1 ) += vert_adjust;
			Points( point2 ) += vert_adjust;
			Points( point3 ) += vert_adjust;

			for( int j=3; j!=vert_count; ++j )
			{
				point2 = point3;
				point3 = model->Verts(vert_offset+j).pVertex;

				Triangles.AddItem( point1 );
				Triangles.AddItem( point2 );
				Triangles.AddItem( point3 );

				Points( point3 ) += vert_adjust;
			}
		}
	}

	// Remove unused points

	// init translation table
	TArray<int> translation;
	translation.AddZeroed( Points.Num() );
	for( int i=0; i!=Points.Num(); ++i )
	{
		translation(i) = -1;

		// calc bounding box
		Box += Points(i);
	}

	// used points list
	TArray<FVector> points_list;

	INT point_old;
	INT point_new;
	INT point_trn;
	for( TArray<INT>::TIterator it_triangle(Triangles); it_triangle; ++it_triangle )
	{
		point_old = *it_triangle;
		point_trn = translation(point_old);

		if( point_trn == -1 )
		{
			// unique index
			point_new = points_list.Num();				// get new index
			translation(point_old) = point_new;			// save index translation
			points_list.AddItem( Points(point_old) );	// save point at new index
			*it_triangle = point_new;					// update Trangles item
		}
		else
		{
			// shared index
			*it_triangle = point_trn;					// update Trangles item
		}
	}

	if( Points.Num() != points_list.Num() )
	{
		SWF_LOG( SWF_NAME, TEXT("%s -- %s :: CULLED %d UNUSED POINTS! %d->%d"), SWF_PLOG, (Points.Num()-points_list.Num()), Points.Num(), points_list.Num() );
	}

	Points = points_list;


	return true;
	unguard;
}


/*-----------------------------------------------------------------------------
	USwFMOD
-----------------------------------------------------------------------------*/
void USwFMOD::LoadGeometry()
{
	guard(USwFMOD::LoadGeometry);
	FMOD_RESULT result;
	SWF_LOG( SWF_NAME, TEXT("%s -- %s :: "), SWF_PLOG );

	if( Geometry )
	{
		Geometry->setActive(false);
		Geometry->release();
		Geometry = NULL;
	}

	ULevel* level = Viewport->Actor->GetLevel();
	if( !level )
		return;

	FSwTriangulator tor;

	if( !tor.Triangulate(level->Model) )
		return;

	if( !tor.Box.IsValid )
		return;

	FLOAT worldsize = Max( tor.Box.Min.Size(), tor.Box.Max.Size() );
	SWF_LOG( SWF_NAME, TEXT("%s -- %s :: World size[%s] Points[%s] Triangles[%s]"), SWF_PLOG, *ToStr(worldsize), *ToStr(tor.Points.Num()), *ToStr(tor.Triangles.Num()) );
	SWF_FMOD_RCALL( System->setGeometrySettings(ToFMODFloat(worldsize)) );

	SWF_FMOD_RCALL( System->createGeometry(tor.Triangles.Num()/3, tor.Triangles.Num(), &Geometry) );
	for( TArray<INT>::TIterator it_triangle(tor.Triangles); it_triangle; ++it_triangle )
	{
		int index;
		FMOD_VECTOR vertices[3];

		vertices[2] = ToFMODVector(tor.Points(*it_triangle)); ++it_triangle;
		vertices[1] = ToFMODVector(tor.Points(*it_triangle)); ++it_triangle;
		vertices[0] = ToFMODVector(tor.Points(*it_triangle));
		//SWF_LOG( SWF_NAME, TEXT("%s -- %s :: Adding [%s] [%s] [%s]"), SWF_PLOG, *ToStr(vertices[0]), *ToStr(vertices[1]), *ToStr(vertices[2]) );
		SWF_FMOD_RCALL( Geometry->addPolygon( 0.75f, 0.0f, false, 3, vertices, &index) );
	}

	Geometry->setActive(true);
	unguard;
}


/*------------------------------------------------------------------------------------
	USwFMOD.
------------------------------------------------------------------------------------*/


UBOOL USwFMOD::IsObstructed( AActor* Actor )
{
	guard(USwFMOD::IsObstructed);
	//SWF_LOG( SWF_NAME, TEXT("%s -- %s :: "), SWF_PLOG );
	//FMOD_RESULT result;

	return 0;
	unguard;
}


void USwFMOD::RenderAudioGeometry( FSceneNode* Frame )
{
	guard(USwFMOD::RenderAudioGeometry);
	FMOD_RESULT result;

	if( !GRenderDevice || !Geometry || !StatRender )
		return;

	static TArray<FMOD_VECTOR> poly;
	static FPlane linecolor = FPlane(1,0,0,1);

	int polys;
	SWF_FMOD_RCALL( Geometry->getNumPolygons(&polys) );
	for( int i=0; i<polys; ++i )
	{
		int tris;
		SWF_FMOD_RCALL( Geometry->getPolygonNumVertices(i,&tris) );

		if( poly.Num() < tris )
			poly.Insert(0,tris-poly.Num());

		for( int j=0; j<tris; ++j )
		{
			SWF_FMOD_RCALL( Geometry->getPolygonVertex(i,j,&poly(j)) );
			if( j != 0 )
			{
				// TODO :: offset by normal to prevent z-artifacts
				GRenderDevice->Draw3DLine( Frame, linecolor, 0, ToUEVector(poly(j-1)), ToUEVector(poly(j)) );
			}
		}

		// TODO :: render normal
		// TODO :: don't render distant ones
	}

	unguard;
}


/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
