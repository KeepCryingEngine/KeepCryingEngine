#include "DebugDraw.h"

#include <GL/glew.h>

DebugDraw::DebugDraw()
{}

DebugDraw::~DebugDraw()
{}

void DebugDraw::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
{
	glBegin(GL_LINES);
	glLineWidth(20);
	glColor3f(color.getX(), color.getY(), color.getZ());
	glVertex3d(from.getX(), from.getY(), from.getZ());
	glVertex3d(to.getX(), to.getY(), to.getZ());
	glEnd();
}


void DebugDraw::drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color)
{}

void DebugDraw::reportErrorWarning(const char * warningString)
{}

void DebugDraw::draw3dText(const btVector3 & location, const char * textString)
{}

void DebugDraw::setDebugMode(int debugMode)
{	
	this->debugMode = debugMode;
}

int DebugDraw::getDebugMode() const
{
	return debugMode;
}
