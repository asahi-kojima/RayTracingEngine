#pragma once
#include "object.h"
#include "ray.h"

class Object
{
public:
	Object() = default;
	Object(std::shared_ptr<Hittable>&& hittableObject);
	Object(const std::shared_ptr<Hittable>& hittableObject);
	~Object() = default;

	AABB getAABB() const { return mAABB; }
	const std::shared_ptr<Hittable>& getObject() const { return mGeometry; }

private:
	AABB mAABB;
	std::shared_ptr<Hittable> mGeometry;
};




struct Node
{
	Node() = default;
	Node(const Node&) = default;
	Node(Node&&) = default;
	Node(std::vector<std::unique_ptr<Object> >&& objectList);
	Node(std::vector<std::shared_ptr<Hittable> >&& hittableList);


	bool hit(const Ray& r, const f32 t_min, const f32 t_max, HitRecord& record);

private:
	bool isLeaf = false;
	AABB aabb;
	std::unique_ptr<Object> object = nullptr;

	std::unique_ptr<Node> lhs_node = nullptr;
	std::unique_ptr<Node> rhs_node = nullptr;

};