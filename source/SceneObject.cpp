#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "SceneObject.h"
#include "Triangle.h"


SceneObject::~SceneObject()
{	//now use memory pool
	//for (auto& primitive : mPrimitives) {
	//	if (primitive) {
	//		delete primitive;
	//	}
	//}
}

bool SceneObject::Intersect(Ray ray, Intersection& isect) const
{
	bool hit = false;
	for (const auto& primitive : mPrimitives) {
		if (primitive->Intersect(ray, isect)) {
			ray.maxt = isect.t;
			hit = true;
		}
	}
	return hit;
}

void SceneObject::Sample(Vector3f& p, Vector3f& normal, float& pdf)
{
	// choose a primitive randomly
	int index = RandomInt(0, mPrimitives.size() - 1);
	mPrimitives[index]->Sample(p, normal, pdf);
	
	float totalArea = 0.0f;
	for (Primitive* mPrimitive : mPrimitives)
	{
		totalArea += mPrimitive->getArea();
	}
	float div = std::max(1e-5f, totalArea);
	pdf = pdf * mPrimitives[index]->getArea() / div;
}

void SceneObject::CreatePrimitivesFromMesh(const std::string& meshFilePath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshFilePath.c_str());

	if (!warn.empty())
	{
		std::cout << warn << std::endl;
	}

	if (!err.empty())
	{
		std::cout << err << std::endl;
	}

	if (!ret)
	{
		return;
	}

	// loop over shapes (objs "o")
	for (std::size_t s = 0; s < shapes.size(); ++s)
	{
		// loop over faces (each one is a triangle)
		std::size_t index_offset = 0;
		for (std::size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
		{
			std::size_t fv = std::size_t(shapes[s].mesh.num_face_vertices[f]);
			assert(fv == 3);

			Vector3f positions[3] = {};
			Vector3f normals[3] = {};
			Vector2f texcoords[3] = {};

			for (std::size_t v = 0; v < 3; ++v)
			{
				// vertices (triangles)
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				positions[v].x = attrib.vertices[3 * std::size_t(idx.vertex_index) + 0];
				positions[v].y = attrib.vertices[3 * std::size_t(idx.vertex_index) + 1];
				positions[v].z = attrib.vertices[3 * std::size_t(idx.vertex_index) + 2];

				// check if there is normal ( >=0; no normal if < 0)
				if (idx.normal_index >= 0)
				{
					normals[v].x = attrib.normals[3 * std::size_t(idx.normal_index) + 0];
					normals[v].y = attrib.normals[3 * std::size_t(idx.normal_index) + 1];
					normals[v].z = attrib.normals[3 * std::size_t(idx.normal_index) + 2];
				}

				// check if there is texcoord ( >=0; no texcoord if < 0)
				if (idx.texcoord_index >= 0)
				{
					texcoords[v].x = attrib.texcoords[2 * std::size_t(idx.texcoord_index) + 0];
					texcoords[v].y = attrib.texcoords[2 * std::size_t(idx.texcoord_index) + 1];
				}
			}
			index_offset += fv;
			CreatePrimitive<Triangle>(positions, texcoords, normals);
		}
	}

}
