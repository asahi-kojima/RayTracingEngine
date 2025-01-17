#pragma once
#include "node.h"

class RayTracingEngine
{
public:
	RayTracingEngine() = default;

	void hit()
	{

	}

	void create_node(std::vector<std::unique_ptr<Object> >&& objectList)
	{
		//mRootNode = std::make_unique<Node>(std::move(objectList));
	}

private:
	std::unique_ptr<Node> mRootNode = nullptr;
};