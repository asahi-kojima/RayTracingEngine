#include "node.h"

Node::Node(std::vector<std::unique_ptr<Object> >&& objectList)
{
	//���X�g�ɂP�����Ȃ��ꍇ�A�t�ƂȂ�B
	if (objectList.size() == 1)
	{
		isLeaf = true;
		object = std::move(objectList[0]);
		aabb = object->getAABB();
	}
	//�Q�ȏ�I�u�W�F�N�g������ꍇ�A�܂��������s���B
	else
	{
		std::vector<std::unique_ptr<Object> > objectListLhs(std::make_move_iterator(objectList.begin()), std::make_move_iterator(objectList.begin()) + objectList.size() / 2);
		std::vector<std::unique_ptr<Object> > objectListRhs(std::make_move_iterator(objectList.begin()) + objectList.size() / 2, std::make_move_iterator(objectList.end()));

		lhs_node = std::make_unique<Node>(std::move(objectListLhs));
		rhs_node = std::make_unique<Node>(std::move(objectListRhs));

		const AABB lhs_node_aabb = lhs_node->aabb;
		const AABB rhs_node_aabb = rhs_node->aabb;
		const AABB wrap_aabb = AABB::wraping(lhs_node_aabb, rhs_node_aabb);
	}
}

Node::Node(std::vector<std::shared_ptr<Hittable> >&& hittableList)
{
	//���X�g�ɂP�����Ȃ��ꍇ�A�t�ƂȂ�B
	if (hittableList.size() == 1)
	{
		isLeaf = true;
		std::unique_ptr<Object> pObject = std::make_unique<Object>(hittableList[0]);
		object = std::move(pObject);
		aabb = object->getAABB();
	}
	//�Q�ȏ�I�u�W�F�N�g������ꍇ�A�܂��������s���B
	else
	{
		std::vector<std::shared_ptr<Hittable> > objectListLhs(std::make_move_iterator(hittableList.begin()), std::make_move_iterator(hittableList.begin() + hittableList.size() / 2));
		std::vector<std::shared_ptr<Hittable> > objectListRhs(std::make_move_iterator(hittableList.begin() + hittableList.size() / 2), std::make_move_iterator(hittableList.end()));

		lhs_node = std::make_unique<Node>(std::move(objectListLhs));
		rhs_node = std::make_unique<Node>(std::move(objectListRhs));

		const AABB lhs_node_aabb = lhs_node->aabb;
		const AABB rhs_node_aabb = rhs_node->aabb;
		aabb = AABB::wraping(lhs_node_aabb, rhs_node_aabb);
	}
}

bool Node::hit(const Ray& r, const f32 t_min, const f32 t_max, HitRecord& record)
{
	//AABB�ƐڐG�����邩�m�F����B
	if (!isIntersecting(this->aabb, r, t_min, t_max))
	{
		return false;
	}

	//�ڐG������΁A���̓����Ƃ��������Ă���\��������̂ŁA
	//�����̃m�[�h�ɃA�N�Z�X���ɂ����B
	if (isLeaf)
	{
		const std::shared_ptr<Hittable>& pObject = object->getObject();
		return pObject->hit(r, t_min, t_max, record);
	}
	else
	{
		HitRecord lhsRecord;
		bool isHitLhs = lhs_node->hit(r, t_min, t_max, lhsRecord);
		HitRecord rhsRecord;
		bool isHitRhs = rhs_node->hit(r, t_min, t_max, rhsRecord);


		if (isHitLhs && isHitRhs)
		{
			if (lhsRecord.t < rhsRecord.t)
			{
				record = lhsRecord;
			}
			else
			{
				record = rhsRecord;
			}
		}
		else if (isHitLhs)
		{
			record = lhsRecord;
		}
		else if (isHitRhs)
		{
			record = rhsRecord;
		}
		else
		{
			return false;
		}



		return true;
	}
}


Object::Object(std::shared_ptr<Hittable>&& hittableObject)
{
	mGeometry = std::move(hittableObject);
	mAABB = mGeometry->calcAABB();
}

Object::Object(const std::shared_ptr<Hittable>& hittableObject)
{
	mGeometry = hittableObject;
	mAABB = mGeometry->calcAABB();
}
