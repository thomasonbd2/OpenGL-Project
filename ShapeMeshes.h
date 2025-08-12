///////////////////////////////////////////////////////////////////////////////
// ShapeMeshes.h
// ============
// Defines the interface for rendering basic 3D shapes (box, sphere, cone, etc.)
//
// AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
// Created for CS-330-Computational Graphics and Visualization
///////////////////////////////////////////////////////////////////////////////

#pragma once

class ShapeMeshes
{
public:
    // constructor and destructor
    ShapeMeshes();
    ~ShapeMeshes();
    void DrawBox();

    // Shape rendering methods
    void RenderCube();
    void RenderBox();     
    void RenderCone();
    void RenderCylinder();
    void RenderPlane();
    void RenderPrism();
    void RenderSphere();
    void RenderTaperedCylinder();
    void RenderTorus();
};
