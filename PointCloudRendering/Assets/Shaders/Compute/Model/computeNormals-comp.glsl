vec3 computeNormal(uint faceIndex)
{
	vec3 p1 = vertexData[meshData[faceIndex].vertices.x].position;
	vec3 p2 = vertexData[meshData[faceIndex].vertices.y].position;
	vec3 p3 = vertexData[meshData[faceIndex].vertices.z].position;

	vec3 v = p2 - p1;
	vec3 w = p3 - p1;

	return normalize(cross(v, w));
}