#pragma once

extern "C" void getSphereVertexData(	GLfloat spherePositionCoords[1146], 
										GLfloat sphereNormalCoords[1146], 
										GLfloat sphereTexCoords[764], 
										unsigned short sphereElements[2280]
									);

extern "C" unsigned int getNumberOfSphereVertices(void);

extern "C" unsigned int getNumberOfSphereElements(void);
