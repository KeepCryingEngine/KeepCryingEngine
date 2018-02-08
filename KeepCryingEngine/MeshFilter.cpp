#include "MeshFilter.h"



MeshFilter::MeshFilter()
{
}


MeshFilter::~MeshFilter()
{
}

void MeshFilter::DrawUI()
{
}

std::vector<ComponentType> MeshFilter::GetProhibitedComponents() const
{
	return { ComponentType::MeshFilter };
}

MeshEntity * MeshFilter::GetMesh() const
{
	return mesh;
}
