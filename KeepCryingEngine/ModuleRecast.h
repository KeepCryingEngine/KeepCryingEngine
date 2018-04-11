#ifndef _MODULERECAST_H_
#define _MODULERECAST_H_

#include "Module.h"

#include <Detour\Include\DetourNavMesh.h>
#include <Detour\Include\DetourNavMeshQuery.h>
#include <Recast\Include\Recast.h>
#include <vector>

#include "GameObject.h"
#include "Mesh.h"
#include "MeshFilter.h"

class ModuleRecast : public Module
{
public:
	ModuleRecast();
	~ModuleRecast();

	bool Init() override;
	bool CleanUp() override;

	void MeshChanged();

	float GetCellSize()const;
	void SetCellSize(float value);
	float GetCellHeight()const;
	void SetCellHeight(float value);
	float GetAgentHeight()const;
	void SetAgentHeight(float value);
	float GetAgentRadius() const;
	void SetAgentRadius(float value);
	float GetAgentMaxClimb()const;
	void SetAgentMaxClimb(float value);
	float GetAgentMaxSlope()const;
	void SetAgentMaxSlope(float value);
	float GetRegionMinSize() const;
	void SetRegionMinSize(float value);
	float GetRegionMergeSize() const;
	void SetRegionMergeSize(float value);
	float GetEdgeMaxLen() const;
	void SetEdgeMaxLen(float value);
	float GetEdgeMaxError() const;
	void SetEdgeMaxError(float value);
	float GetVertsPerPoly() const;
	void SetVertsPerPoly(float value);
	float  GetDetailSampleDist() const;
	void SetDetailSampleDist(float value);
	float GetDetailSampleMaxError() const;
	void SetDetailSampleMaxError(float value);

private:
	bool BuildMesh();

private:
	std::vector<GameObject*> staticObjects;
	std::vector<Mesh*> staticMeshes;

	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcConfig m_cfg;
	rcPolyMeshDetail* m_dmesh;
//-------------------------------------
	class dtNavMesh* m_navMesh;
	class dtNavMeshQuery* m_navQuery;
	class dtCrowd* m_crowd;
//-------------------------------------
	unsigned char m_navMeshDrawFlags;

	float m_cellSize;
	float m_cellHeight;
	float m_agentHeight;
	float m_agentRadius;
	float m_agentMaxClimb;
	float m_agentMaxSlope;
	float m_regionMinSize;
	float m_regionMergeSize;
	float m_edgeMaxLen;
	float m_edgeMaxError;
	float m_vertsPerPoly;
	float m_detailSampleDist;
	float m_detailSampleMaxError;
	//int m_partitionType;

	//BuildContext* m_ctx;
};

#endif // !_MODULERECAST_H_

